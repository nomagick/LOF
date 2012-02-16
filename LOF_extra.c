#include "LOF_openfetion.h"
#include "LOF_extra.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int LOF_GLOBAL_CommandId = 1;
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

LOF_TOOL_CommandType* LOF_TOOL_Command_new(LOF_TOOL_Command_InstructionType command,const char* pram1,const char* pram2){
//	if (pram1==NULL) return NULL;
	LOF_TOOL_CommandType* The_Command = (LOF_TOOL_CommandType*)malloc(sizeof(LOF_TOOL_CommandType));
	memset(The_Command,0,sizeof(LOF_TOOL_CommandType));
	The_Command->Status = LOF_COMMAND_STATUS_NOT_HANDLED;
	The_Command->CommandId = LOF_GLOBAL_CommandId++;
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

int LOF_TOOL_Command_arrange(LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** Command_List,const char* command,const char* pram1,const char* pram2){
	if (Command_List == NULL || command == NULL ) return -1;
	LOF_TOOL_FxListType* commandlistptr;
	LOF_TOOL_CommandType* commandptr;
	LOF_DATA_BuddyContactType* contactptr;
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
		commandptr = LOF_TOOL_Command_new(LOF_COMMAND_SEND_MSG,contactptr->sipuri,pram2);
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
						commandptr = LOF_TOOL_Command_new(LOF_COMMAND_SEND_SMS,user->sipuri,pram2);
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
				commandptr = LOF_TOOL_Command_new(LOF_COMMAND_SEND_SMS,contactptr->sipuri,pram2);
				if (*Command_List == NULL){
					*Command_List = LOF_TOOL_FxList_new(commandptr);
				}else{
					LOF_TOOL_FxList_append(*Command_List,LOF_TOOL_FxList_new(commandptr));
				}
				LOF_debug_info("Arranged A New SMS Command, Which is NO. [%d]",commandptr->CommandId);
				return commandptr->CommandId;
			}




	return 0;
}

int LOF_TOOL_Command_main(LOF_TOOL_FxListType** Command_List,LOF_TOOL_FxListType* ConversationList){
	if (Command_List == NULL || ConversationList == NULL ) return -1;
	if (*Command_List == NULL) return 1;
	LOF_TOOL_FxListType* listptr=*Command_List;
	LOF_TOOL_FxListType* tempptr = NULL;
	int execcount=0;
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
				if (((LOF_TOOL_CommandType*)(listptr->data))->Status == LOF_COMMAND_STATUS_FAIL){
					LOF_debug_error("Command [%d] Failed Twice , Abandoning.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
				}else{
					LOF_debug_info("Command [%d] Successfully Done , Removing From List.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
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
					return 1;
				}
				if (((listptr)->next == (listptr)) && ((listptr)->pre != (listptr))){
					LOF_TOOL_Command_destroy(((LOF_TOOL_CommandType*)(listptr->data)));
					LOF_TOOL_FxList_del(&listptr);
//					LOF_debug_info("Destroyed Bottom Of List, Command Execute Loop Over.");
					return 0;

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



			execcount++;
		}
		if (listptr->next != listptr){
			listptr = listptr->next;
		}else break;

	}
//	LOF_debug_info("Executed Bottom Of List, Which Is NO. [%d] , Command Execute Loop Over.",((LOF_TOOL_CommandType*)(listptr->data))->CommandId);
	return 0;
}

int LOF_CallBack_Message (LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List ,char* sipmsg){

	char from[64];
	memset(from , 0 , sizeof(from));
	LOF_SIP_get_attr(sipmsg , "F" , from);
	LOF_TOOL_Command_arrange(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentUser,Command_List,"MSG",from,sipmsg);
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
int LOF_FIFO_open(LOF_DATA_LocalUserType* user,const char* fname){
	char fullname[256];
	memset (fullname,0,sizeof(fullname));
	snprintf((char*)fullname, sizeof(fullname), "%s/%s",user->config->globalPath,fname);
	return open(fullname,O_RDONLY|O_NONBLOCK);
}
int LOF_TOOL_Command_ParseStr(LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** cmdlist,char* str){
	char Instruction[6];
	char Pram1[1024];
	char Pram2[2048];
	char* strptr1=str;
	char* strptr2=NULL;
	char* strptr3=NULL;
//	printf("!!!\n%s",str);
	strptr2 = strstr(str,";;");
	for(;;){
		if (strptr2 == NULL) break;
		memset (Instruction,0,sizeof(Instruction));
		memset (Pram1,0,sizeof(Pram1));
		memset (Pram2,0,sizeof(Pram2));
		for (;;){
		if ((char)(*strptr1) == '\n' || ((char)(*strptr1) == '\r') || ((char)(*strptr1) == ' ') || ((char)(*strptr1) == '\t')) strptr1++;
		else break;
		}
		strptr3=strstr(strptr1," ");
		memcpy(Instruction,strptr1,(size_t)(strptr3-strptr1));
		strptr1=strptr3+1;
		strptr3=strstr(strptr1," ");
		memcpy(Pram1,strptr1,(size_t)(strptr3-strptr1));
		strptr1=strptr3+1;
		memcpy(Pram2,strptr1,(size_t)(strptr2-strptr1));
//		LOF_debug_info("Result : %s : %s : %s ;\n\n",Instruction,Pram1,Pram2);
		LOF_TOOL_Command_arrange(user,cmdlist,Instruction,Pram1,Pram2);
		strptr1=strptr2+2;
		strptr2 = strstr(strptr2+2,";;");
	}
	return 0;
}
