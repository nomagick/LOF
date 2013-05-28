#include "LOF_openfetion.h"
#include "LOF_extra.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

int LOF_GLOBAL_CommandId = 0;
LOF_TOOL_StopWatchType* LOF_TOOL_StopWatch_new(){
	LOF_TOOL_StopWatchType* the_watch = (LOF_TOOL_StopWatchType*)malloc(sizeof(LOF_TOOL_StopWatchType));
	the_watch->begin = (time_t)0;
	the_watch->end = (time_t)0;
	return the_watch;
}
void LOF_TOOL_StopWatch_start(LOF_TOOL_StopWatchType* the_watch){
	the_watch->begin = time(NULL);
	the_watch->end = (time_t)0;
	return;
}
void LOF_TOOL_StopWatch_stop(LOF_TOOL_StopWatchType* the_watch){
	the_watch->end =  time(NULL);
	return;
}
int LOF_TOOL_StopWatch_read(LOF_TOOL_StopWatchType* the_watch){
	if (the_watch->end == (time_t)0) return ((int)((time(NULL)-the_watch->begin)));else
	return (int)(the_watch->end - the_watch->begin);
}
void LOF_TOOL_AutoKeepAlive(LOF_DATA_LocalUserType* user,LOF_TOOL_StopWatchType* the_watch,int waitsec){
	if (the_watch == NULL){LOF_debug_error("Fatal !  No Keep Alive Clock !"); return;}
	if (the_watch->begin == (time_t)0) {
		LOF_TOOL_StopWatch_start(the_watch);
		LOF_DATA_LocalUser_keep_alive(user);
		return;}
	if (LOF_TOOL_StopWatch_read(the_watch) > waitsec){
		LOF_TOOL_StopWatch_start(the_watch);
		LOF_DATA_LocalUser_keep_alive(user);
	}
	return;
}

LOF_TOOL_CommandType* LOF_TOOL_Command_new(LOF_TOOL_Command_InstructionType command,const char* pram1,const char* pram2,int outerID){
//	if (pram1==NULL) return NULL;
	LOF_TOOL_CommandType* The_Command = (LOF_TOOL_CommandType*)malloc(sizeof(LOF_TOOL_CommandType));
	memset(The_Command,0,sizeof(LOF_TOOL_CommandType));
	The_Command->Status = LOF_COMMAND_STATUS_NOT_HANDLED;
	if (outerID > 0)The_Command->CommandId = outerID; else The_Command->CommandId = --LOF_GLOBAL_CommandId;
	The_Command->Progress = 0;
	The_Command->Retry = 0;
	The_Command->Callid = 0;
	The_Command->Instruction = command;
	The_Command->Timer = LOF_TOOL_StopWatch_new();
	LOF_TOOL_StopWatch_start(The_Command->Timer);
	The_Command->Pram1 = (char*)malloc(strlen(pram1)+2);
	memset(The_Command->Pram1,0,sizeof(The_Command->Pram1));
	strcpy(The_Command->Pram1,pram1);
	if (pram2 != NULL){
	The_Command->Pram2 = (char*)malloc(strlen(pram2)+2);
	memset(The_Command->Pram2,0,sizeof(The_Command->Pram2));
	strcpy(The_Command->Pram2,pram2);
	}
	The_Command->BackSipMsg = NULL;
//	LOF_debug_info("Built A New Command, Which Has Id %d.",The_Command->CommandId);
	return The_Command;
}
void LOF_TOOL_Command_destroy(LOF_TOOL_CommandType* The_Command){
	if (The_Command == NULL) return;
	if (The_Command->BackSipMsg != NULL) free(The_Command->BackSipMsg);
	free(The_Command->Timer);
	if (The_Command->Pram2 != NULL) free(The_Command->Pram1);
	if (The_Command->Pram2 != NULL) free(The_Command->Pram2);
	return;

}
int LOF_TOOL_Command_exec_send_sms(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* ConversationList){
	if(The_Command->Pram2 ==NULL||The_Command->Pram1 ==NULL||ConversationList == NULL) {
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				The_Command->Progress = -1;
				return -1;
			}
	if (The_Command->Progress == 0 && The_Command->Status == LOF_COMMAND_STATUS_NOT_HANDLED){

		LOF_SIP_SipHeaderType *toheader , *eheader , *aheader;
			char* res;
			char* xml;
			LOF_DATA_LocalUserType *user = ((LOF_USER_ConversationType*)(ConversationList->data))->currentUser;
			LOF_SIP_FetionSipType* sip = user->sip;
			char astr[256];
			char* sipuri = The_Command->Pram1;

			LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
			toheader = LOF_SIP_SipHeader_new("T" , sipuri);
			eheader  = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SENDCATMESSAGE);
			LOF_SIP_FetionSip_add_header(sip , toheader);
			if(user->verification != NULL){
				sprintf(astr , "Verify algorithm=\"picc\",chid=\"%s\",response=\"%s\""
						, user->verification->guid
						, user->verification->code);
				aheader = LOF_SIP_SipHeader_new("A" , astr);
				LOF_SIP_FetionSip_add_header(sip , aheader);
			}
			LOF_SIP_FetionSip_add_header(sip , eheader);
			res = LOF_SIP_to_string(sip , The_Command->Pram2);

			LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
			free(res);
			The_Command->Callid = sip->callid;
			The_Command->Progress = 1 ;
			The_Command->Status = LOF_COMMAND_STATUS_ON_HOLD;
			return 1;
	}

	if (The_Command->Progress == 1 && The_Command->BackSipMsg != NULL)
	{
		char* xml;
		int daycount,monthcount;
			if(LOF_SIP_get_code(The_Command->BackSipMsg) == 280){
				xml = strstr(The_Command->BackSipMsg , "\r\n\r\n") + 4;
				LOF_USER_Conversation_parse_send_sms(xml , &daycount , &monthcount);
				((LOF_USER_ConversationType*)(ConversationList->data))->currentUser->smsDayCount = daycount;
				((LOF_USER_ConversationType*)(ConversationList->data))->currentUser->smsMonthCount = monthcount;
				free (The_Command->BackSipMsg);
				The_Command->BackSipMsg = NULL;
				LOF_debug_info("Sent a message to (%s)`s mobile phone" , The_Command->Pram1);
				The_Command->Progress = 2 ;
				The_Command->Status = LOF_COMMAND_STATUS_SUCCESS;
				return 2;
			}else{
				LOF_debug_error("Send a message to (%s)`s mobile phone failed",
						The_Command->Pram1);
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				The_Command->Progress = -1;
				return -1;
			}
	}

	return 0;
}
int LOF_TOOL_Command_exec_send_msg(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* ConversationList){
		if(The_Command->Pram2 ==NULL||ConversationList == NULL) {
			The_Command->Status = LOF_COMMAND_STATUS_FAIL;
			The_Command->Progress = -1;
			return -1;
		}

		LOF_DATA_LocalUserType* CurUser = ((LOF_DATA_LocalUserType*)(((LOF_USER_ConversationType*)(ConversationList->data))->currentUser));

		LOF_USER_ConversationType* theconversation=NULL;
		LOF_TOOL_FxListType* ConversationListPtr = ConversationList;
						for(;;){
											if (strcmp(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip->sipuri,The_Command->Pram1) == 0 ){
											theconversation = (LOF_USER_ConversationType*)( ConversationListPtr->data);
											 break;
											}
											if (ConversationListPtr->next != ConversationListPtr)
												ConversationListPtr=ConversationListPtr->next;
											else {
												theconversation = NULL;
												break;
											}
										}

						if(The_Command->Progress == 0  && theconversation != NULL){
							if (theconversation->ready!=1 )
							{
							The_Command->Status = LOF_COMMAND_STATUS_ON_HOLD;

							return 0;
							}
							else {
								The_Command->Progress = 4;
								The_Command->Status = LOF_COMMAND_STATUS_ON_HOLD;

							}
						}
		if (The_Command->Progress != 0 && theconversation == NULL){
		The_Command->Status = LOF_COMMAND_STATUS_FAIL;
		The_Command->Progress = -1;
		return -1;
		}


		if (The_Command->Progress == 0 && The_Command->Status != LOF_COMMAND_STATUS_FAIL && theconversation == NULL){
			LOF_SIP_SipHeaderType* eheader;
			theconversation = LOF_USER_Conversation_new(CurUser,
									  The_Command->Pram1 , NULL);
			if (theconversation == NULL) LOF_debug_error("WTF");
			char* res;
			theconversation->currentSip = LOF_SIP_FetionSip_clone(CurUser->sip);
			memset(theconversation->currentSip->sipuri, 0 , sizeof(theconversation->currentSip->sipuri));
			strcpy(theconversation->currentSip->sipuri , theconversation->currentContact->sipuri);
			LOF_SIP_FetionSip_set_connection(theconversation->currentSip , NULL);
			LOF_TOOL_FxList_append(ConversationList,LOF_TOOL_FxList_new((LOF_USER_ConversationType*)theconversation));

			LOF_SIP_FetionSip_set_type(CurUser->sip , LOF_SIP_SERVICE);
			eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_STARTCHAT);
			LOF_SIP_FetionSip_add_header(CurUser->sip , eheader);
			res = LOF_SIP_to_string(CurUser->sip , NULL);
			LOF_CONNECTION_FetionConnection_send(CurUser->sip->tcp , res , strlen(res));
			free(res);
			The_Command->Progress = 1;
			The_Command->Callid = ((CurUser->sip->callid) );
			The_Command->Status = LOF_COMMAND_STATUS_ON_HOLD;
			return 1;
		}



		if (The_Command->Progress == 1 && The_Command->BackSipMsg != NULL && theconversation != NULL){
			char *res , *ip , *credential , auth[256] ;
				int port , ret;
				LOF_CONNECTION_FetionConnectionType* conn=LOF_CONNECTION_FetionConnection_new();

				LOF_SIP_FetionSipType* sip;
				LOF_SIP_SipHeaderType *eheader , *theader , *mheader , *nheader , *aheader;


				memset(auth , 0 , sizeof(auth));
				LOF_SIP_get_attr(The_Command->BackSipMsg , "A" , auth);
				if(auth==NULL){
					free(The_Command->BackSipMsg);
					The_Command->BackSipMsg = NULL;
					The_Command->Status = LOF_COMMAND_STATUS_FAIL;
					The_Command->Progress = -1;
					return -1;
				}
				LOF_SIP_get_auth_attr(auth , &ip , &port , &credential);
				free(The_Command->BackSipMsg); The_Command->BackSipMsg = NULL;
				ret = LOF_CONNECTION_FetionConnection_connect(conn, ip, port);
		//		if(ret == -1) ret = LOF_CONNECTION_FetionConnection_connect(conn, ip, 443);
				if(ret == -1){
					The_Command->Status = LOF_COMMAND_STATUS_FAIL;
					The_Command->Progress = -1;
					return -1;
				}
				/*set sip*/

				LOF_SIP_FetionSip_set_connection(theconversation->currentSip , conn);
				theconversation->ready = 0;
				free(ip); ip = NULL;


				/*register*/
				sip = theconversation->currentSip;
				LOF_SIP_FetionSip_set_type(sip , LOF_SIP_REGISTER);
				aheader = LOF_SIP_SipHeader_credential_new(credential);
				theader = LOF_SIP_SipHeader_new("K" , "text/html-fragment");
				mheader = LOF_SIP_SipHeader_new("K" , "multiparty");
				nheader = LOF_SIP_SipHeader_new("K" , "nudge");
				LOF_SIP_FetionSip_add_header(sip , aheader);
				LOF_SIP_FetionSip_add_header(sip , theader);
				LOF_SIP_FetionSip_add_header(sip , mheader);
				LOF_SIP_FetionSip_add_header(sip , nheader);
				res = LOF_SIP_to_string(sip , NULL);
				LOF_CONNECTION_FetionConnection_send(conn , res , strlen(res));
				The_Command->Callid = ((sip->callid));
				The_Command->Progress = 2;
				The_Command->Status = LOF_COMMAND_STATUS_ON_HOLD;
				free(res);res = NULL;
				free(credential); credential = NULL;
				return 2;
			}

		if (The_Command->Progress == 2 && The_Command->BackSipMsg != NULL){
				LOF_SIP_SipHeaderType* eheader;
				char* res;
				char* body;
					if (theconversation == NULL) {
						The_Command->Status = LOF_COMMAND_STATUS_FAIL;
						free(The_Command->BackSipMsg);
						The_Command->BackSipMsg = NULL;
						The_Command->Progress = -1;
						return -1;
					}

				LOF_SIP_FetionSip_set_type(((LOF_USER_ConversationType*)(theconversation))->currentSip , LOF_SIP_SERVICE);
				eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_INVITEBUDDY);
				LOF_SIP_FetionSip_add_header(((LOF_USER_ConversationType*)(theconversation))->currentSip , eheader);
				body = LOF_SIP_generate_invite_friend_body(((LOF_USER_ConversationType*)(theconversation))->currentSip->sipuri);
				res = LOF_SIP_to_string(((LOF_USER_ConversationType*)(theconversation))->currentSip , body);
				free(body); body = NULL;
				LOF_CONNECTION_FetionConnection_send(((LOF_USER_ConversationType*)(theconversation))->currentSip->tcp , res , strlen(res));
				free(res); res = NULL;
				The_Command->Callid = ((((LOF_USER_ConversationType*)(theconversation))->currentSip->callid));
				free(The_Command->BackSipMsg);
				The_Command->BackSipMsg = NULL;
				The_Command->Progress = 3;
				return 3;
		}


		if (The_Command->Progress == 3 && The_Command->BackSipMsg != NULL && theconversation != NULL){
			if(LOF_SIP_get_code(The_Command->BackSipMsg) == 200)	{
					free(The_Command->BackSipMsg);
					The_Command->BackSipMsg = NULL;
					The_Command->Progress = 4;
					if ((((LOF_USER_ConversationType*)(theconversation))->ready != 1)){
							return 4;
					}

				}else{
					free(The_Command->BackSipMsg);
					The_Command->BackSipMsg = NULL;
					The_Command->Status = LOF_COMMAND_STATUS_FAIL;
					The_Command->Progress = -1;
					return -1;
				}
		}

		if (The_Command->Progress == 4 && (((LOF_USER_ConversationType*)(theconversation))->ready >= 0)){
			if (theconversation == NULL){
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				The_Command->Progress = -1;
			return -1;
				}

			if (((LOF_USER_ConversationType*)(theconversation))->ready == 1){
				LOF_USER_Conversation_send_sms(((LOF_USER_ConversationType*)(theconversation)) , The_Command->Pram2);
				The_Command->Callid = ((((LOF_USER_ConversationType*)(theconversation))->currentSip->callid));
				The_Command->BackSipMsg = NULL;
				The_Command->Progress = 5;
				LOF_TOOL_StopWatch_start(The_Command->Timer);
				return 5;
			}
		}

		if (The_Command->Progress == 4 &&  theconversation != NULL){
			if ((((LOF_USER_ConversationType*)(theconversation))->ready < 0)){
			LOF_SIP_FetionSip_free((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) );
			free(((LOF_USER_ConversationType*)(ConversationListPtr->data))->timer);
			LOF_TOOL_FxList_del(&ConversationListPtr);
			The_Command->Status = LOF_COMMAND_STATUS_FAIL;
			The_Command->Progress = -1;
			return -1;
			}
		}

		if (The_Command->Progress == 5 && The_Command->BackSipMsg != NULL ){
			if(LOF_SIP_get_code(The_Command->BackSipMsg) == 200)
			{
				free(The_Command->BackSipMsg);
				The_Command->BackSipMsg = NULL;
				The_Command->Status = LOF_COMMAND_STATUS_SUCCESS;
				The_Command->Progress = 6 ;
				return 6;
			}
		}


		return 0;
}

int LOF_TOOL_Command_ack_sipc40 (LOF_TOOL_FxListType* Command_List, char* sipc40msg){
	int callid= -8964;
	if (Command_List == NULL || sipc40msg == NULL) return -1;
	LOF_SIP_parse_sipc(sipc40msg,&callid,NULL);
	for (;;){
		if (((LOF_TOOL_CommandType*)(Command_List->data))->Callid == callid){
			((LOF_TOOL_CommandType*)(Command_List->data))->BackSipMsg = sipc40msg;
	//		LOF_debug_info("Found Target Reply Of Command [%d] .",((LOF_TOOL_CommandType*)(Command_List->data))->CommandId);
			return 1;
		}
		if (Command_List->next == Command_List) break;
		else Command_List = Command_List->next;
	}

return 0;
}

int LOF_TOOL_Command_arrange(LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** Command_List,const char* command,const char* pram1,const char* pram2,int outerID){

	if (Command_List == NULL || command == NULL ) return -1;

	LOF_TOOL_FxListType* commandlistptr;
	LOF_TOOL_CommandType* commandptr;
	LOF_DATA_BuddyContactType* contactptr;
	if (command != NULL && strlen(command)<=0)command=NULL;
	if (pram1 != NULL && strlen(pram1)<=0)pram1=NULL;
	if (pram2 != NULL && strlen(pram2)<=0)pram2=NULL;

	if (strcmp(command,"MSG")==0){
		if (pram1==NULL || pram2 == NULL || command == NULL) {
					LOF_debug_error("Illegal Command, Will Not Arrange.");
					return -1;
				}
		if (strlen(pram1) <= 10 ) {
			contactptr = LOF_DATA_BuddyContact_list_find_by_sid(user->contactList,pram1);
		}
		else {
			if (strlen(pram1) == 11){
				contactptr = LOF_DATA_BuddyContact_list_find_by_mobileno(user->contactList,pram1);
			}
			else {
				contactptr = LOF_DATA_BuddyContact_list_find_by_sipuri(user->contactList,pram1);
			}
		}
		if (contactptr == NULL) {
			LOF_debug_error("Illegal Command, Will Not Arrange.");
			return -1;
		}
		commandptr = LOF_TOOL_Command_new(LOF_COMMAND_SEND_MSG,contactptr->sipuri,pram2,outerID);
		if (*Command_List == NULL){
			*Command_List = LOF_TOOL_FxList_new(commandptr);
		}else{
			LOF_TOOL_FxList_append(*Command_List,LOF_TOOL_FxList_new(commandptr));
		}
		LOF_debug_info("Arranged A New MSG Command, Which is NO. [%d]",commandptr->CommandId);
		return commandptr->CommandId;
	}

	if (strcmp(command,"SMS")==0) {
		if (pram1==NULL || pram2 == NULL || command == NULL) {
							LOF_debug_error("Illegal Command, Will Not Arrange.");
							return -1;
			}
		if (strlen(pram1) <= 10 ) {
					contactptr = LOF_DATA_BuddyContact_list_find_by_sid(user->contactList,pram1);
				}
				else {
					if (strlen(pram1) == 11){
						contactptr = LOF_DATA_BuddyContact_list_find_by_mobileno(user->contactList,pram1);
					}
					else {
						contactptr = LOF_DATA_BuddyContact_list_find_by_sipuri(user->contactList,pram1);
					}
				}
				if (contactptr == NULL) {
					if (strcmp(pram1,user->sId)==0 || (strcmp(pram1,user->sipuri)==0) || (strcmp(pram1,user->mobileno)==0)){
						commandptr = LOF_TOOL_Command_new(LOF_COMMAND_SEND_SMS,user->sipuri,pram2,outerID);
						LOF_debug_info("Sending SMS to self.");
						if (*Command_List == NULL){
						*Command_List = LOF_TOOL_FxList_new(commandptr);
						}else{
						LOF_TOOL_FxList_append(*Command_List,LOF_TOOL_FxList_new(commandptr));
						}
						LOF_debug_info("Arranged A New SMS Command, Which is NO. [%d]",commandptr->CommandId);
						return commandptr->CommandId;
					}

					LOF_debug_error("Illegal Command, Will Not Arrange.");
					return -1;
				}
				commandptr = LOF_TOOL_Command_new(LOF_COMMAND_SEND_SMS,contactptr->sipuri,pram2,outerID);
				if (*Command_List == NULL){
					*Command_List = LOF_TOOL_FxList_new(commandptr);
				}else{
					LOF_TOOL_FxList_append(*Command_List,LOF_TOOL_FxList_new(commandptr));
				}
				LOF_debug_info("Arranged A New SMS Command, Which is NO. [%d]",commandptr->CommandId);
				return commandptr->CommandId;
			}

	if (strcmp(command,"DUMP")==0) {
			if (pram1==NULL || command == NULL) {
								LOF_debug_error("Illegal Command, Will Not Arrange.");
								return -1;
				}
			if (strcmp(pram1,"STATUS")==0 ) {
				if (pram2 != NULL && strlen(pram2)>0)
				commandptr = LOF_TOOL_Command_new(LOF_COMMAND_DUMP_STATUS,pram1,pram2,outerID);
				else commandptr = LOF_TOOL_Command_new(LOF_COMMAND_DUMP_STATUS,pram1,"StatusFile",outerID);
				if (*Command_List == NULL){
				*Command_List = LOF_TOOL_FxList_new(commandptr);
				}else{
				LOF_TOOL_FxList_append(*Command_List,LOF_TOOL_FxList_new(commandptr));
				}
				LOF_debug_info("Arranged A New DUMP STATUS Command, Which is NO. [%d]",commandptr->CommandId);
				return commandptr->CommandId;
				}
			if (strcmp(pram1,"CONTACT")==0 ) {
							if (pram2 != NULL && strlen(pram2)>0)
							commandptr = LOF_TOOL_Command_new(LOF_COMMAND_DUMP_CONTACT,pram1,pram2,outerID);
							else commandptr = LOF_TOOL_Command_new(LOF_COMMAND_DUMP_CONTACT,pram1,"ContactFile",outerID);
							if (*Command_List == NULL){
							*Command_List = LOF_TOOL_FxList_new(commandptr);
							}else{
							LOF_TOOL_FxList_append(*Command_List,LOF_TOOL_FxList_new(commandptr));
							}
							LOF_debug_info("Arranged A New DUMP CONTACT Command, Which is NO. [%d]",commandptr->CommandId);
							return commandptr->CommandId;
							}
			if (strcmp(pram1,"MSG")==0 ) {
						if (pram2 != NULL && strlen(pram2)>0)
						commandptr = LOF_TOOL_Command_new(LOF_COMMAND_DUMP_MSG,pram1,pram2,outerID);
						else commandptr = LOF_TOOL_Command_new(LOF_COMMAND_DUMP_MSG,pram1,"MessageFile",outerID);
						if (*Command_List == NULL){
						*Command_List = LOF_TOOL_FxList_new(commandptr);
						}else{
						LOF_TOOL_FxList_append(*Command_List,LOF_TOOL_FxList_new(commandptr));
						}
						LOF_debug_info("Arranged A New DUMP MSG Command, Which is NO. [%d]",commandptr->CommandId);
						return commandptr->CommandId;
						}

						LOF_debug_error("Illegal Command, Will Not Arrange.");
						return -1;

				}

	if (strcmp(command,"EXIT")==0) exit(2);


	return 0;
}

int LOF_TOOL_Command_main(LOF_TOOL_FxListType** Command_List,LOF_TOOL_FxListType* ConversationList,LOF_TOOL_FxListType** MSG_List,int* ResultTunnel){
	if (Command_List == NULL || ConversationList == NULL ) return -1;
	if (*Command_List == NULL) return 1;
	int returnvalue=0;
	LOF_TOOL_FxListType* listptr=*Command_List;
	LOF_TOOL_FxListType* tempptr = NULL;
	char ResultTunnelBuff[PIPE_BUF+2];
	char strbuff[128];
	int execcount=0;
	memset(ResultTunnelBuff,0,sizeof(ResultTunnelBuff));
	for (;;){
		if (LOF_TOOL_StopWatch_read(((LOF_TOOL_CommandType*)(listptr->data))->Timer) > LOF_COMMAND_MAX_TIME ){
			((LOF_TOOL_CommandType*)(listptr->data))->Status = LOF_COMMAND_STATUS_FAIL;
			((LOF_TOOL_CommandType*)(listptr->data))->Progress = -1;
			LOF_debug_info("Command [%d] waited for too long, assume it to be a Failure.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
		}
		if (((LOF_TOOL_CommandType*)(listptr->data))->Status == LOF_COMMAND_STATUS_FAIL || ((LOF_TOOL_CommandType*)(listptr->data))->Status == LOF_COMMAND_STATUS_SUCCESS ){
			if (((LOF_TOOL_CommandType*)(listptr->data))->Retry == 0 && ((LOF_TOOL_CommandType*)(listptr->data))->Status == LOF_COMMAND_STATUS_FAIL  ){
				((LOF_TOOL_CommandType*)(listptr->data))->Retry = 1;
				((LOF_TOOL_CommandType*)(listptr->data))->Status = LOF_COMMAND_STATUS_NOT_HANDLED;
				((LOF_TOOL_CommandType*)(listptr->data))->Progress = 0;
				LOF_TOOL_StopWatch_start(((LOF_TOOL_CommandType*)(listptr->data))->Timer);
				LOF_debug_info("Arranging Failed Command [%d] To Retry.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
			}else {
				if (*ResultTunnel == -1) {
					(*ResultTunnel)=LOF_FIFO_open(((LOF_USER_ConversationType*)(ConversationList->data))->currentUser,"ResultTunnel",1);
					if (*ResultTunnel == -1) LOF_debug_error("WTF! %d",errno);
				}
				if (((LOF_TOOL_CommandType*)(listptr->data))->Status == LOF_COMMAND_STATUS_FAIL){
					LOF_debug_error("Command [%d] Failed Twice , Abandoning.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
					if (*ResultTunnel != -1){
						memset(strbuff,0,sizeof(strbuff));
						sprintf(strbuff,"%d,0;\n",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
						strcat(ResultTunnelBuff,strbuff);
					}
				}else{
					LOF_debug_info("Command [%d] Successfully Done , Removing From List.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
					if (*ResultTunnel != -1){
						memset(strbuff,0,sizeof(strbuff));
						sprintf(strbuff,"%d,1;\n",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
						strcat(ResultTunnelBuff,strbuff);
						}
				}
				if (((listptr)->pre == (listptr)) && ((listptr)->next != (listptr))){
						tempptr=listptr->next;
						*Command_List = listptr->next;
						(listptr->next)->pre = (listptr->next);
						LOF_TOOL_Command_destroy((LOF_TOOL_CommandType*)(listptr->data));
						LOF_TOOL_FxList_del(&listptr);
						listptr = tempptr;
//						LOF_debug_info("Destroyed Top Of List, Reset Pointer . Command Execute Loop Over.");
						continue;

								}

				if (((listptr)->pre == (listptr)) && ((listptr)->next == (listptr))) {
					*Command_List = NULL;
					LOF_TOOL_Command_destroy((LOF_TOOL_CommandType*)(listptr->data));
					LOF_TOOL_FxList_del(&listptr);
				LOF_debug_info("Destroyed Only Item Of List, Command Execute Loop Over, List Is Now Empty. ");
				returnvalue= 1;
				break;
				}
				if (((listptr)->next == (listptr)) && ((listptr)->pre != (listptr))){
					LOF_TOOL_Command_destroy(((LOF_TOOL_CommandType*)(listptr->data)));
					LOF_TOOL_FxList_del(&listptr);
//					LOF_debug_info("Destroyed Bottom Of List, Command Execute Loop Over.");
					returnvalue= 0;
					break;
				}
				LOF_TOOL_Command_destroy((LOF_TOOL_CommandType*)(listptr->data));
				tempptr=listptr->next;
				LOF_TOOL_FxList_del(&listptr);
				listptr=tempptr;
//				LOF_debug_info("Destroyed Normal Item Of List, Continuing.");
				continue;
			}
		}else{
				if (execcount >= LOF_MAX_CONCURRENT) {
					LOF_TOOL_StopWatch_start(((LOF_TOOL_CommandType*)(listptr->data))->Timer);
					if (listptr->next != listptr){
						listptr = listptr->next;
						}else break;
						continue;
					}

			if (((LOF_TOOL_CommandType*)(listptr->data))->Instruction == LOF_COMMAND_SEND_MSG ){
				if (LOF_TOOL_Command_exec_send_msg(((LOF_TOOL_CommandType*)(listptr->data)),ConversationList)==6) continue;
					}

			if (((LOF_TOOL_CommandType*)(listptr->data))->Instruction == LOF_COMMAND_SEND_SMS ){
				if (LOF_TOOL_Command_exec_send_sms(((LOF_TOOL_CommandType*)(listptr->data)),ConversationList)==2) continue;
					}
			if (((LOF_TOOL_CommandType*)(listptr->data))->Instruction == LOF_COMMAND_DUMP_MSG ){
				if (LOF_TOOL_Command_exec_dump_msg(((LOF_TOOL_CommandType*)(listptr->data)),*MSG_List,(((LOF_USER_ConversationType*)ConversationList->data)->currentUser))==1) continue;
					}
			if (((LOF_TOOL_CommandType*)(listptr->data))->Instruction == LOF_COMMAND_DUMP_STATUS ){
				if (LOF_TOOL_Command_exec_dump_status(((LOF_TOOL_CommandType*)(listptr->data)),((LOF_USER_ConversationType*)ConversationList->data)->currentUser)==1) continue;
					}
			if (((LOF_TOOL_CommandType*)(listptr->data))->Instruction == LOF_COMMAND_DUMP_CONTACT ){
				if (LOF_TOOL_Command_exec_dump_contact(((LOF_TOOL_CommandType*)(listptr->data)),((LOF_USER_ConversationType*)ConversationList->data)->currentUser)==1) continue;
				}
			execcount++;
		}
		if (listptr->next != listptr){
			listptr = listptr->next;
		}else break;

	}
	if ((*ResultTunnel) != -1){
		write(*ResultTunnel,ResultTunnelBuff,strlen(ResultTunnelBuff));
	}
	//	LOF_debug_info("Executed Bottom Of List, Which Is NO. [%d] , Command Execute Loop Over.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
	return returnvalue;
}

int LOF_CallBack_Message (LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List ,char* sipmsg){

	char from[64];
	memset(from , 0 , sizeof(from));
	LOF_SIP_get_attr(sipmsg , "F" , from);
	LOF_TOOL_Command_arrange(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentUser,Command_List,"MSG",from,sipmsg,0);
	LOF_debug_info("Received Message From %s. \n %s",from,strstr(sipmsg,"\r\n\r\n")+4);
	free(sipmsg);



return 0;
}
int LOF_CallBack_Invitation(LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List ,char* sipmsg){
	LOF_SIP_FetionSipType* conversationSip;
	char* sipuri;

	LOF_SIP_parse_invitation((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) , NULL , sipmsg
		, &conversationSip , &sipuri);
	LOF_TOOL_FxList_append(ConversationListPtr,LOF_TOOL_FxList_new( LOF_USER_Conversation_new(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentUser,
		 sipuri , conversationSip)));
	free(sipmsg);

return 0;
}

int LOF_CallBack_Presence(LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List ,char* sipmsg){
	LOF_DATA_BuddyContactType* ContactPtr;
	ContactPtr=LOF_SIP_parse_presence_body((strstr(sipmsg,"\r\n\r\n")) , ((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentUser);
	LOF_DATA_BuddyContact_list_free(ContactPtr);
	free(sipmsg);
return 0;
}


int LOF_File_prepare(LOF_DATA_LocalUserType* user,const char* fname){
	char fullname[256];
	int result;
	memset (fullname,0,sizeof(fullname));
	snprintf((char*)fullname, sizeof(fullname), "%s/%s",user->config->globalPath,fname);
	result = access((char*)fullname,06);
	if (result == EACCES) return -1;
	if (result == ENOENT) {
		LOF_debug_info("about to creat file %s.",fullname);
			if (creat((char*)fullname,0664) == -1) return -1;
			result = access((char*)fullname,06);
		}
	if (result == ENOENT || result == EACCES) return -1;

	return fclose (fopen((char*)fullname,"w+"));
}
int LOF_FIFO_prepare(LOF_DATA_LocalUserType* user,const char* fname){
	char fullname[256];
	int result;
	memset (fullname,0,sizeof(fullname));
	snprintf((char*)fullname, sizeof(fullname), "%s/%s",user->config->globalPath,fname);
	result = mkfifo((char*)fullname,0666);
	if (result == -1 && errno == EEXIST) {
		unlink((char*)fullname);
		result = mkfifo((char*)fullname,0664);
	}
	if (result == -1) {
		LOF_debug_error("Failed to creat Tunnle %s.",fullname);
		return -1;
		}
	LOF_debug_info("Created Tunnle %s.",fullname);
	return 0;
}
int LOF_FIFO_open(LOF_DATA_LocalUserType* user,const char* fname,int write){
	char fullname[256];
	memset (fullname,0,sizeof(fullname));
	snprintf((char*)fullname, sizeof(fullname), "%s/%s",user->config->globalPath,fname);
	if (write == 0)return open(fullname,O_RDONLY|O_NONBLOCK);
	else return open(fullname,O_WRONLY|O_NONBLOCK);
}
int LOF_TOOL_Command_ParseJson(int* fifo,LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** cmdlist){
	cJSON *rootJsonObject,*jsonObjectPtr,*jsonArrayPtr,*tmpJsonPtr;
	char *Ins,*Pram1,*Pram2,*jsonStr;
	int outerID,counter,arraysize,readresult;
	char fifoBuff1[PIPE_BUF+2],fifoBuff2[PIPE_BUF+2];
	char* extenedFifoBuff=NULL;
	if (*fifo == -1) return -1;
	memset(fifoBuff1,0,sizeof(fifoBuff1));
	memset(fifoBuff2,0,sizeof(fifoBuff2));
	readresult = read(*fifo,fifoBuff1,PIPE_BUF);
	if (readresult < 0 ) {
		close(*fifo);
		*fifo=-1;
		return -1;
	}else if (readresult == 0){
		return 0;
	}
	if (read(*fifo,fifoBuff2,PIPE_BUF) >0){
		extenedFifoBuff = (char*)malloc(8*PIPE_BUF+16);
		memset(extenedFifoBuff,0,sizeof(extenedFifoBuff));
		strcpy(extenedFifoBuff,fifoBuff1);
		strcat(extenedFifoBuff,fifoBuff2);
		jsonStr=extenedFifoBuff;
		memset(fifoBuff2,0,sizeof(fifoBuff2));
	}else {
		jsonStr=fifoBuff1;
	}
	counter = 2;
	while (read(*fifo,fifoBuff2,PIPE_BUF) >0){
		if (counter > 8) return -8;
		strcat(extenedFifoBuff,fifoBuff2);
		memset(fifoBuff2,0,sizeof(fifoBuff2));
		counter++;
	}

	rootJsonObject = cJSON_Parse(jsonStr);
	if (rootJsonObject == NULL) return -2;
	jsonArrayPtr = cJSON_GetObjectItem(rootJsonObject,"NEWCMD");
	/*do {*/
		if (jsonArrayPtr == NULL) return -3;
		arraysize = cJSON_GetArraySize(jsonArrayPtr);
		for (counter=0;counter < arraysize;counter++){
			jsonObjectPtr = cJSON_GetArrayItem(jsonArrayPtr,counter);
			if (jsonObjectPtr == NULL) continue;

			tmpJsonPtr = cJSON_GetObjectItem(jsonObjectPtr,"INS");
			if (tmpJsonPtr) Ins = tmpJsonPtr->valuestring; else Ins = NULL;
			tmpJsonPtr = cJSON_GetObjectItem(jsonObjectPtr,"PRAM1");
			if (tmpJsonPtr) Pram1 = tmpJsonPtr->valuestring; else Pram1 = NULL;
			tmpJsonPtr = cJSON_GetObjectItem(jsonObjectPtr,"PRAM2");

			if (tmpJsonPtr) Pram2 = tmpJsonPtr->valuestring; else Pram2 = NULL;

			tmpJsonPtr = cJSON_GetObjectItem(jsonObjectPtr,"ID");

			if (tmpJsonPtr) outerID = tmpJsonPtr->valueint; else outerID = 0;
			tmpJsonPtr = NULL;

			LOF_TOOL_Command_arrange(user,cmdlist,Ins,Pram1,Pram2,outerID);

		}

	/*cJSON_Delete(jsonArrayPtr);
	memset(jsonStr,0,sizeof(jsonStr));
	jsonStr = cJSON_Print(rootJsonObject);
	printf("!!!%s\n",jsonStr);
	cJSON_Delete(rootJsonObject);
	rootJsonObject = cJSON_Parse(jsonStr);
	jsonArrayPtr=NULL;
	jsonArrayPtr = cJSON_GetObjectItem(rootJsonObject,"NEWCMD");

	} while (jsonArrayPtr != NULL);*/
	cJSON_Delete(rootJsonObject);
	if (extenedFifoBuff != NULL)free(extenedFifoBuff);

	return 0;
}

int LOF_TOOL_Command_ParseStr(LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** cmdlist,char* str){
	char Instruction[6];
	char Pram1[1024];
	char Pram2[2048];
	char outerID[8];
	char* strptr1=str;
	char* strptr2=NULL;
	char* strptr3=NULL;
	char* idptr=NULL;
//

	strptr2 = strstr(str,":::");
	for(;;){
		if (strptr2 == NULL) break;
		memset (Instruction,0,sizeof(Instruction));
		memset (Pram1,0,sizeof(Pram1));
		memset (Pram2,0,sizeof(Pram2));
		memset (outerID,0,sizeof(outerID));
		for (;;){
		if ((char)(*strptr1) == '\n' || ((char)(*strptr1) == '\r') || ((char)(*strptr1) == ' ') || ((char)(*strptr1) == '\t')) strptr1++;
		else break;
		}
		strptr3=strstr(strptr1," ");
		if ((strptr3 != NULL) && (strptr1 < strptr2)){
			memcpy(Instruction,strptr1,(size_t)(strptr3-strptr1));
			strptr1=strptr3+1;
			strptr3=strstr(strptr1," ");
			if ((strptr3 != NULL) && (strptr3 < strptr2)) {
				memcpy(Pram1,strptr1,(size_t)(strptr3-strptr1));
				strptr1=strptr3+1;
				memcpy(Pram2,strptr1,(size_t)(strptr2-strptr1));
				//		LOF_debug_info("Result : %s : %s : %s ;\n\n",Instruction,Pram1,Pram2);
			}else{
			if (strptr1 < strptr2){
				memcpy(Pram1,strptr1,(size_t)(strptr2-strptr1));
			}
		}

		}else if (strptr1<strptr2){

		memcpy(Instruction,strptr1,(size_t)(strptr2-strptr1));

		}
		idptr = strstr(strptr2+3,";;;");
		strptr3=strptr2+3;
		strptr2=strstr(strptr2+3,":::");
		if (idptr!=NULL && strptr2==NULL){
		memcpy(outerID,strptr3,(size_t)(idptr-strptr3));
		strptr1=idptr+3;
		}else if (idptr!=NULL && strptr2!=NULL){
			if (strptr2>idptr){
				memcpy(outerID,strptr3,(size_t)(idptr-strptr3));
				strptr1=idptr+3;
			}

		}else strptr1=strptr3;
		if (strlen(outerID) > 0){
			LOF_TOOL_Command_arrange(user,cmdlist,Instruction,Pram1,Pram2,atoi(outerID));
		}else {
			LOF_TOOL_Command_arrange(user,cmdlist,Instruction,Pram1,Pram2,0);
		}
LOF_debug_info ("CMD--> %s %s %s %s",Instruction,Pram1,Pram2,outerID);
	}
	return 0;
}
int LOF_FILE_Status_dump(LOF_DATA_LocalUserType* user,FILE* filehandle){
	if (user == NULL || filehandle == NULL) return -1;
	cJSON* jsonObject;
	cJSON* rootJsonObject;
	cJSON* jsonArrayObject;
	char* string;
	LOF_DATA_BuddyContactType* cl_cur;
	int count=0;
	//	LOF_debug_info("Doing Find By Sid, which is %s.",sid);
	jsonArrayObject=cJSON_CreateArray();
	rootJsonObject=cJSON_CreateObject();
		foreach_contactlist(user->contactList , cl_cur){
			if(cl_cur->status > 0){
				jsonObject=cJSON_CreateObject();
				cJSON_AddStringToObject(jsonObject,"URI",	cl_cur->sipuri);
				cJSON_AddStringToObject(jsonObject,"MOBILE",	cl_cur->mobileno);
				cJSON_AddStringToObject(jsonObject,"SID",	cl_cur->sId);
				cJSON_AddStringToObject(jsonObject,"NAME",	cl_cur->nickname);
				cJSON_AddStringToObject(jsonObject,"LOCALNAME",	cl_cur->localname);
				cJSON_AddNumberToObject(jsonObject,"STATUS",	cl_cur->status);
				cJSON_AddStringToObject(jsonObject,"PHRASE",	cl_cur->impression);
				cJSON_AddStringToObject(jsonObject,"STATUSPHRASE",	cl_cur->statusPhrase);
				cJSON_AddItemToArray(jsonArrayObject,jsonObject);
				count++;
			}

		}

		cJSON_AddItemToObject(rootJsonObject,"ONLINEBUDDY",jsonArrayObject);
		string=cJSON_Print(rootJsonObject);
		cJSON_Delete(rootJsonObject);
		fprintf(filehandle,"%s\n",string);
		free(string);

	return count;
}
/*int LOF_FILE_Contact_dump(LOF_DATA_LocalUserType* user,FILE* filehandle){
	if (user == NULL || filehandle == NULL) return -1;
	cJSON* jsonObject;
	cJSON* rootJsonObject;
	cJSON* jsonArrayObject;
	cJson* rootArrayObject;
	char* string;
	LOF_DATA_BuddyContactType* cl_cur;
	int count=0;
	//	LOF_debug_info("Doing Find By Sid, which is %s.",sid);
	jsonArrayObject=cJSON_CreateArray();
	rootJsonObject=cJSON_CreateObject();
		foreach_contactlist(user->contactList , cl_cur){

				jsonObject=cJSON_CreateObject();
				cJSON_AddStringToObject(jsonObject,"URI",	cl_cur->sipuri);
				cJSON_AddStringToObject(jsonObject,"MOBILE",	cl_cur->mobileno);
				cJSON_AddStringToObject(jsonObject,"SID",	cl_cur->sId);
				cJSON_AddStringToObject(jsonObject,"NAME",	cl_cur->nickname);
				cJSON_AddStringToObject(jsonObject,"LOCALNAME",	cl_cur->localname);
	//			cJSON_AddStringToObject(jsonObject,"BIRTH",	cl_cur->birthday);
				cJSON_AddStringToObject(jsonObject,"CARRIER",	cl_cur->carrier);
	//			cJSON_AddStringToObject(jsonObject,"CITY",	cl_cur->city);
	//			cJSON_AddStringToObject(jsonObject,"COUNTRY",	cl_cur->country);
	//			cJSON_AddStringToObject(jsonObject,"PROVINCE",	cl_cur->province);
				cJSON_AddStringToObject(jsonObject,"DEVICETYPE",	cl_cur->devicetype);
		//		cJSON_AddNumberToObject(jsonObject,"GENDER",	cl_cur->gender);
				cJSON_AddNumberToObject(jsonObject,"GROUPID",	cl_cur->groupid);
				cJSON_AddStringToObject(jsonObject,"GROUPIDS",	cl_cur->groupids);
				cJSON_AddNumberToObject(jsonObject,"SERVICESTATUS",	cl_cur->serviceStatus);
				cJSON_AddNumberToObject(jsonObject,"STATUS",	cl_cur->status);
				cJSON_AddStringToObject(jsonObject,"PHRASE",	cl_cur->impression);
				cJSON_AddStringToObject(jsonObject,"STATUSPHRASE",	cl_cur->statusPhrase);
				cJSON_AddItemToArray(jsonArrayObject,jsonObject);
				count++;


		}

		cJSON_AddItemToObject(rootJsonObject,"CONTACT",jsonArrayObject);
		string=cJSON_Print(rootJsonObject);
		cJSON_Delete(rootJsonObject);
		fprintf(filehandle,"%s\n",string);
		free(string);

	return count;
}
*/
int LOF_FILE_Contact_dump(LOF_DATA_LocalUserType* user,FILE* filehandle){
	if (user == NULL || filehandle == NULL || user->groupList == NULL) return -1;
	cJSON* jsonObject;
	cJSON* rootJsonObject;
	cJSON* groupJsonObject;
	cJSON* jsonArrayObject;
	cJSON* rootArrayObject;
	char* string;
	LOF_DATA_BuddyContactType* cl_cur;
	LOF_DATA_BuddyGroupType* groupPtr = user->groupList;
	int count=0;
	//	LOF_debug_info("Doing Find By Sid, which is %s.",sid);

	rootJsonObject=cJSON_CreateObject();
	rootArrayObject = cJSON_CreateArray();
	for(;;){
		groupJsonObject=cJSON_CreateObject();
		jsonArrayObject=cJSON_CreateArray();

		foreach_contactlist(user->contactList , cl_cur){

			if (cl_cur->groupid == groupPtr->groupid){
						jsonObject=cJSON_CreateObject();
						cJSON_AddStringToObject(jsonObject,"URI",	cl_cur->sipuri);
						cJSON_AddStringToObject(jsonObject,"MOBILE",	cl_cur->mobileno);
						cJSON_AddStringToObject(jsonObject,"SID",	cl_cur->sId);
						cJSON_AddStringToObject(jsonObject,"NAME",	cl_cur->nickname);
						cJSON_AddStringToObject(jsonObject,"LOCALNAME",	cl_cur->localname);
			//			cJSON_AddStringToObject(jsonObject,"BIRTH",	cl_cur->birthday);
						cJSON_AddStringToObject(jsonObject,"CARRIER",	cl_cur->carrier);
			//			cJSON_AddStringToObject(jsonObject,"CITY",	cl_cur->city);
			//			cJSON_AddStringToObject(jsonObject,"COUNTRY",	cl_cur->country);
			//			cJSON_AddStringToObject(jsonObject,"PROVINCE",	cl_cur->province);
						cJSON_AddStringToObject(jsonObject,"DEVICETYPE",	cl_cur->devicetype);
				//		cJSON_AddNumberToObject(jsonObject,"GENDER",	cl_cur->gender);
						cJSON_AddNumberToObject(jsonObject,"GROUPID",	cl_cur->groupid);
						cJSON_AddStringToObject(jsonObject,"GROUPIDS",	cl_cur->groupids);
						cJSON_AddNumberToObject(jsonObject,"SERVICESTATUS",	cl_cur->serviceStatus);
						cJSON_AddNumberToObject(jsonObject,"STATUS",	cl_cur->status);
						cJSON_AddStringToObject(jsonObject,"PHRASE",	cl_cur->impression);
						cJSON_AddStringToObject(jsonObject,"STATUSPHRASE",	cl_cur->statusPhrase);
						cJSON_AddItemToArray(jsonArrayObject,jsonObject);
						count++;
				}

				}
		cJSON_AddStringToObject(groupJsonObject,"GROUPNAME",groupPtr->groupname);
		cJSON_AddItemToObject(groupJsonObject,"CONTACT",jsonArrayObject);
		cJSON_AddItemToArray(rootArrayObject,groupJsonObject);

		if (groupPtr->next == groupPtr || groupPtr->next == user->groupList) break;else
		groupPtr = groupPtr -> next;
	}
		cJSON_AddItemToObject(rootJsonObject,"BUDDYLIST",rootArrayObject);
		string=cJSON_Print(rootJsonObject);
		cJSON_Delete(rootJsonObject);
		fprintf(filehandle,"%s\n",string);
		free(string);

	return count;
}
int LOF_FILE_MSG_dump(LOF_TOOL_FxListType* msg_list,FILE* filehandle){
	if (filehandle == NULL) return -1;
	if (msg_list == NULL) return 0;
 	char timestr[64];
 	char* string;
	int msgcounter = 1;
	LOF_TOOL_FxListType* msglistptr=msg_list;
	LOF_DATA_FetionMessageType* msgitemptr=NULL;
	cJSON* jsonObject;
	cJSON* rootJsonObject;
	cJSON* jsonArrayObject;
	jsonArrayObject=cJSON_CreateArray();
	rootJsonObject=cJSON_CreateObject();
	for (;;){
		jsonObject=cJSON_CreateObject();
		msgitemptr = ((LOF_DATA_FetionMessageType*)(msglistptr->data));
		memset(timestr,0,sizeof(timestr));
		if (msgitemptr->sendtime.tm_year){

			snprintf(timestr,sizeof(timestr),"%d-%d-%d %2d:%2d:%2d",(msgitemptr->sendtime.tm_year + 1900),(msgitemptr->sendtime.tm_mon + 1),(msgitemptr->sendtime.tm_mday),(msgitemptr->sendtime.tm_hour),(msgitemptr->sendtime.tm_min),(msgitemptr->sendtime.tm_sec));

		}
		if (msgitemptr->sipuri != NULL)cJSON_AddStringToObject(jsonObject,"URI",msgitemptr->sipuri);
		if (msgitemptr->pguri != NULL)cJSON_AddStringToObject(jsonObject,"GRI",	msgitemptr->pguri);
		cJSON_AddStringToObject(jsonObject,"MSG",msgitemptr->message);
		cJSON_AddStringToObject(jsonObject,"TIME",timestr);
		cJSON_AddItemToArray(jsonArrayObject,jsonObject);
		if (msglistptr->next == msglistptr) break;
		msglistptr = msglistptr->next;
		msgcounter++;
	}
	cJSON_AddItemToObject(rootJsonObject,"NEWMSG",jsonArrayObject);
	string=cJSON_Print(rootJsonObject);
	cJSON_Delete(rootJsonObject);
	fprintf(filehandle,"%s\n",string);
	free(string);
	return msgcounter;
}


int LOF_TOOL_Command_exec_dump_status(LOF_TOOL_CommandType* The_Command,LOF_DATA_LocalUserType* User){
	if(The_Command->Pram1 ==NULL||The_Command->Pram2 ==NULL||User == NULL) {
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				The_Command->Progress = -1;
				return -1;
			}
	if (The_Command->Progress == 0 && The_Command->Status == LOF_COMMAND_STATUS_NOT_HANDLED){
		FILE* StatusFile=NULL;
		char fullname[256];
		memset (fullname,0,sizeof(fullname));
		snprintf(fullname,sizeof(fullname),"%s/%s",User->config->globalPath,The_Command->Pram2);
		StatusFile=fopen(fullname,"w");
		if (StatusFile != NULL){
			if (LOF_FILE_Status_dump(User,StatusFile) > 0){
				fclose(StatusFile);
				The_Command->Progress = 1 ;
				The_Command->Status = LOF_COMMAND_STATUS_SUCCESS;
				return 1;
			}else{
				fclose(StatusFile);
				The_Command->Progress = -1 ;
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				return -1;
			}
		}else {
			The_Command->Progress = -1 ;
			The_Command->Status = LOF_COMMAND_STATUS_FAIL;
			return -1;
		}
	}
	return 0;
}
int LOF_TOOL_Command_exec_dump_contact(LOF_TOOL_CommandType* The_Command,LOF_DATA_LocalUserType* User){
	if(The_Command->Pram1 ==NULL||The_Command->Pram2 ==NULL||User == NULL) {
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				The_Command->Progress = -1;
				return -1;
			}
	if (The_Command->Progress == 0 && The_Command->Status == LOF_COMMAND_STATUS_NOT_HANDLED){
		FILE* ContactFile=NULL;
		char fullname[256];
		memset (fullname,0,sizeof(fullname));
		snprintf(fullname,sizeof(fullname),"%s/%s",User->config->globalPath,The_Command->Pram2);
		ContactFile=fopen(fullname,"w");
		if (ContactFile != NULL){
			if (LOF_FILE_Contact_dump(User,ContactFile) > 0){
				fclose(ContactFile);
				The_Command->Progress = 1 ;
				The_Command->Status = LOF_COMMAND_STATUS_SUCCESS;
				return 1;
			}else{
				fclose(ContactFile);
				The_Command->Progress = -1 ;
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				return -1;
			}
		}else {
			The_Command->Progress = -1 ;
			The_Command->Status = LOF_COMMAND_STATUS_FAIL;
			return -1;
		}
	}
	return 0;
}
int LOF_TOOL_Command_exec_dump_msg(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* MSGList,LOF_DATA_LocalUserType* User){
	if(The_Command->Pram1 ==NULL||The_Command->Pram2 ==NULL) {
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				The_Command->Progress = -1;
				return -1;
			}
	if (The_Command->Progress == 0 && The_Command->Status == LOF_COMMAND_STATUS_NOT_HANDLED){
		FILE* MessageFile=NULL;
		char fullname[256];
		memset (fullname,0,sizeof(fullname));
		snprintf(fullname,sizeof(fullname),"%s/%s",User->config->globalPath,The_Command->Pram2);
		MessageFile=fopen(fullname,"w");
		if (MessageFile != NULL){
			if (LOF_FILE_MSG_dump(MSGList,MessageFile) >= 0){
				fclose(MessageFile);
				The_Command->Progress = 1 ;
				The_Command->Status = LOF_COMMAND_STATUS_SUCCESS;
				return 1;
			}else{
				fclose(MessageFile);
				The_Command->Progress = -1 ;
				The_Command->Status = LOF_COMMAND_STATUS_FAIL;
				return -1;
			}
		}else {
			The_Command->Progress = -1 ;
			The_Command->Status = LOF_COMMAND_STATUS_FAIL;
			return -1;
		}
	}
	return 0;
}
