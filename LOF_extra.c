#include "LOF_openfetion.h"
#include "LOF_extra.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


LOF_TOOL_StopWatchType* LOF_TOOL_StopWatch_new(){
	LOF_TOOL_StopWatchType* the_watch = (LOF_TOOL_StopWatchType*)malloc(sizeof(LOF_TOOL_StopWatchType));
	the_watch->begin = (time_t)0;
	the_watch->end = (time_t)0;
	return the_watch;
}
void LOF_TOOL_StopWatch_start(LOF_TOOL_StopWatchType* the_watch){
	the_watch->begin = clock();
	the_watch->end = (time_t)0;
	return;
}
void LOF_TOOL_StopWatch_stop(LOF_TOOL_StopWatchType* the_watch){
	the_watch->end = clock();
	return;
}
void LOF_TOOL_AutoSleep(LOF_TOOL_StopWatchType* the_watch,int usleeptime){
	double  sleep;
	if (the_watch == NULL || the_watch->begin == (time_t)0) {usleep(usleeptime);return;}
	if (the_watch->end == (time_t)0 && the_watch->begin != (time_t)0) the_watch->end = clock();
	sleep = usleeptime - (int)(((double)(the_watch->end - the_watch->begin) / CLOCKS_PER_SEC)*1000000);
	if (sleep > 10000 ) {usleep(sleep);printf("slept %f sec.\n",sleep/1000000);}
	return;
}

LOF_TOOL_CommandType* LOF_TOOL_Command_new(LOF_TOOL_Command_InstructionType command,const char* pram1,const char* pram2){
	if (pram1==NULL) return NULL;
	LOF_TOOL_CommandType* The_Command = (LOF_TOOL_CommandType*)malloc(sizeof(LOF_TOOL_CommandType));
	memset(The_Command,0,sizeof(LOF_TOOL_CommandType));
	The_Command->Status = LOF_COMMAND_STATUS_NOT_HANDALED;
	The_Command->Progress = 0;
	The_Command->Callid = 0;
	The_Command->Instruction = command;
	The_Command->Pram1 = (char*)malloc(sizeof(The_Command->Pram1));
	memcpy(The_Command->Pram1,pram1,sizeof(pram1));
	if (pram2 != NULL){
	The_Command->Pram2 = (char*)malloc(sizeof(The_Command->Pram2));
	memcpy(The_Command->Pram2,pram2,sizeof(pram2));
	}
	The_Command->BackSipMsg = NULL;
	LOF_debug_info("Built A New Command.");
	return The_Command;
}
int LOF_TOOL_Command_exec_send_msg(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* ConversationList){
		if(The_Command->Pram2 ==NULL||ConversationList == NULL) {
			The_Command->Status = LOF_COMMAND_STATUS_FAIL;
			return -1;
		}

		LOF_DATA_LocalUserType* CurUser = ((LOF_DATA_LocalUserType*)(((LOF_USER_ConversationType*)(ConversationList->data))->currentUser));
		LOF_SIP_FetionSipType* CurSip = ((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationList->data))->currentSip));

		if (The_Command->Progress == 0 && The_Command->BackSipMsg == NULL){
			LOF_SIP_SipHeaderType* eheader;
			char* res;
			LOF_SIP_FetionSip_set_type(CurUser->sip , LOF_SIP_SERVICE);
			eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_STARTCHAT);
			LOF_SIP_FetionSip_add_header(CurUser->sip , eheader);
			res = LOF_SIP_to_string(CurUser->sip , NULL);
			LOF_CONNECTION_FetionConnection_send(CurUser->sip->tcp , res , strlen(res));
			free(res);
			The_Command->Progress = 1;
			The_Command->Callid = ((CurUser->sip->callid) - 1);
			The_Command->Status = LOF_COMMAND_STATUS_ON_HOLD;
			return 2;
		}



		if (The_Command->Progress == 1 && The_Command->BackSipMsg != NULL){
				char *res , *ip , *credential , auth[256] ;
				int port , ret;
				LOF_CONNECTION_FetionConnectionType* conn=LOF_CONNECTION_FetionConnection_new();
				LOF_USER_ConversationType* theconversation = LOF_USER_Conversation_new(CurUser,
						  The_Command->Pram1 , NULL);
				LOF_SIP_FetionSipType* sip;
				LOF_SIP_SipHeaderType *eheader , *theader , *mheader , *nheader , *aheader;

				memset(auth , 0 , sizeof(auth));
				LOF_SIP_get_attr(The_Command->BackSipMsg , "A" , auth);
				if(auth==NULL)
					return -1;

				LOF_SIP_get_auth_attr(auth , &ip , &port , &credential);
				free(The_Command->BackSipMsg); The_Command->BackSipMsg = NULL;
				ret = LOF_CONNECTION_FetionConnection_connect(conn, ip, port);
				if(ret == -1)
				ret = LOF_CONNECTION_FetionConnection_connect(conn, ip, 443);
				if(ret == -1){
					The_Command->Status = LOF_COMMAND_STATUS_FAIL;
					return -1;
				}
				/*clone sip*/
				theconversation->currentSip = LOF_SIP_FetionSip_clone(CurUser->sip);
				memset(theconversation->currentSip->sipuri, 0 , sizeof(theconversation->currentSip->sipuri));
				strcpy(theconversation->currentSip->sipuri , theconversation->currentContact->sipuri);
				LOF_SIP_FetionSip_set_connection(theconversation->currentSip , conn);
				free(ip); ip = NULL;

				LOF_TOOL_FxList_append(ConversationList,LOF_TOOL_FxList_new((LOF_USER_ConversationType*)theconversation));

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
				The_Command->Callid = ((sip->callid) - 1);
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
				LOF_TOOL_FxListType* ConversationListPtr = ConversationList;
				for(;;){
					if (strcmp(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip->sipuri,The_Command->Pram1) == 0 ) break;
					if (ConversationListPtr->next != ConversationListPtr)ConversationListPtr=ConversationListPtr->next;else {
						The_Command->Status = LOF_COMMAND_STATUS_FAIL;
						return -1;
					}
				}

				LOF_SIP_FetionSip_set_type(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip , LOF_SIP_SERVICE);
				eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_INVITEBUDDY);
				LOF_SIP_FetionSip_add_header(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip , eheader);
				body = LOF_SIP_generate_invite_friend_body(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip->sipuri);
				res = LOF_SIP_to_string(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip , body);
				free(body); body = NULL;
				LOF_CONNECTION_FetionConnection_send(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip->tcp , res , strlen(res));
				free(res); res = NULL;
				The_Command->Callid = ((((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip->callid) - 1);
				The_Command->Progress = 3;
		}





		return 0;
}
