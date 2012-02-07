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
 *   51 Franklin Street, Suite 500, Boston, MA 02110-1335, USA.            *
 *                                                                         *
 *   OpenSSL linking exception                                             *
 *   --------------------------                                            *
 *   If you modify this Program, or any covered work, by linking or        *
 *   combining it with the OpenSSL project's "OpenSSL" library (or a       *
 *   modified version of that library), containing parts covered by        *
 *   the terms of OpenSSL/SSLeay license, the licensors of this            *
 *   Program grant you additional permission to convey the resulting       *
 *   work. Corresponding Source for a non-source form of such a            *
 *   combination shall include the source code for the parts of the        *
 *   OpenSSL library used as well as that of the covered work.             *
 ***************************************************************************/

#include "LOF_openfetion.h"

/*extern char* generate_invite_friend_body(const char* sipuri);
extern char* generate_send_nudge_body();
extern void fetion_conversation_parse_send_sms(const char* xml , int* daycount , int* mountcount);
*/
//extern  LOF_DATA_UnackedListType* LOF_GLOBAL_unackedlist;

LOF_USER_ConversationType* LOF_USER_Conversation_new(LOF_DATA_LocalUserType* user,
			  const char* sipuri , LOF_SIP_FetionSipType* sip)
{
	LOF_USER_ConversationType* conversation = (LOF_USER_ConversationType*)malloc(sizeof(LOF_USER_ConversationType));
	memset(conversation , 0 , sizeof(LOF_USER_ConversationType));
	conversation->currentUser = user;
	if(sipuri != NULL)
		conversation->currentContact = 
				LOF_DATA_BuddyContact_list_find_by_sipuri(user->contactList , sipuri);
	else
		conversation->currentContact = NULL;
	if(sipuri != NULL && conversation->currentContact == NULL){
		free(conversation);
		return NULL;
	}
	conversation->currentSip = sip;
	conversation->ready = (sip == NULL )? -1 : 0 ;
	return conversation;
}
LOF_USER_ConversationType* LOF_USER_Conversation_new_sid(LOF_DATA_LocalUserType* user,
			  const char* sid , LOF_SIP_FetionSipType* sip)
{
	LOF_debug_info("Gona Creat Conversation By Sid, Which is %s.",sid);
	LOF_USER_ConversationType* conversation = (LOF_USER_ConversationType*)malloc(sizeof(LOF_USER_ConversationType));
	memset(conversation , 0 , sizeof(LOF_USER_ConversationType));
	conversation->currentUser = user;
	if(sid != NULL)
		conversation->currentContact =
				LOF_DATA_BuddyContact_list_find_by_sid(user->contactList , sid);
	else
		conversation->currentContact = NULL;
	if(sid != NULL && conversation->currentContact == NULL){
		free(conversation);
		return NULL;
	}
	conversation->currentSip = sip;
	conversation->ready = (sip == NULL )? -1 : 0 ;
	return conversation;
}
int LOF_USER_Conversation_send_sms(LOF_USER_ConversationType* conversation , const char* msg)
{
	LOF_SIP_FetionSipType* sip = conversation->currentSip == NULL ?
		   	conversation->currentUser->sip : conversation->currentSip;
	LOF_SIP_SipHeaderType *toheader , *cheader , *kheader , *nheader;
	LOF_DATA_FetionMessageType *message;
	LOF_DATA_UnackedListType *unacked;
	char* res;
	struct tm *now;
	struct tm now_copy;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
	nheader  = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_CATMESSAGE);
	toheader = LOF_SIP_SipHeader_new("T" , conversation->currentContact->sipuri);
	cheader  = LOF_SIP_SipHeader_new("C" , "text/plain");
	kheader  = LOF_SIP_SipHeader_new("K" , "SaveHistory");
	LOF_SIP_FetionSip_add_header(sip , toheader);
	LOF_SIP_FetionSip_add_header(sip , cheader);
	LOF_SIP_FetionSip_add_header(sip , kheader);
	LOF_SIP_FetionSip_add_header(sip , nheader);
	/* add message to list */
	/*now = LOF_TOOL_get_currenttime();
	now_copy = *now;
	message = LOF_DATA_FetionMessage_new();
	LOF_DATA_FetionMessage_set_sipuri(message , conversation->currentContact->sipuri);
	LOF_DATA_FetionMessage_set_time(message , now_copy);
	LOF_DATA_FetionMessage_set_message(message , msg);
	LOF_DATA_FetionMessage_set_callid(message , sip->callid);
	//unacked = LOF_DATA_UnackedList_new(message);
	//LOF_DATA_UnackedList_append(LOF_GLOBAL_unackedlist , unacked);
*/
	res = LOF_SIP_to_string(sip , msg);

	if(LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res)) == -1){
		free(res);
		return -1;
	}
	LOF_debug_info("Sent a message to %s" , conversation->currentContact->sipuri);
	free(res);
	return 1;
}

int LOF_USER_Conversation_send_sms_with_reply(LOF_USER_ConversationType *conv, const char *msg)
{
	char       rep[1024];

	LOF_SIP_FetionSipType* sip = conv->currentSip == NULL ?
		   	conv->currentUser->sip : conv->currentSip;
	LOF_SIP_SipHeaderType *toheader , *cheader , *kheader , *nheader;
	LOF_DATA_FetionMessageType *message;
	char* res;
	struct tm *now;
	struct tm now_copy;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
	nheader  = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_CATMESSAGE);
	toheader = LOF_SIP_SipHeader_new("T" , conv->currentContact->sipuri);
	cheader  = LOF_SIP_SipHeader_new("C" , "text/plain");
	kheader  = LOF_SIP_SipHeader_new("K" , "SaveHistory");
	LOF_SIP_FetionSip_add_header(sip , toheader);
	LOF_SIP_FetionSip_add_header(sip , cheader);
	LOF_SIP_FetionSip_add_header(sip , kheader);
	LOF_SIP_FetionSip_add_header(sip , nheader);
	/* add message to list */
	now = LOF_TOOL_get_currenttime();
	now_copy = *now;
	message = LOF_DATA_FetionMessage_new();
	LOF_DATA_FetionMessage_set_sipuri(message , conv->currentContact->sipuri);
	LOF_DATA_FetionMessage_set_time(message , now_copy);
	LOF_DATA_FetionMessage_set_message(message , msg);
	LOF_DATA_FetionMessage_set_callid(message , sip->callid);

	res = LOF_SIP_to_string(sip , msg);

	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	LOF_debug_info("Sent a message to %s" , conv->currentContact->sipuri);
	free(res);

	memset(rep , 0 , sizeof(rep));
	LOF_CONNECTION_FetionConnection_recv(sip->tcp , rep , sizeof(rep));

	if(LOF_SIP_get_code(rep) == 280 || LOF_SIP_get_code(rep) == 200){
		return 1;
	}else{
		return -1;
	}
}

int LOF_USER_Conversation_send_sms_to_myself(LOF_USER_ConversationType* conversation,
			   	const char* message)
{
	LOF_SIP_SipHeaderType *toheader = NULL;
	LOF_SIP_SipHeaderType *eheader = NULL;
	char* res = NULL;
	LOF_SIP_FetionSipType* sip = conversation->currentUser->sip;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
	toheader = LOF_SIP_SipHeader_new("T" , conversation->currentUser->sipuri);
	eheader  = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SENDCATMESSAGE);
	LOF_SIP_FetionSip_add_header(sip , toheader);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	res = LOF_SIP_to_string(sip , message);
	LOF_debug_info("Sent a message to myself" , conversation->currentContact->sipuri);
	if(LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res)) == -1) {
		free(res);
		return -1;
	}
	free(res);
	res = LOF_SIP_get_response(sip);
	free(res);
	return 1;
}

int LOF_USER_Conversation_send_sms_to_myself_with_reply(LOF_USER_ConversationType* conversation,
			   	const char* message)
{
	LOF_SIP_SipHeaderType *toheader = NULL;
	LOF_SIP_SipHeaderType *eheader = NULL;
	char       *res = NULL;
	char        rep[1024];
	int         code;
	LOF_SIP_FetionSipType  *sip = conversation->currentUser->sip;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
	toheader = LOF_SIP_SipHeader_new("T" , conversation->currentUser->sipuri);
	eheader  = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SENDCATMESSAGE);
	LOF_SIP_FetionSip_add_header(sip , toheader);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	res = LOF_SIP_to_string(sip , message);
	LOF_debug_info("Sent a message to myself" , conversation->currentContact->sipuri);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	memset(rep, 0, sizeof(rep));
	LOF_CONNECTION_FetionConnection_recv(sip->tcp , rep , sizeof(rep));
	code = LOF_SIP_get_code(rep);
	if(code == 200 || code == 280){
		return 1;
	}else{
		return -1;
	}
}

int LOF_USER_Conversation_send_sms_to_phone(LOF_USER_ConversationType* conversation,
			   	const char* message)
{
	
	LOF_SIP_SipHeaderType *toheader = NULL;
	LOF_SIP_SipHeaderType *eheader = NULL;
	LOF_SIP_SipHeaderType *aheader = NULL;
	LOF_DATA_LocalUserType *user = conversation->currentUser;
	char* res = NULL;
	LOF_SIP_FetionSipType* sip = user->sip;
	char* sipuri = conversation->currentContact->sipuri;
	char astr[256] , rep[1024];
	int code;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
	toheader = LOF_SIP_SipHeader_new("T" , sipuri);
	eheader  = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SENDCATMESSAGE);
	LOF_SIP_FetionSip_add_header(sip , toheader);
	if(user->verification != NULL){
		memset(astr, 0, sizeof(astr));
		sprintf(astr , "Verify algorithm=\"picc\",chid=\"%s\",response=\"%s\""
				, user->verification->guid
				, user->verification->code);
		aheader = LOF_SIP_SipHeader_new("A" , astr);
		LOF_SIP_FetionSip_add_header(sip , aheader);
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	res = LOF_SIP_to_string(sip , message);
	LOF_debug_info("Sent a message to (%s)`s mobile phone" , sipuri);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	memset(rep, 0, sizeof(rep));
	LOF_CONNECTION_FetionConnection_recv(sip->tcp , rep , sizeof(rep));
	code = LOF_SIP_get_code(rep);
	if(code == 420 || code == 421){
		return -1;
	}else{
		return 1;
	}
}
int LOF_USER_Conversation_send_sms_to_phone_with_reply(LOF_USER_ConversationType* conversation
		, const char* message , int* daycount , int* monthcount)
{
	
	LOF_SIP_SipHeaderType *toheader , *eheader , *aheader;
	char* res;
	char* xml;
	LOF_DATA_LocalUserType *user = conversation->currentUser;
	LOF_SIP_FetionSipType* sip = user->sip;
	char astr[256] , rep[1024];
	char* sipuri = conversation->currentContact->sipuri;

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
	res = LOF_SIP_to_string(sip , message);
	LOF_debug_info("Sent a message to (%s)`s mobile phone" , sipuri);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);

	memset(rep , 0 , sizeof(rep));
	LOF_CONNECTION_FetionConnection_recv(sip->tcp , rep , sizeof(rep));

	if(LOF_SIP_get_code(rep) == 280){
		xml = strstr(rep , "\r\n\r\n") + 4;
		LOF_USER_Conversation_parse_send_sms(xml , daycount , monthcount);
		return 1;
	}else{
		LOF_debug_error("Send a message to (%s)`s mobile phone failed",
				sipuri);
		return -1;
	}
}
int LOF_USER_Conversation_invite_friend(LOF_USER_ConversationType* conversation)
{
	LOF_SIP_FetionSipType* sip = conversation->currentUser->sip;
	char *res , *ip , *credential , auth[256] , *body;
	int port , ret;
	LOF_CONNECTION_FetionConnectionType* conn;
	LOF_CONNECTION_ProxyType *proxy = conversation->currentUser->config->proxy;
	LOF_SIP_SipHeaderType *eheader , *theader , *mheader , *nheader , *aheader;


	/*start chat*/
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_STARTCHAT);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	res = LOF_SIP_to_string(sip , NULL);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res); res = NULL;
	res = LOF_SIP_get_response(sip);
	if(!res)
		return -1;

	memset(auth , 0 , sizeof(auth));
	LOF_SIP_get_attr(res , "A" , auth);
	if(auth==NULL)
		return -1;

	LOF_SIP_get_auth_attr(auth , &ip , &port , &credential);
	free(res); res = NULL;
	conn = LOF_CONNECTION_FetionConnection_new();

	if(proxy != NULL && proxy->proxyEnabled)
		ret = LOF_CONNECTION_FetionConnection_connect_with_proxy(conn, ip, port, proxy);
	else {
		ret = LOF_CONNECTION_FetionConnection_connect(conn, ip, port);
		if(ret == -1)
			ret = LOF_CONNECTION_FetionConnection_connect(conn, ip, 443);
	}

	if(ret == -1)
		return -1;

	/*clone sip*/
	conversation->currentSip = LOF_SIP_FetionSip_clone(conversation->currentUser->sip);
	memset(conversation->currentSip->sipuri, 0 , sizeof(conversation->currentSip->sipuri));
	strcpy(conversation->currentSip->sipuri , conversation->currentContact->sipuri);
	LOF_SIP_FetionSip_set_connection(conversation->currentSip , conn);
	free(ip); ip = NULL;
	/*register*/
	sip = conversation->currentSip;
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
	free(res);res = NULL;
	free(credential); credential = NULL;
	res = LOF_SIP_get_response(sip);
	free(res); res = NULL;
	/*invite buddy*/
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_INVITEBUDDY);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_invite_friend_body(conversation->currentContact->sipuri);
	res = LOF_SIP_to_string(sip , body);
	free(body); body = NULL;
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res); res = NULL;
	res = LOF_SIP_get_response(sip);

	if(LOF_SIP_get_code(res) == 200)	{
		free(res);
		char lastbuf[2048];
		LOF_CONNECTION_FetionConnection_recv(sip->tcp, lastbuf, sizeof(lastbuf));
		return 1;
	}else{
		free(res);
		return -1;
	}
}
int LOF_USER_Conversation_send_nudge(LOF_USER_ConversationType* conversation)
{
	LOF_SIP_SipHeaderType *toheader = NULL;
	char* res = NULL;
	char* body = NULL;
	LOF_SIP_FetionSipType* sip = conversation->currentSip;
	if(sip == NULL)
	{
		LOF_debug_error("Did not start a chat chanel , can not send a nudge");
		return -1;
	}
	char* sipuri = conversation->currentContact->sipuri;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_INCOMING);
	toheader = LOF_SIP_SipHeader_new("T" , sipuri);
	LOF_SIP_FetionSip_add_header(sip , toheader);
	body = LOF_SIP_generate_send_nudge_body();
	res = LOF_SIP_to_string(sip , body);
	free(body);
	LOF_debug_info("Sent a nudge to (%s)" , sipuri);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
/*	res = fetion_sip_get_response(sip);
	if(fetion_sip_get_code(res) == 280)
	{
		free(res);
		return 1;
	}
	else
	{
		printf("%s\n" , res);
		free(res);
		debug_error("Send nuge failed");
		return -1;
	}*/
	return 1;

}
char* LOF_SIP_generate_invite_friend_body(const char* sipuri)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST sipuri);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}
char* LOF_SIP_generate_send_nudge_body()
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<is-composing></is-composing>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "state" , NULL);
	xmlNodeSetContent(node , BAD_CAST "nudge");
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}
void LOF_USER_Conversation_parse_send_sms(const char* xml , int* daycount , int* mountcount)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar* res;
	doc = xmlParseMemory(xml , strlen(xml));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "frequency");
	res = xmlGetProp(node , BAD_CAST "day-count");
	*daycount = atoi((char*)res);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "month-count");
	*mountcount = atoi((char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
}
