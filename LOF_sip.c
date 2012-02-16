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

#define _XOPEN_SOURCE
#define LOF_SIP_BUFFER_SIZE 2048
#include "LOF_openfetion.h"
#include <time.h>

int callid = 1;

LOF_SIP_FetionSipType* LOF_SIP_FetionSip_new(LOF_CONNECTION_FetionConnectionType* tcp , const char* sid)
{
	LOF_SIP_FetionSipType* sip = (LOF_SIP_FetionSipType*)malloc(sizeof(LOF_SIP_FetionSipType));
	memset(sip , 0 , sizeof(LOF_SIP_FetionSipType));
	strcpy(sip->from , sid);
	sip->sequence = 2;
	sip->tcp = tcp;
	sip->header = NULL;
	return sip;
}

LOF_SIP_FetionSipType* LOF_SIP_FetionSip_clone(LOF_SIP_FetionSipType* sip)
{
	LOF_SIP_FetionSipType* res = (LOF_SIP_FetionSipType*)malloc(sizeof(LOF_SIP_FetionSipType));
	memset(res , 0 , sizeof(LOF_SIP_FetionSipType));
	memcpy(res , sip , sizeof(LOF_SIP_FetionSipType));
	sip->header = NULL;
	return res;
}

LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_new(const char* name , const char* value)
{
	LOF_SIP_SipHeaderType* header = (LOF_SIP_SipHeaderType*)malloc(sizeof(LOF_SIP_SipHeaderType));
	memset(header , 0 , sizeof(LOF_SIP_SipHeaderType));
	strcpy(header->name , name);
	header->value = (char*)malloc(strlen(value) + 2 );
	memset(header->value , 0 , strlen(value) + 2);
	strcpy(header->value , value);
	header->next = NULL;
	return header;
}

void LOF_SIP_FetionSip_set_type(LOF_SIP_FetionSipType *sip, LOF_TOOL_SipType type)
{
	if(!sip) return;
	sip->type = type;
	sip->callid = callid;
}

void LOF_SIP_FetionSip_set_callid(LOF_SIP_FetionSipType* sip , int callid)
{
	sip->callid = callid;
}

LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_authentication_new(const char* response)
{
	int len;
	char* res;
	char start[] = "Digest response=\"";
	char end[]   = "\",algorithm=\"SHA1-sess-v4\"";
	LOF_SIP_SipHeaderType* header;
	
	len = strlen(start) + strlen(end) + strlen(response) + 10;
	res = (char*)malloc(len);
	memset(res, 0 , len);
	sprintf(res, "%s%s%s" , start , response , end);
	header = (LOF_SIP_SipHeaderType*)malloc(sizeof(LOF_SIP_SipHeaderType));
	memset(header , 0 , sizeof(LOF_SIP_SipHeaderType));
	strcpy(header->name , "A");
	header->value = res;
	return header;
}

LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_ack_new(const char* code , const char* algorithm , const char* type , const char* guid)
{
	char ack[512];
	sprintf(ack , "Verify response=\"%s\",algorithm=\"%s\",type=\"%s\",chid=\"%s\""
			 	, code , algorithm , type , guid);
	return LOF_SIP_SipHeader_new("A" , ack);
}

LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_event_new(int eventType)
{
	char event[48];
	memset(event, 0, sizeof(event));
	switch(eventType)
	{
		case LOF_SIP_EVENT_PRESENCE :
			strcpy(event , "PresenceV4");
			break;
		case LOF_SIP_EVENT_SETPRESENCE :
			strcpy(event , "SetPresenceV4");
			break;
		case LOF_SIP_EVENT_CATMESSAGE :
			strcpy(event , "CatMsg");
			break;
		case LOF_SIP_EVENT_SENDCATMESSAGE :
			strcpy(event , "SendCatSMS");
			break;
		case LOF_SIP_EVENT_STARTCHAT :
			strcpy(event , "StartChat");
			break;
		case LOF_SIP_EVENT_GETCONTACTINFO :
			strcpy(event , "GetContactInfoV4");
			break;
		case LOF_SIP_EVENT_CONVERSATION :
			strcpy(event , "Conversation");
			break;
		case LOF_SIP_EVENT_INVITEBUDDY :
			strcpy(event , "InviteBuddy");
			break;
		case LOF_SIP_EVENT_CREATEBUDDYLIST :
			strcpy(event , "CreateBuddyList");
			break;
		case LOF_SIP_EVENT_DELETEBUDDYLIST :
			strcpy(event , "DeleteBuddyList");
			break;
		case LOF_SIP_EVENT_SETCONTACTINFO :
			strcpy(event , "SetContactInfoV4");
			break;
		case LOF_SIP_EVENT_SETUSERINFO :
			strcpy(event , "SetUserInfoV4");
			break;
		case LOF_SIP_EVENT_SETBUDDYLISTINFO :
			strcpy(event , "SetBuddyListInfo");
			break;
		case LOF_SIP_EVENT_DELETEBUDDY :
			strcpy(event , "DeleteBuddyV4");
			break;
		case LOF_SIP_EVENT_ADDBUDDY :
			strcpy(event , "AddBuddyV4");
			break;
		case LOF_SIP_EVENT_KEEPALIVE :
			strcpy(event , "KeepAlive");
			break;
		case LOF_SIP_EVENT_DIRECTSMS :
			strcpy(event , "DirectSMS");
			break;
		case LOF_SIP_EVENT_HANDLECONTACTREQUEST :
			strcpy(event , "HandleContactRequestV4");
			break;
		case LOF_SIP_EVENT_SENDDIRECTCATSMS :
			strcpy(event , "SendDirectCatSMS");
			break;
		case LOF_SIP_EVENT_PGGETGROUPLIST:
			strcpy(event , "PGGetGroupList");
			break;
		case LOF_SIP_EVENT_PGGETGROUPINFO:
			strcpy(event , "PGGetGroupInfo");
			break;
		case LOF_SIP_EVENT_PGPRESENCE:
			strcpy(event , "PGPresence");
			break;
		case LOF_SIP_EVENT_PGGETGROUPMEMBERS:
			strcpy(event , "PGGetGroupMembers");
			break;
		case LOF_SIP_EVENT_PGSENDCATSMS:
			strcpy(event , "PGSendCatSMS");
			break;
		default:
			break;
	}
	return LOF_SIP_SipHeader_new("N" , event);
}

LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_credential_new(const char* credential)
{
	char value[64];
	memset(value , 0, sizeof(value));
	sprintf(value , "TICKS auth=\"%s\"" , credential);
	return LOF_SIP_SipHeader_new("A" , value);
}

void LOF_SIP_FetionSip_add_header(LOF_SIP_FetionSipType* sip , LOF_SIP_SipHeaderType* header)
{
	LOF_SIP_SipHeaderType* pos = sip->header;
	if(pos == NULL)
	{
		sip->header = header;
		return;
	}
	while(pos != NULL)
	{
		if(pos->next == NULL)
		{
			pos->next = header;
			break;
		}
		pos = pos->next;
	}
}

char* LOF_SIP_to_string(LOF_SIP_FetionSipType* sip , const char* body)
{
	char *res , *head , buf[1024] , type[128];
	LOF_SIP_SipHeaderType *pos , *tmp;
	int len = 0;

	pos = sip->header;
	while(pos){
		len += (strlen(pos->value) + strlen(pos->name) + 5);
		pos = pos->next;
	}
	len += (body == NULL ? 100 : strlen(body) + 100 );
	res = (char*)malloc(len + 1);
	memset(res , 0 , len + 1);
	memset(type, 0 , sizeof(type));
	switch(sip->type){
		case LOF_SIP_REGISTER     : strcpy(type , "R");
			break;
		case LOF_SIP_SUBSCRIPTION :	strcpy(type , "SUB");
			break;
		case LOF_SIP_SERVICE 	  : strcpy(type , "S");
			break;
		case LOF_SIP_MESSAGE      : strcpy(type , "M");
			break;
		case LOF_SIP_INCOMING	  : strcpy(type , "IN");
			break;
		case LOF_SIP_OPTION 	  : strcpy(type , "O");
			break;
		case LOF_SIP_INVITATION	: strcpy(type , "I");
			break;
		case LOF_SIP_ACKNOWLEDGE	: strcpy(type , "A");
			break;
		default:
			break;
	};

	if(*type == '\0'){
		free(res);
		return NULL;
	}

	sprintf(buf, "%s fetion.com.cn SIP-C/4.0\r\n"
			"F: %s\r\n"
			"I: %d\r\n"
			"Q: 2 %s\r\n",
			type,
			sip->from,
			sip->callid,
			type);

	strcat(res , buf);

	pos = sip->header;
	while(pos){
		len = strlen(pos->value) + strlen(pos->name) + 5;
		head = (char*)malloc(len);
		sprintf(head , "%s: %s\r\n" , pos->name , pos->value);
		strcat(res , head);
		tmp = pos;
		pos = pos->next;
		free(head);
		free(tmp->value);
		free(tmp);
	}
	if(body){
		sprintf(buf , "L: %d\r\n\r\n" , (int)strlen(body));
		strcat(res , buf);
		strcat(res , body);
	}else{
		strcat(res , "\r\n");
	}
	callid ++;
	sip->header = NULL;
	return res;
}
void LOF_SIP_FetionSip_free(LOF_SIP_FetionSipType* sip)
{
	LOF_debug_info("Free sip struct and close socket");
	if(sip != NULL)
		LOF_CONNECTION_FetionConnection_free(sip->tcp);
	free(sip);
}

char* LOF_SIP_get_sid_by_sipuri(const char* sipuri)
{
	char *res , *pos;
	int n;
	pos = strstr(sipuri , ":") + 1;
	n = strlen(pos) - (strstr(pos , "@") == 0 ? 0 : strlen(strstr(pos , "@"))) ;
	res = (char*)malloc(n + 1);
	memset(res , 0 , n + 1);
	strncpy(res , pos , n);
	return res;
}

char* LOF_SIP_get_pgid_by_sipuri(const char *pgsipuri)
{
	char *res , *pos;
	int n;
	if(strstr(pgsipuri , "PG") == NULL)
	    return NULL;
	pos = strstr(pgsipuri , "PG") + 2;
	n = strlen(pos) - (strstr(pos , "@") == 0 ? 0 : strlen(strstr(pos , "@"))) ;
	res = (char*)malloc(n + 1);
	memset(res , 0 , n + 1);
	strncpy(res , pos , n);
	return res;
}

int LOF_SIP_get_attr(const char* sip , const char* name , char* result)
{
	char m_name[16];
	char* pos;
	int n;

	sprintf(m_name , "%s: " , name);
	if(strstr(sip , m_name) == NULL)
		return -1;
	pos = strstr(sip , m_name) + strlen(m_name);
	if(strstr(pos , "\r\n") == NULL)
		n = strlen(pos);
	else
		n = strlen(pos) - strlen(strstr(pos , "\r\n"));
	strncpy(result , pos , n);
	return 1;
}

int LOF_SIP_get_length(const char* sip)
{
	char res[6];
	char name[] = "L";
	memset(res, 0, sizeof(res));
	if(LOF_SIP_get_attr(sip , name , res) == -1)
		return 0;
	return atoi(res);
}
int LOF_SIP_get_code(const char* sip)
{
	char *pos , res[32];
	int n;

	memset(res, 0, sizeof(res));
	if(strstr(sip , "4.0 ") == NULL)
	    return 400;
	pos = strstr(sip , "4.0 ") + 4;
	if(strstr(pos , " ") == NULL)
	    return 400;
	n = strlen(pos) - strlen(strstr(pos , " "));
	strncpy(res , pos , n);
	return atoi(res);
}
int LOF_SIP_get_type(const char* sip)
{
	char res[128];
	int n;

	if(!strstr(sip, " "))
		return LOF_SIP_UNKNOWN;

	n = strlen(sip) - strlen(strstr(sip , " "));
	memset(res, 0, sizeof(res));
	strncpy(res , sip , n);
	if(strcmp(res , "I") == 0 )
		return LOF_SIP_INVITATION;
	if(strcmp(res , "M") == 0 )
		return LOF_SIP_MESSAGE;
	if(strcmp(res , "BN") == 0)
		return LOF_SIP_NOTIFICATION;
	if(strcmp(res , "SIP-C/4.0") == 0
		|| strcmp(res , "SIP-C/2.0") == 0)
		return LOF_SIP_SIPC_4_0;
	if(strcmp(res , "IN") == 0)
		return LOF_SIP_INCOMING;
	if(strcmp(res , "O") == 0 )
		return LOF_SIP_OPTION;
	return LOF_SIP_UNKNOWN;
		
}
char* LOF_SIP_get_response(LOF_SIP_FetionSipType* sip)
{
	char *res;
	int len , n;
   	int c, c1;
	char buf[1024 * 20];

	memset(buf, 0, sizeof(buf));

	if((c = LOF_CONNECTION_FetionConnection_recv(sip->tcp , buf , sizeof(buf) - 2)) == -1) return (char*)0;

	len = LOF_SIP_get_length(buf);

	while(strstr(buf , "\r\n\r\n") == NULL && c < (int)sizeof(buf))
		c += LOF_CONNECTION_FetionConnection_recv(sip->tcp, buf + c, sizeof(buf) - c - 1);
	

	n = strlen(buf) - strlen(strstr(buf , "\r\n\r\n") + 4);
	len += n;
	if(!(res = (char*)malloc(len + 10))) return (char*)0; 

	memset(res, 0, len + 10);
	strcpy(res, buf);
	if(c < len) {
	   	for(;;) {
			memset(buf, 0, sizeof(buf));
			if((c1 = LOF_CONNECTION_FetionConnection_recv(sip->tcp , buf
					, len -c < (int)(sizeof(buf) - 1) ? len -c : (int)(sizeof(buf) - 1) ))
				== -1) {free(res); return (char*)0; }

			strncpy(res + c, buf, c1);
			c += c1;
			if(c >= len) {
				break;
			}
		}
	}
	return res;
}
void LOF_DATA_SipMsg_append(LOF_DATA_SipMsgType* msglist , LOF_DATA_SipMsgType* msg)
{
	LOF_DATA_SipMsgType* pos = msglist;
	while(pos != NULL)
	{
		if(pos->next == NULL)
		{
			pos->next = msg;
			break;
		}
		pos = pos->next;
	}
}

void LOF_SIP_FetionSip_set_connection(LOF_SIP_FetionSipType* sip , LOF_CONNECTION_FetionConnectionType* conn)
{
	sip->tcp = conn;
}
void LOF_SIP_get_auth_attr(const char* auth , char** ipaddress , int* port , char** credential)
{
	char* pos = strstr(auth , "address=\"") + 9;
	int n = strlen(pos) - strlen(strstr(pos , ":"));
	char port_str[6] = { 0 };
	*credential = (char*)malloc(256);
	memset(*credential , 0 , 256);
	*ipaddress = (char*)malloc(256);
	memset(*ipaddress , 0 , 256);
	strncpy(*ipaddress , pos , n);
	pos = strstr(pos , ":") + 1;
	n = strlen(pos) - strlen(strstr(pos , ";"));
	strncpy(port_str , pos , n);
	*port = atoi(port_str);
	pos = strstr(pos , "credential=\"") + 12;
	strncpy(*credential , pos , strlen(pos) - 1);
}

LOF_DATA_SipMsgType *LOF_DATA_SipMsg_new(void)
{
	LOF_DATA_SipMsgType *msg = (LOF_DATA_SipMsgType*)malloc(sizeof(LOF_DATA_SipMsgType));
	memset(msg, 0, sizeof(LOF_DATA_SipMsgType));
	msg->next = NULL;
	return msg;
}

void LOF_DATA_SipMsg_set_msg(LOF_DATA_SipMsgType *sipmsg, const char *msg, int n)
{
	sipmsg->message = (char*)malloc(n + 1);
	memset(sipmsg->message, 0, n + 1);
	strncpy(sipmsg->message, msg, n);
}

#define BUFFER_SIZE (1024 * 100)

/* the following code is hard to read,forgive me! */
LOF_DATA_SipMsgType* LOF_SIP_FetionSip_listen(LOF_SIP_FetionSipType *sip, int *error)
{
	int     n;
	int     body_len;
	char    buffer[BUFFER_SIZE];
	char    holder[BUFFER_SIZE];
	char   *cur;
	char   *pos;
	LOF_DATA_SipMsgType *list = NULL;
	LOF_DATA_SipMsgType *msg;

	*error = 0;

	memset(buffer, 0, sizeof(buffer));
	n = LOF_CONNECTION_FetionConnection_recv_dont_wait(sip->tcp,
				buffer, sizeof(buffer) - 1);
	if(n == 0){
		LOF_debug_info("fetion_sip_listen 0");
		*error = 2;
		return list;
	}
	if(n < 0){
		*error = 1;
		return list;
	}
	cur = buffer;
	/*Got Something ,Do Processing*/
	for(;;){
		pos = strstr(cur, "\r\n\r\n");
		body_len = 0;
		if(pos){
			n = strlen(cur) - strlen(pos);
			memset(holder, 0, sizeof(holder));
			strncpy(holder, cur, n);
			body_len = LOF_SIP_get_length(holder);
		}

		if(cur == NULL || *cur == '\0')
			return list;

		if(body_len == 0 && pos){
			msg = LOF_DATA_SipMsg_new();
			n = strlen(cur) - strlen(pos) + 4;
			LOF_DATA_SipMsg_set_msg(msg, cur, n);
			if(list)
				LOF_DATA_SipMsg_append(list, msg);
			else
				list = msg;
			cur += n;
			continue;
		}

		if((body_len == 0 && !pos) ||
				(body_len != 0 && !pos)){
			memset(holder, 0 , sizeof(holder));
			strcpy(holder, cur);
			LOF_CONNECTION_FetionConnection_recv(sip->tcp,
					holder + strlen(cur),
					BUFFER_SIZE - strlen(cur) - 1);
			memset(buffer, 0 , sizeof(buffer));
			strcpy(buffer, holder);
			cur = buffer;
			continue;
		}else{
			/* now body_len != 0 */
			pos += 4;
			memset(holder, 0 , sizeof(holder));
			if((int)strlen(pos) < body_len){
				strcpy(holder, cur);
				LOF_CONNECTION_FetionConnection_recv(sip->tcp,
					holder + strlen(cur),
					BUFFER_SIZE - strlen(cur) - 1);
				memset(buffer, 0, sizeof(buffer));
				strcpy(buffer, holder);
				cur = buffer;
				continue;
			}else if((int)strlen(pos) == body_len){
				msg = LOF_DATA_SipMsg_new();
				LOF_DATA_SipMsg_set_msg(msg, cur, strlen(cur));
				if(list)
					LOF_DATA_SipMsg_append(list, msg);
				else
					list = msg;
				return list;
			}else{
				msg = LOF_DATA_SipMsg_new();
				n = strlen(cur) - strlen(pos) + body_len;
				LOF_DATA_SipMsg_set_msg(msg, cur, n);
				if(list)
					LOF_DATA_SipMsg_append(list, msg);
				else
					list = msg;

				memset(holder, 0 , sizeof(holder));
				strcpy(holder, cur + n);
				memset(buffer, 0, sizeof(buffer));
				strcpy(buffer, holder);
				cur = buffer;
				continue;
			}
		}
	}

}
int LOF_SIP_keep_alive(LOF_SIP_FetionSipType* sip)
{
	char *res = NULL;
	int ret;

	LOF_debug_info("Send a periodical chat keep alive request");

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_REGISTER);
	res = LOF_SIP_to_string(sip , NULL);
	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	return ret;
}
void LOF_DATA_SipMsg_free(LOF_DATA_SipMsgType* msg)
{
	LOF_DATA_SipMsgType* pos = msg;
	LOF_DATA_SipMsgType* pot;
	while(pos != NULL)
	{
		pot = pos;
		pos = pos->next;
		if(pot != NULL ){
			free(pot->message);
		}
		free(pot);
		pot = NULL;
	}
}
struct tm LOF_TOOL_convert_date(const char* date)
{
	char* pos = strstr(date , ",") + 2;
	struct tm dstr;

	strptime(pos , "%d %b %Y %T %Z" , &dstr);

	dstr.tm_hour += 8;
	if(dstr.tm_hour > 23)
		dstr.tm_hour -= 24;
	return dstr;

}
void LOF_SIP_parse_notification(const char* sip , int* type , int* event , char** xml)
{
	char type1[16] , *pos;
	xmlChar *event1;
	xmlDocPtr doc;
	xmlNodePtr node;
	memset(type1, 0, sizeof(type1));
	LOF_SIP_get_attr(sip , "N" , type1);
	if(strcmp(type1 , "PresenceV4") == 0)
		*type = LOF_NOTIFICATION_TYPE_PRESENCE;
	else if(strcmp(type1 , "Conversation") == 0)
		*type = LOF_NOTIFICATION_TYPE_CONVERSATION;
	else if(strcmp(type1 , "contact") == 0)
		*type = LOF_NOTIFICATION_TYPE_CONTACT;
	else if(strcmp(type1 , "registration") == 0)
		*type = LOF_NOTIFICATION_TYPE_REGISTRATION;
	else if(strcmp(type1 , "SyncUserInfoV4") == 0)
		*type = LOF_NOTIFICATION_TYPE_SYNCUSERINFO;
	else if(strcmp(type1 , "PGGroup") == 0)
	    	*type = LOF_NOTIFICATION_TYPE_PGGROUP;
	else
		*type = LOF_NOTIFICATION_TYPE_UNKNOWN;

	pos = strstr(sip , "\r\n\r\n") + 4;
	*xml = (char*)malloc(strlen(pos) + 1);
	memset(*xml , 0 , strlen(pos) + 1);
	strcpy(*xml , pos);
	doc = xmlReadMemory(*xml , strlen(*xml) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "event");
	*event = LOF_NOTIFICATION_EVENT_UNKNOWN;
	while ((node != NULL) && (*event == LOF_NOTIFICATION_EVENT_UNKNOWN)){
	event1 = xmlGetProp(node ,  BAD_CAST "type");
	if(xmlStrcmp(event1 , BAD_CAST "PresenceChanged") == 0)
		*event = LOF_NOTIFICATION_EVENT_PRESENCECHANGED;
	else if(xmlStrcmp(event1 , BAD_CAST "UserLeft") == 0)
		*event = LOF_NOTIFICATION_EVENT_USERLEFT;
	else if(xmlStrcmp(event1 , BAD_CAST "deregistered") == 0)
		*event = LOF_NOTIFICATION_EVENT_DEREGISTRATION;
	else if(xmlStrcmp(event1 , BAD_CAST "SyncUserInfo") == 0)
		*event = LOF_NOTIFICATION_EVENT_SYNCUSERINFO;
	else if(xmlStrcmp(event1 , BAD_CAST "AddBuddyApplication") == 0)
		*event = LOF_NOTIFICATION_EVENT_ADDBUDDYAPPLICATION;
	else if(xmlStrcmp(event1 , BAD_CAST "PGGetGroupInfo") == 0)
	    	*event = LOF_NOTIFICATION_EVENT_PGGETGROUPINFO;
	else if(xmlStrcmp(event1, BAD_CAST "UserEntered") == 0)
		*event = LOF_NOTIFICATION_EVENT_USERENTER;
	else if(xmlStrcmp(event1, BAD_CAST "UserFailed") == 0)
			*event = LOF_NOTIFICATION_EVENT_USERFAILED;
	else
		*event = LOF_NOTIFICATION_EVENT_UNKNOWN;
	node = node->next;
	}
	xmlFree(event1);
	xmlFreeDoc(doc);
}
void LOF_SIP_parse_message(LOF_SIP_FetionSipType* sip , const char* sipmsg , LOF_DATA_FetionMessageType** msg)
{
	char len[16] , callid[16] , sequence[16] ;
	char sendtime[32] , from[64] , rep[256];
	char memsipuri[64];

	char *pos = NULL;
	xmlDocPtr doc;
	xmlNodePtr node;
	memset(len , 0 , sizeof(len));
	memset(callid , 0 , sizeof(callid));
	memset(sequence , 0 , sizeof(sequence));
	memset(sendtime , 0 , sizeof(sendtime));
	memset(from , 0 , sizeof(from));
	memset(rep, 0, sizeof(rep));
	LOF_SIP_get_attr(sipmsg , "F" , from);
	LOF_SIP_get_attr(sipmsg , "L" , len);
	LOF_SIP_get_attr(sipmsg , "I" , callid);
	LOF_SIP_get_attr(sipmsg , "Q" , sequence);
	LOF_SIP_get_attr(sipmsg , "D" , sendtime);

	*msg = LOF_DATA_FetionMessage_new();

	(*msg)->sysback = 0;
	if(strstr(sipmsg, "SIP-C/3.0") &&
		!strstr(sipmsg, "SIP-C/4.0"))
		(*msg)->sysback = 1;


	if(strstr(from , "PG") != NULL){
	    LOF_DATA_FetionMessage_set_pguri(*msg , from);
	    memset(memsipuri , 0 , sizeof(memsipuri));
	    LOF_SIP_get_attr(sipmsg , "SO" , memsipuri);
	    LOF_DATA_FetionMessage_set_sipuri(*msg , memsipuri);
	}else{
	    LOF_DATA_FetionMessage_set_sipuri(*msg , from);
	}

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos) , "UTF-8" , NULL , XML_PARSE_NOERROR);
	LOF_DATA_FetionMessage_set_time(*msg ,LOF_TOOL_convert_date(sendtime));

	if(doc != NULL){
		node = xmlDocGetRootElement(doc);
		pos = (char*)xmlNodeGetContent(node);
		LOF_DATA_FetionMessage_set_message(*msg , pos);
		free(pos);
		xmlFreeDoc(doc);
	}else{
		LOF_DATA_FetionMessage_set_message(*msg , pos);
	}


	if(strstr(from , "PG") == NULL)
	    sprintf(rep ,"SIP-C/4.0 200 OK\r\nF: %s\r\nI: %s\r\nQ: %s\r\n\r\n"
				    , from , callid , sequence );
	else
	    sprintf(rep ,"SIP-C/4.0 200 OK\r\nI: %s\r\nQ: %s\r\nF: %s\r\n\r\n"
				    , callid , sequence , from);

	LOF_CONNECTION_FetionConnection_send(sip->tcp , rep , strlen(rep));
}
void LOF_SIP_parse_invitation(LOF_SIP_FetionSipType* sip , LOF_CONNECTION_ProxyType *proxy , const char* sipmsg
		, LOF_SIP_FetionSipType** conversionSip , char** sipuri)
{
	char from[50];
	char auth[128];
	char* ipaddress = NULL;
	char buf[1024];
	int port;
	char* credential = NULL;
	LOF_CONNECTION_FetionConnectionType* conn = NULL;
	LOF_SIP_SipHeaderType* aheader = NULL;
	LOF_SIP_SipHeaderType* theader = NULL;
	LOF_SIP_SipHeaderType* mheader = NULL;
	LOF_SIP_SipHeaderType* nheader = NULL;
	char* sipres = NULL;

	memset(from, 0, sizeof(from));
	memset(auth, 0, sizeof(auth));
	memset(buf, 0, sizeof(buf));
	LOF_SIP_get_attr(sipmsg , "F" , from);
	LOF_SIP_get_attr(sipmsg , "A" , auth);
	LOF_SIP_get_auth_attr(auth , &ipaddress , &port , &credential);
	conn = LOF_CONNECTION_FetionConnection_new();

	if(proxy != NULL && proxy->proxyEnabled)
		LOF_CONNECTION_FetionConnection_connect_with_proxy(conn , ipaddress , port , proxy);
	else {
		int ret = LOF_CONNECTION_FetionConnection_connect(conn , ipaddress , port);
		if(ret == -1)
			ret = LOF_CONNECTION_FetionConnection_connect(conn , ipaddress , 443);
		if(ret == -1) {
			LOF_debug_error("Connect to server failed: %s:%d/%s:%d", ipaddress, port, ipaddress, 443);
			return;
		}
	}

	*conversionSip = LOF_SIP_FetionSip_clone(sip);
	LOF_SIP_FetionSip_set_connection(*conversionSip , conn);
	LOF_debug_info("Received a conversation invitation");
	sprintf(buf , "SIP-C/4.0 200 OK\r\nF: %s\r\nI: -61\r\nQ: 200002 I\r\n\r\n"
				, from);
	*sipuri = (char*)malloc(48);
	memset(*sipuri , 0 , 48);
	strcpy(*sipuri , from);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , buf , strlen(buf));

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_REGISTER);
	aheader = LOF_SIP_SipHeader_credential_new(credential);
	theader = LOF_SIP_SipHeader_new("K" , "text/html-fragment");
	mheader = LOF_SIP_SipHeader_new("K" , "multiparty");
	nheader = LOF_SIP_SipHeader_new("K" , "nudge");
	LOF_SIP_FetionSip_add_header(sip , aheader);
	LOF_SIP_FetionSip_add_header(sip , theader);
	LOF_SIP_FetionSip_add_header(sip , mheader);
	LOF_SIP_FetionSip_add_header(sip , nheader);
	sipres = LOF_SIP_to_string(sip , NULL);
	LOF_debug_info("Register to conversation server %s:%d" , ipaddress , port);
	LOF_CONNECTION_FetionConnection_send(conn , sipres , strlen(sipres));
	free(sipres);
	free(ipaddress);
	memset(buf , 0 , sizeof(buf));
//	port = LOF_CONNECTION_FetionConnection_recv(conn , buf , sizeof(buf));

	memset((*conversionSip)->sipuri, 0, sizeof((*conversionSip)->sipuri));
	strcpy((*conversionSip)->sipuri , *sipuri);

}
void LOF_SIP_parse_addbuddyapplication(const char* sipmsg
		, char** sipuri , char** userid , char** desc , int* phrase)
{
	char *pos = NULL;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res = NULL;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "application");

	res = xmlGetProp(node , BAD_CAST "uri");
	*sipuri = (char*)malloc(strlen((char*)res) + 1);
	memset(*sipuri, 0, strlen((char*)res) + 1);
	strcpy(*sipuri , (char*)res);
	xmlFree(res);

	res = xmlGetProp(node , BAD_CAST "user-id");
	*userid = (char*)malloc(strlen((char*)res) + 1);
	memset(*userid, 0, strlen((char*)res) + 1);
	strcpy(*userid , (char*)res);
	xmlFree(res);

	res = xmlGetProp(node , BAD_CAST "desc");
	*desc = (char*)malloc(xmlStrlen(res) + 1);
	memset(*desc, 0, xmlStrlen(res) + 1);
	strcpy(*desc , (char*)res);
	xmlFree(res);

	res = xmlGetProp(node , BAD_CAST "addbuddy-phrase-id");
	*phrase = atoi((char*)res);
	xmlFree(res);

	xmlFreeDoc(doc);

}

void LOF_SIP_parse_incoming(LOF_SIP_FetionSipType* sip
		, const char* sipmsg , char** sipuri
		, LOF_EVENT_IncomingType* type , LOF_EVENT_IncomingActionType *action)
{
	char *pos = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xmlChar *res = NULL;
	char replyMsg[128];
	char callid[10];
	char seq[10];

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	if(xmlStrcmp(node->name , BAD_CAST "share-content") == 0){
		LOF_debug_info("Received a share-content IN message");
		*type = LOF_INCOMING_SHARE_CONTENT;
		/*
		*sipuri = (char*)malloc(48);
		memset(*sipuri, 0, 48);
		LOF_SIP_get_attr(sipmsg , "F" , *sipuri);
		if(! xmlHasProp(node , BAD_CAST "action")){
			*action = LOF_INCOMING_ACTION_UNKNOWN;
			xmlFreeDoc(doc);
			return;
		}
		res = xmlGetProp(node , BAD_CAST "action");
		if(xmlStrcmp(res , BAD_CAST "accept") == 0){
			*action = LOF_INCOMING_ACTION_ACCEPT;
		} else if( xmlStrcmp(res , BAD_CAST "cancel") == 0){
			*action = LOF_INCOMING_ACTION_CANCEL;
		} else {
			*action = LOF_INCOMING_ACTION_UNKNOWN;
		}
		xmlFree(res);
		xmlFreeDoc(doc);
		return;
	}*/}
	if(xmlStrcmp(node->name , BAD_CAST "is-composing") != 0){
		LOF_debug_info("WTF??");
		*type = LOF_INCOMING_UNKNOWN;
		xmlFreeDoc(doc);
		return;
	}
	node = node->xmlChildrenNode;
	res = xmlNodeGetContent(node);
	if(xmlStrcmp(res, BAD_CAST "nudge") == 0 ||
		xmlStrcmp(res, BAD_CAST "input") == 0) {
		*type = LOF_INCOMING_UNKNOWN;
		*sipuri = (char*)malloc(50);
		memset(replyMsg, 0, sizeof(replyMsg));
		memset(callid, 0, sizeof(callid));
		memset(seq, 0, sizeof(seq));
		memset(*sipuri, 0, 50);

		LOF_SIP_get_attr(sipmsg , "I" , callid);
		LOF_SIP_get_attr(sipmsg , "Q" , seq);
		LOF_SIP_get_attr(sipmsg , "F" , *sipuri);
		sprintf(replyMsg , "SIP-C/4.0 200 OK\r\n"
						   "F: %s\r\n"
						   "I: %s \r\n"
						   "Q: %s\r\n\r\n"
						 , *sipuri , callid , seq);
		LOF_CONNECTION_FetionConnection_send(sip->tcp , replyMsg , strlen(replyMsg));
		if(xmlStrcmp(res, BAD_CAST "nudge") == 0)
			{
			*type = LOF_INCOMING_NUDGE;
		}
		else
		{
			*type = LOF_INCOMING_INPUT;
		}
		}
	xmlFree(res);
	xmlFreeDoc(doc);
}

void LOF_SIP_parse_userleft(const char* sipmsg , char** sipuri)
{
	char *pos = NULL;
	xmlDocPtr doc = NULL;
	xmlNodePtr node = NULL;
	xmlChar *res;

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "member");
	res = xmlGetProp(node , BAD_CAST "uri");
	*sipuri = (char*)malloc(xmlStrlen(res) + 1);
	memset(*sipuri, 0, xmlStrlen(res) + 1);
	strcpy(*sipuri , (char*)res);
	xmlFreeDoc(doc);
}

char *LOF_TOOL_generate_action_accept_body(LOF_DATA_ShareType *share)
{
	xmlChar *buf = NULL;
	xmlDocPtr doc;
	xmlNodePtr node , root;
	char body[] = "<share-content></share-content>";

	doc = xmlParseMemory(body , strlen(body));
	root = xmlDocGetRootElement(doc);
	node = xmlNewChild(root , NULL , BAD_CAST "client" , NULL);
	xmlNewProp(node , BAD_CAST "prefer-types" , BAD_CAST share->preferType);
	printf("%s\n" , LOF_TOOL_hexip_to_dotip("3B408066"));
	xmlNewProp(node , BAD_CAST "inner-ip" , BAD_CAST "3B408066");
	xmlNewProp(node , BAD_CAST "net-type" , BAD_CAST "0");
	xmlNewProp(node , BAD_CAST "tcp-port" , BAD_CAST "443");
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}

int LOF_SIP_parse_shareaccept(LOF_SIP_FetionSipType *sip
		, const char* sipmsg , LOF_DATA_ShareType *share)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;
	char callid[16];
	char from[48];
	char seq[16];
	char response[1024];
	char *pos;

	memset(callid, 0, sizeof(callid));
	memset(from, 0, sizeof(from));
	memset(seq, 0, sizeof(seq));
	LOF_SIP_get_attr(sipmsg , "I" , callid);
	LOF_SIP_get_attr(sipmsg , "F" , from);
	LOF_SIP_get_attr(sipmsg , "Q" , seq);
	
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos) , NULL , "UTF-8" , XML_PARSE_RECOVER );
	node = xmlDocGetRootElement(doc);

	node = node->xmlChildrenNode->next;
	if(xmlStrcmp(node->name , BAD_CAST "client") != 0)
		return -1;
	
	res = xmlGetProp(node , BAD_CAST "prefer-types");
	strcpy(share->preferType , (char*)res);
	xmlFree(res);
	
	res = xmlGetProp(node , BAD_CAST "inner-ip");
	pos = LOF_TOOL_hexip_to_dotip((char*)res);
	xmlFree(res);
	strcpy(share->outerIp , pos);
	free(pos);
	pos = NULL;

	res = xmlGetProp(node , BAD_CAST "udp-inner-port");
	share->outerUdpPort = atoi((char*)res);
	xmlFree(res);

	res = xmlGetProp(node , BAD_CAST "tcp-port");
	share->outerTcpPort = atoi((char*)res);
	xmlFree(res);

	pos = LOF_TOOL_generate_action_accept_body(share);
	memset(response, 0, sizeof(response));
	sprintf(response , "SIP-C/4.0 200 OK\r\n"
					   "F: %s\r\n"
					   "I: %s\r\n"
					   "Q: %s\r\n"
					   "L: %d\r\n\r\n%s"
					 , from , callid , seq , (int)strlen(pos) , pos);
	free(pos);
	pos = NULL;

	printf("%s\n" , response);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , response , strlen(response) );
	
	return 1;
}

void LOF_SIP_parse_sysmsg(const char* sipmsg , int *type
		, int* showonce , char **content , char **url){

	char *pos;
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlReadMemory(pos , strlen(pos)
			, NULL , "UTF-8" , XML_PARSE_RECOVER);

	node = xmlDocGetRootElement(doc);
	res = xmlGetProp(node , BAD_CAST "type");
	*type = atoi((char*)type);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "show-once");
	*showonce = atoi((char*)res);
	xmlFree(res);
	node = node->xmlChildrenNode->next->next->next;
	res = xmlNodeGetContent(node);
	*content = (char*)malloc(xmlStrlen(res) + 1);
	memset(*content, 0, xmlStrlen(res) + 1);
	strcpy(*content , (char*)res);
	xmlFree(res);
	node = node->next->next;
	res = xmlNodeGetContent(node);
	*url = (char*)malloc(xmlStrlen(res) + 1);
	memset(*url, 0, xmlStrlen(res) + 1);
	strcpy(*url , (char*)res);
	xmlFree(res);

}

int LOF_SIP_parse_sipc(const char *sipmsg , int *callid , char **xml)
{
	char callid_str[16];
	char *pos;
	int n;
	char code[5];

	pos = strstr(sipmsg , " ") + 1;
	n = strlen(pos) - strlen(strstr(pos , " "));
	strncpy(code , pos , n);
	
	LOF_SIP_get_attr(sipmsg , "I" , callid_str);
	*callid = atoi(callid_str);
/*
	pos = strstr(sipmsg , "\r\n\r\n");

	if(pos)
	    pos += 4;
	else{
	    *xml = NULL;
	    return -1;
	}
	
	*xml = (char*)malloc(strlen(pos) + 1);
	memset(*xml , 0 , strlen(pos) + 1);
	strcpy(*xml , pos);
*/
	return atoi(code);

}



inline void 
LOF_SIP_set_conn(LOF_SIP_FetionSipType *sip, LOF_CONNECTION_FetionConnectionType *conn)
{
	sip->tcp = conn;
}
