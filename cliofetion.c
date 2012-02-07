/***************************************************************************
 *   Copyright (C) 2010 by lwp                                             *
 *   levin108@gmail.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include "LOF_openfetion.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <time.h>
#include "LOF_extra.h"
#define BUFLEN 1024
#define BUFFER_SIZE 1024*100
int   password_inputed = 0;
int   mobileno_inputed = 0;
int   tono_inputed = 0;
int   message_inputed = 0;
LOF_DATA_LocalUserType *user;
pthread_t th;

static void usage(char *argv[]);

int fx_login(const char *mobileno, const char *password)
{
	LOF_TOOL_ConfigType           *config;
	LOF_CONNECTION_FetionConnectionType *tcp;
	LOF_SIP_FetionSipType        *sip;

	char             *res;
	char             *nonce;
	char             *key;
	char             *aeskey;
	char             *response;
	int               local_group_count;
	int               local_buddy_count;
	int               group_count;
	int               buddy_count;
	int               ret;

	/* construct a user object */
 	user = LOF_DATA_LocalUser_new(mobileno, password);
	/* construct a config object */
	config = LOF_TOOL_Config_new();
	/* attach config to user */
	LOF_DATA_LocalUser_set_config(user, config);
 
	/* start ssi authencation,result string needs to be freed after use */
	res = LOF_LOGIN_ssi_auth_action(user);
	/* parse the ssi authencation result,if success,user's sipuri and userid
	 * are stored in user object,orelse user->loginStatus was marked failed */
	LOF_LOGIN_parse_ssi_auth_response(res, user);
	free(res);
 
	/* whether needs to input a confirm code,or login failed
	 * for other reason like password error */
	if(LOF_USER_AUTH_NEED_CONFIRM(user) || LOF_USER_AUTH_ERROR(user)) {
		LOF_debug_error("authencation failed");
		return 1;
	}
 
	/* initialize configuration for current user */
	if(LOF_DATA_LocalUser_init_config(user) == -1) {
		LOF_debug_error("initialize configuration");
		return 1;
	}
 
	if(LOF_DATA_LocalUser_download_configuration(user) == -1) {
		LOF_debug_error("download configuration");
		return 1;
	}
 
	/* set user's login state to be hidden */
	user -> status =  LOF_STATUS_ONLINE;
 
	/* load user information and contact list information from local host */
	LOF_DATA_LocalUser_load(user);
	LOF_DATA_BuddyContact_load(user, &local_group_count, &local_buddy_count);
 
	/* construct a tcp object and connect to the sipc proxy server */
	tcp = LOF_CONNECTION_FetionConnection_new();
	if((ret = LOF_CONNECTION_FetionConnection_connect(tcp, config->sipcProxyIP, config->sipcProxyPort)) == -1) {
		LOF_debug_error("connect sipc server %s:%d\n", config->sipcProxyIP, config->sipcProxyPort);
		return 1;
	}
 
	/* construct a sip object with the tcp object and attach it to user object */
	sip = LOF_SIP_FetionSip_new(tcp, user->sId);
	LOF_DATA_LocalUser_set_sip(user, sip);

	/* register to sipc server */
	if(!(res = LOF_LOGIN_sipc_reg_action(user))) {
		LOF_debug_error("register to sipc server");
		return 1;
	}
 
	LOF_LOGIN_parse_sipc_reg_response(res, &nonce, &key);
	free(res);
	aeskey = LOF_LOGIN_generate_aes_key();
 
	response = LOF_LOGIN_generate_response(nonce, user->userId, user->password, key, aeskey);
	free(nonce);
	free(key);
	free(aeskey);
 
	/* sipc authencation,you can printf res to see what you received */
	if(!(res = LOF_LOGIN_sipc_aut_action(user, response))) {
		LOF_debug_error("sipc authencation");
		return 1;
	}
	if(LOF_LOGIN_parse_sipc_auth_response(res, user, &group_count, &buddy_count) == -1) {
		LOF_debug_error("authencation failed");
		return 1;
	}
 
	free(res);
	free(response);

	if(LOF_USER_AUTH_ERROR(user) || LOF_USER_AUTH_NEED_CONFIRM(user)) {
		LOF_debug_error("login failed");
		return 1;
	}
 
	/* save the user information and contact list information back to the local database */
	LOF_DATA_LocalUser_save(user);
	LOF_DATA_BuddyContact_save(user);
	LOF_DATA_BuddyContact_subscribe_only(user);
	/* these... fuck the fetion protocol */

	LOF_debug_info("Login is done. start" );


/*
 * ======================================================================================================
 * 								HERE STARTS THE MAIN LOOP
 *
 * do{

				if (curmsg == NULL) break;
				printf ("\n\n%s\n\n",curmsg->message);
				curmsg = (curmsg->next);
				} while (curmsg != NULL);

	//LOF_DATA_LocalUser_set_moodphrase(user , "Testing");*/
		int ttemp;
		int temp;
		LOF_DATA_SipMsgType *curmsg,*tempmsg,*totalmsg;
		LOF_DATA_FetionMessageType *msg;
		LOF_TOOL_StopWatchType* the_watch = LOF_TOOL_StopWatch_new();
		LOF_TOOL_FxListType* ConversationList;
		LOF_TOOL_FxListType* ConversationListPtr;
		LOF_TOOL_FxListType* ConversationListTempPtr;
		LOF_SIP_FetionSipType *conversationSip;
		LOF_TOOL_FxListType* Command_List=NULL;
		LOF_USER_ConversationType *ConversationPtr;
		LOF_EVENT_IncomingType Incoming;
		LOF_DATA_BuddyContactType *ContactPtr;
		char*	sipuri;
		int backcallid;
		int type;
		int event;
		char* xml;
		ConversationList= LOF_TOOL_FxList_new(LOF_USER_Conversation_new(user,NULL, user->sip));


	curmsg=NULL;
	tempmsg=NULL;
	totalmsg=NULL;
	for (temp=0;temp<=3000;temp++){
		ConversationListPtr = ConversationList;
		for(;;)
		{

			if (((LOF_USER_ConversationType*)(ConversationListPtr->data))->ready >= 0){
			curmsg=LOF_SIP_FetionSip_listen((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) , &ttemp);}
			else {
				curmsg=NULL;
			}
			if(ttemp==2){
				ttemp=0;
				LOF_SIP_FetionSip_free((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) );
				if(ConversationListPtr->next==ConversationListPtr) {
					ConversationListTempPtr=ConversationListPtr->pre;
					LOF_TOOL_FxList_del(&ConversationListPtr);
					ConversationListPtr=ConversationListTempPtr;
					break;
				}else{
					ConversationListTempPtr=ConversationListPtr->next;
					LOF_TOOL_FxList_del(&ConversationListPtr);
					ConversationListPtr=ConversationListTempPtr;
					continue;
				}
			}
			if (totalmsg == NULL && curmsg!=NULL) totalmsg=curmsg;
			else if(curmsg!=NULL)LOF_DATA_SipMsg_append(totalmsg,curmsg);
			else printf("NO MSG\n");

		for(;;){
			if (totalmsg == NULL) {break;}
			tempmsg=totalmsg;
			//printf("\n\n%s\n\n",tempmsg->message);
			switch (LOF_SIP_get_type(tempmsg->message)){
			case LOF_SIP_MESSAGE:
				printf ("\nGOT A MESSAGE !!\n%s\n\n",tempmsg->message);
				LOF_SIP_parse_message((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) , tempmsg->message ,&msg);
				if ((LOF_DATA_BuddyContactType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentContact) == NULL) break;else{
					LOF_TOOL_Command_arrange(user,&Command_List,"MSG",(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentContact)->sId,tempmsg->message);

				}
			break;
			case LOF_SIP_INVITATION:
				printf ("\nGOT A INVITATION !!\n%s\n\n",tempmsg->message);
				{
					if ((LOF_DATA_BuddyContactType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentContact) != NULL) break;
					LOF_SIP_parse_invitation((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) , NULL , tempmsg->message
							, &conversationSip , &sipuri);
					LOF_TOOL_FxList_append(ConversationList,LOF_TOOL_FxList_new( LOF_USER_Conversation_new(user,
							  sipuri , conversationSip)));
					ConversationListPtr = ConversationList;
				}
				break;
			case LOF_SIP_NOTIFICATION:
				printf ("\nGOT A NOLIFICATION !!\n%s\n\n",tempmsg->message);
				LOF_SIP_parse_notification(tempmsg->message , &type , &event , &xml);
				switch(type){
				case LOF_NOTIFICATION_TYPE_PRESENCE:
					ContactPtr=LOF_SIP_parse_presence_body((strstr(tempmsg->message,"\r\n\r\n")) , user);
					//Here should do further process,changed contact list is not freed
					break;
				case LOF_NOTIFICATION_TYPE_CONVERSATION:
					if(event == LOF_NOTIFICATION_EVENT_USERLEFT) {
						LOF_SIP_FetionSip_free((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) );
										if(ConversationListPtr->next==ConversationListPtr) {
											ConversationListTempPtr=ConversationListPtr->pre;
											LOF_TOOL_FxList_del(&ConversationListPtr);
											ConversationListPtr=ConversationListTempPtr;
											continue;
										}else{
											ConversationListTempPtr=ConversationListPtr->next;
											LOF_TOOL_FxList_del(&ConversationListPtr);
											ConversationListPtr=ConversationListTempPtr;
											continue;
										}
									break;
					}else 	if(event == LOF_NOTIFICATION_EVENT_USERENTER) {
						//Here Should Enable Conversation Sending
						LOF_debug_info("Detected User Enter");
						((LOF_USER_ConversationType*)(ConversationListPtr->data))->ready = 1;
						break;
					}else if (event == LOF_NOTIFICATION_EVENT_USERFAILED){
						LOF_debug_info("Detected User Failure");
						((LOF_USER_ConversationType*)(ConversationListPtr->data))->ready = -1;
					}
					break;
				case LOF_NOTIFICATION_TYPE_CONTACT:
					if(event == LOF_NOTIFICATION_EVENT_ADDBUDDYAPPLICATION){
						//Here Should Process Add Budy
					}

					break;
				case LOF_NOTIFICATION_TYPE_REGISTRATION:
					if(event == LOF_NOTIFICATION_EVENT_DEREGISTRATION)	{
						LOF_debug_info("Received DEREGISTRATION!");
						LOF_debug_error("You Are KICKED OFFLINE");
						return -1;
						//Fatal Error, Should Exit
					}
					break;
				case LOF_NOTIFICATION_TYPE_SYNCUSERINFO:
					if(event == LOF_NOTIFICATION_EVENT_SYNCUSERINFO){
						//Here should process sync user info.
					}
					break;
				case LOF_NOTIFICATION_TYPE_PGGROUP:
					LOF_debug_info("WTF!! NO PGGROUP Support Now!");
					break;
				case LOF_NOTIFICATION_TYPE_UNKNOWN:
					LOF_debug_info("UNKNOWN Type Of Notification, Dropped.");
					break;
				default:
					LOF_debug_info("WTF?!");
				break;
				}

				break;
			case LOF_SIP_SIPC_4_0:
				printf ("\nGOT A SIPC40 !!\n%s\n\n",tempmsg->message);
				LOF_TOOL_Command_ack_sipc40 (Command_List, tempmsg->message);

				break;
			case LOF_SIP_INCOMING:
				printf ("\nGOT A INCOMING !!\n%s\n\n",tempmsg->message);
				LOF_SIP_parse_incoming((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip)
						, tempmsg->message , &sipuri
						, &Incoming , NULL);
				if (Incoming == LOF_INCOMING_NUDGE) LOF_USER_Conversation_send_nudge((LOF_USER_ConversationType*)(ConversationListPtr->data));
				break;
			case LOF_SIP_OPTION:
				printf ("\nGOT A OPTION !!\n%s\n\n",tempmsg->message);
				break;
			default:printf ("\nGOT A UNKNOWN !!\n%s\n\n",tempmsg->message);
			break;
			}
			if(tempmsg->next==NULL) {
				//free(tempmsg->message);
				free(tempmsg);
				totalmsg=NULL;
				break;
			}
			totalmsg = tempmsg->next;

		//	free(tempmsg->message);
			free(tempmsg);


			}
	//	printf("Process OVER,this is %d . \n\n",temp);

		if (ConversationListPtr->next == ConversationListPtr) break;
		ConversationListPtr = ConversationListPtr->next;
		}


	//	printf("Entering\n");
		if (LOF_TOOL_Command_main(&Command_List,ConversationList)== 1) sleep(3);else usleep(300000);

		if(temp == 2) {
			//LOF_TOOL_Command_arrange(user,&Command_List,"MSG","855272648","开始吧~");

		//	LOF_TOOL_Command_arrange(user,&Command_List,"MSG","932446575","开始吧~");
		}

		LOF_TOOL_AutoKeepAlive(user,the_watch,25);

	}


	return 0;
}
 
int send_message(const char *mobileno, const char *receiveno, const char *message)
{
	LOF_USER_ConversationType *conv;
	LOF_DATA_BuddyContactType      *contact;
	LOF_DATA_BuddyContactType      *contact_cur;
	LOF_DATA_BuddyContactType      *target_contact = NULL;
	int           daycount;
	int           monthcount;
 
	/* send this message to yourself */
	if(*receiveno == '\0' || strcmp(receiveno, mobileno) == 0) {
		/* construct a conversation object with the sipuri to set NULL
		 * to send a message to yourself  */
		conv = LOF_USER_Conversation_new(user, NULL, NULL);
		if(LOF_USER_Conversation_send_sms_to_myself_with_reply(conv, message) == -1) {
			LOF_debug_error("send message \"%s\" to %s", message, user->mobileno);
			return 1;
		}
	}else{
		/* get the contact detail information by mobile number,
		 * note that the result doesn't contain sipuri */
		contact = LOF_DATA_BuddyContact_get_contact_info_by_no(user, receiveno, LOF_MOBILE_NO);
		if(!contact) {
			LOF_debug_error("get contact information of %s", receiveno);
			return 1;
		}
 
		/* find the sipuri of the target user */
		foreach_contactlist(user->contactList, contact_cur) {
			if(strcmp(contact_cur->userId, contact->userId) == 0) {
				target_contact = contact_cur;
				break;
			}
		}
 
		if(!target_contact) {
			LOF_debug_error("sorry,maybe %s isn't in your contact list");
			return 1;
		}
 
		/* do what the function name says */
		conv = LOF_USER_Conversation_new(user, target_contact->sipuri, NULL);
		if(LOF_USER_Conversation_send_sms_to_phone_with_reply(conv, message, &daycount, &monthcount) == -1) {
			LOF_debug_error("send sms to %s", receiveno);
			return 1;
		}else{
			LOF_debug_info("successfully send sms to %s\nyou have sent %d messages today, %d messages this monthcount",
					receiveno, daycount, monthcount);
			return 0;
		}
	}
	return 0;
}
 
int main(int argc, char *argv[])
{
	int ch;
	char mobileno[BUFLEN];
	char password[BUFLEN];
	char receiveno[BUFLEN];
	char message[BUFLEN];
 
	memset(mobileno, 0, sizeof(mobileno));
	memset(password, 0, sizeof(password));
	memset(receiveno, 0, sizeof(receiveno));
	memset(message, 0, sizeof(message));
 
	while((ch = getopt(argc, argv, "f:p:t:d:")) != -1) {
		switch(ch) {
			case 'f':
				mobileno_inputed = 1;
				strncpy(mobileno, optarg, sizeof(mobileno) - 1);	
				break;
			case 'p':
				password_inputed = 1;
				strncpy(password, optarg, sizeof(password) - 1);
				break;
			case 't':
				tono_inputed = 1;
				strncpy(receiveno, optarg, sizeof(receiveno) - 1);
				break;
			case 'd':
				message_inputed = 1;
				strncpy(message, optarg, sizeof(message) - 1);
				break;
			default:
				break;
		}
	}
 
	if(!mobileno_inputed || !password_inputed || !message_inputed) {
		usage(argv);
		return 1;
	}
 
	if(fx_login(mobileno, password))
		return 1;
 
	if(send_message(mobileno, receiveno, message))
		return 1;
 
	LOF_DATA_LocalUser_free(user);
	return 0;
 
}
 
static void usage(char *argv[])
{
	fprintf(stderr, "Usage:%s -f mobileno -p password -t receive_mobileno -d message\n", argv[0]);
}
