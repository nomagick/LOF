#include "LOF_openfetion.h"
#include "LOF_extra.h"







#define BUFLEN 1024
	LOF_DATA_LocalUserType *LOF_GLOBAL_User;
	char mobileno[BUFLEN];
	char password[BUFLEN];
	char receiveno[BUFLEN];
	char message[BUFLEN];


	LOF_TOOL_FxListType* LOF_GLOBAL_ConversationList = NULL;
	LOF_TOOL_FxListType* LOF_GLOBAL_Command_List=NULL;
int LOF_TAKEOVER(){


		/* pointers for incoming messages */
		LOF_DATA_SipMsgType *curmsg=NULL,*tempmsg=NULL,*totalmsg=NULL;
	//	LOF_DATA_FetionMessageType *msg;
		/* Stop watch used to decide whether a KeepAlive request should be sent via the root channel. */
		LOF_TOOL_StopWatchType* the_watch = LOF_TOOL_StopWatch_new();
		/* Root conversation list , keeping every socket channel .the first item is the root channel, which should never be destroyed.*/
		 LOF_GLOBAL_ConversationList = LOF_TOOL_FxList_new(LOF_USER_Conversation_new(LOF_GLOBAL_User,NULL, LOF_GLOBAL_User->sip));
		LOF_TOOL_FxListType* ConversationListPtr;
		LOF_TOOL_FxListType* ConversationListTempPtr;
		//LOF_USER_ConversationType *ConversationPtr;

		/* The Command List used to arrange sending operations.*/

		/* Die flag */
		int WANADIE = 0;
		/* All Kinds Of temporary vars.*/
		LOF_EVENT_IncomingType Incoming;
		LOF_DATA_BuddyContactType *ContactPtr;
		char*	sipuri;
		int backcallid;
		int type;
		int event;
		char* xml;
		int loop_counter=0;
		int listener_error;
		int LongSleepFlag = 1;


	/* The Main Loop*/
	for (loop_counter=0;loop_counter<=3000 && WANADIE == 0;loop_counter++){
		ConversationListPtr = LOF_GLOBAL_ConversationList;

		for(;;)
		{

			if (((LOF_USER_ConversationType*)(ConversationListPtr->data))->ready >= 0){
			curmsg=LOF_SIP_FetionSip_listen((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) , &listener_error);

			}
			else {
				curmsg=NULL;
			}
			if((listener_error==2 || LOF_TOOL_StopWatch_read(((LOF_USER_ConversationType*)(ConversationListPtr->data))->timer) >= (LOF_COMMAND_MAX_TIME + 15)) && (((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentContact != NULL)){
				listener_error=0;
				LOF_debug_info("Closing channel for buddy %s",((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentContact->sipuri);
				free(((LOF_USER_ConversationType*)(ConversationListPtr->data))->timer);
				LOF_SIP_FetionSip_free((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) );
				if(ConversationListPtr->next==ConversationListPtr) {
					ConversationListTempPtr=ConversationListPtr->pre;
					LOF_TOOL_FxList_del(&ConversationListPtr);
					ConversationListPtr=ConversationListTempPtr;
					curmsg = NULL;
				}else{
					ConversationListTempPtr=ConversationListPtr->pre;
					LOF_TOOL_FxList_del(&ConversationListPtr);
					ConversationListPtr=ConversationListTempPtr;
					curmsg = NULL;
				}
			}
			if (curmsg != NULL) LOF_TOOL_StopWatch_start(((LOF_USER_ConversationType*)(ConversationListPtr->data))->timer);
			if (((LOF_USER_ConversationType*)(ConversationListPtr->data))->ready == 1) LongSleepFlag = 0;
			if (totalmsg == NULL && curmsg!=NULL) totalmsg=curmsg;
			else if(curmsg!=NULL)LOF_DATA_SipMsg_append(totalmsg,curmsg);
	//	else printf("NO MSG\n");

		for(;;){
			if (totalmsg == NULL) {break;}
			tempmsg=totalmsg;
			//printf("\n\n%s\n\n",tempmsg->message);
			switch (LOF_SIP_get_type(tempmsg->message)){
			case LOF_SIP_MESSAGE:
				printf ("\nGOT A MESSAGE !!\n%s\n\n",tempmsg->message);
				/* say ack to server */
				LOF_SIP_parse_message((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) , tempmsg->message ,NULL);
				/* do whatever you want */
				LOF_CallBack_Message(ConversationListPtr,&LOF_GLOBAL_Command_List,tempmsg->message);
				break;
			case LOF_SIP_INVITATION:
//			printf ("\nGOT A INVITATION !!\n%s\n\n",tempmsg->message);
				if (((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentContact != NULL)
				{
					LOF_debug_error("Received Invitation from non root channel, which should not happen.");
					break;
				}
				LOF_CallBack_Invitation(ConversationListPtr,&LOF_GLOBAL_Command_List,tempmsg->message);
				LongSleepFlag = 0;
				break;
			case LOF_SIP_NOTIFICATION:
//			printf ("\nGOT A NOLIFICATION !!\n%s\n\n",tempmsg->message);
				LOF_SIP_parse_notification(tempmsg->message , &type , &event , &xml);
				free (xml);
				switch(type){
				case LOF_NOTIFICATION_TYPE_PRESENCE:
					LOF_CallBack_Presence(ConversationListPtr,&LOF_GLOBAL_Command_List,tempmsg->message);

					//Here should do further process,changed contact list is not freed
					break;
				case LOF_NOTIFICATION_TYPE_CONVERSATION:
					if(event == LOF_NOTIFICATION_EVENT_USERLEFT) {
						free(((LOF_USER_ConversationType*)(ConversationListPtr->data))->timer);
						LOF_SIP_FetionSip_free((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip) );
										if(ConversationListPtr->next==ConversationListPtr) {
											ConversationListTempPtr=ConversationListPtr->pre;
											LOF_TOOL_FxList_del(&ConversationListPtr);
											ConversationListPtr=ConversationListTempPtr;
										//	break;
										}else{
											ConversationListTempPtr=ConversationListPtr->pre;
											LOF_TOOL_FxList_del(&ConversationListPtr);
											ConversationListPtr=ConversationListTempPtr;
									//		break;
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
						WANADIE=1;
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
//				printf ("\nGOT A SIPC40 !!\n%s\n\n",tempmsg->message);
				LOF_TOOL_Command_ack_sipc40 (LOF_GLOBAL_Command_List, tempmsg->message);

				break;
			case LOF_SIP_INCOMING:
//				printf ("\nGOT A INCOMING !!\n%s\n\n",tempmsg->message);
				LOF_SIP_parse_incoming((LOF_SIP_FetionSipType*)(((LOF_USER_ConversationType*)(ConversationListPtr->data))->currentSip)
						, tempmsg->message , &sipuri
						, &Incoming , NULL);
				if (Incoming == LOF_INCOMING_NUDGE) LOF_USER_Conversation_send_nudge((LOF_USER_ConversationType*)(ConversationListPtr->data));
				break;
			case LOF_SIP_OPTION:
//				printf ("\nGOT A OPTION !!\n%s\n\n",tempmsg->message);
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

		LOF_TOOL_AutoKeepAlive(LOF_GLOBAL_User,the_watch,25);

		//	printf("Entering\n");
		if (LOF_TOOL_Command_main(&LOF_GLOBAL_Command_List, LOF_GLOBAL_ConversationList) != 1) LongSleepFlag = 0;

		if (LongSleepFlag == 1) sleep(3);else {
			LongSleepFlag = 1 ;
			usleep(300000);
		}

		if(loop_counter == 5) {
			LOF_DATA_BuddyContactType *cl_cur;
				foreach_contactlist(LOF_GLOBAL_User->contactList , cl_cur)
			{
					if (cl_cur->status == LOF_STATUS_HIDDEN)
						LOF_TOOL_Command_arrange(LOF_GLOBAL_User,&LOF_GLOBAL_Command_List,"MSG",cl_cur->sipuri,message);

				}/*
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);
			LOF_TOOL_Command_arrange(user,&Command_List,"MSG","470167670",message);

*/
		}


	//	LOF_debug_info("Loop Finish. All Good.");

	}


	return 0;
	}


	int LOF_INIT(const char *mobileno, const char *password)
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
		LOF_GLOBAL_User = LOF_DATA_LocalUser_new(mobileno, password);
		/* construct a config object */
		config = LOF_TOOL_Config_new();
		/* attach config to user */
		LOF_DATA_LocalUser_set_config(LOF_GLOBAL_User, config);

		/* start ssi authencation,result string needs to be freed after use */
		res = LOF_LOGIN_ssi_auth_action(LOF_GLOBAL_User);
		/* parse the ssi authencation result,if success,user's sipuri and userid
		 * are stored in user object,orelse user->loginStatus was marked failed */
		LOF_LOGIN_parse_ssi_auth_response(res, LOF_GLOBAL_User);
		free(res);

		/* whether needs to input a confirm code,or login failed
		 * for other reason like password error */
		if(LOF_USER_AUTH_NEED_CONFIRM(LOF_GLOBAL_User) || LOF_USER_AUTH_ERROR(LOF_GLOBAL_User)) {
			LOF_debug_error("authencation failed");
			return 1;
		}

		/* initialize configuration for current user */
		if(LOF_DATA_LocalUser_init_config(LOF_GLOBAL_User) == -1) {
			LOF_debug_error("initialize configuration");
			return 1;
		}

		if(LOF_DATA_LocalUser_download_configuration(LOF_GLOBAL_User) == -1) {
			LOF_debug_error("download configuration");
			return 1;
		}

		/* set user's login state to be hidden */
		LOF_GLOBAL_User -> status =  LOF_STATUS_ONLINE;

		/* load user information and contact list information from local host */
		LOF_DATA_LocalUser_load(LOF_GLOBAL_User);
		LOF_DATA_BuddyContact_load(LOF_GLOBAL_User, &local_group_count, &local_buddy_count);

		/* construct a tcp object and connect to the sipc proxy server */
		tcp = LOF_CONNECTION_FetionConnection_new();
		if((ret = LOF_CONNECTION_FetionConnection_connect(tcp, config->sipcProxyIP, config->sipcProxyPort)) == -1) {
			LOF_debug_error("connect sipc server %s:%d\n", config->sipcProxyIP, config->sipcProxyPort);
			return 1;
		}

		/* construct a sip object with the tcp object and attach it to user object */
		sip = LOF_SIP_FetionSip_new(tcp, LOF_GLOBAL_User->sId);
		LOF_DATA_LocalUser_set_sip(LOF_GLOBAL_User, sip);

		/* register to sipc server */
		if(!(res = LOF_LOGIN_sipc_reg_action(LOF_GLOBAL_User))) {
			LOF_debug_error("register to sipc server");
			return 1;
		}

		LOF_LOGIN_parse_sipc_reg_response(res, &nonce, &key);
		free(res);
		aeskey = LOF_LOGIN_generate_aes_key();

		response = LOF_LOGIN_generate_response(nonce, LOF_GLOBAL_User->userId, LOF_GLOBAL_User->password, key, aeskey);
		free(nonce);
		free(key);
		free(aeskey);

		/* sipc authencation,you can printf res to see what you received */
		if(!(res = LOF_LOGIN_sipc_aut_action(LOF_GLOBAL_User, response))) {
			LOF_debug_error("sipc authencation");
			return 1;
		}
		if(LOF_LOGIN_parse_sipc_auth_response(res, LOF_GLOBAL_User, &group_count, &buddy_count) == -1) {
			LOF_debug_error("authencation failed");
			return 1;
		}

		free(res);
		free(response);

		if(LOF_USER_AUTH_ERROR(LOF_GLOBAL_User) || LOF_USER_AUTH_NEED_CONFIRM(LOF_GLOBAL_User)) {
			LOF_debug_error("login failed");
			return 1;
		}

		/* save the user information and contact list information back to the local database */
		LOF_DATA_LocalUser_save(LOF_GLOBAL_User);
		LOF_DATA_BuddyContact_save(LOF_GLOBAL_User);
		LOF_DATA_BuddyContact_subscribe_only(LOF_GLOBAL_User);

		LOF_debug_info("Login is done. start" );
		return 0;
	}


	int main(int argc, char *argv[])
{

	int ch;

	int   password_inputed = 0;
	int   mobileno_inputed = 0;
	int   tono_inputed = 0;
	int   message_inputed = 0;


	memset(mobileno, 0, sizeof(mobileno));
	memset(password, 0, sizeof(password));
	memset(receiveno, 0, sizeof(receiveno));
	memset(message, 0, sizeof(message));

	while((ch = getopt(argc, argv, "f:p:t:m:")) != -1) {
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
			case 'm':
				message_inputed = 1;
				strncpy(message, optarg, sizeof(message) - 1);
				break;
			default:
				break;
		}
	}

	if(!mobileno_inputed || !password_inputed || !message_inputed) {
		printf ("Send Message to all HIDDEN Fetion Friends.\n usage: %s -f mobile -p passwd -m message.\n",argv[0]);
		return 1;
	}

	if(LOF_INIT(mobileno, password))
		return 1;

	LOF_TAKEOVER();

	LOF_DATA_LocalUser_free(LOF_GLOBAL_User);
	return 0;

}
