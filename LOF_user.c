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
#include <signal.h>



int LOF_DATA_LocalUser_download_avatar_again(const char* filepath , const char* buf , LOF_CONNECTION_ProxyType *proxy);
char* LOF_SIP_generate_set_status_body(LOF_USER_StatusType state);
 char* LOF_SIP_generate_set_moodphrase_body(const char* customConfigVersion
				, const char* customConfig , const char* personalVersion ,  const char* moodphrase);
 char* LOF_SIP_generate_update_information_body(LOF_DATA_LocalUserType* user);
 char* LOF_SIP_generate_keep_alive_body();
 void LOF_SIP_parse_set_moodphrase_response(LOF_DATA_LocalUserType* user , const char* sipmsg);
 char* LOF_SIP_generate_set_sms_status_body(int days);
 void LOF_SIP_parse_set_sms_status_response(LOF_DATA_LocalUserType *user , const char *sipmsg);

LOF_DATA_LocalUserType* LOF_DATA_LocalUser_new(const char* no , const char* password)
{
	LOF_DATA_LocalUserType* user = (LOF_DATA_LocalUserType*)malloc(sizeof(LOF_DATA_LocalUserType));

 	struct sigaction sa;
 	sa.sa_handler = SIG_IGN;
 	sigaction(SIGPIPE, &sa, 0 );

	memset(user , 0 , sizeof(LOF_DATA_LocalUserType));
	if(strlen(no) == 11){
		strcpy(user->mobileno , no);
		user->loginType = LOF_LOGIN_TYPE_MOBILENO;
	}else{
		strcpy(user->sId , no);
		user->loginType = LOF_LOGIN_TYPE_FETIONNO;
	}
	strcpy(user->password , password);
	user->contactList = LOF_DATA_BuddyContact_new();
	user->groupList = LOF_DATA_BuddyGroup_new();
	user->pggroup = NULL;
	user->sip = NULL;
	user->verification = NULL;
	user->customConfig = NULL;
	user->ssic = NULL;
	user->config = NULL;

//	LOF_GLOBAL_unackedlist = LOF_DATA_UnackedList_new((LOF_DATA_FetionMessageType*)NULL);

	return user;
}

void LOF_DATA_LocalUser_set_userid(LOF_DATA_LocalUserType* user, const char* userid1)
{
	strcpy(user->userId, userid1);
}

void LOF_DATA_LocalUser_set_sid(LOF_DATA_LocalUserType* user, const char* sId1)
{
	strcpy(user->sId, sId1);
}

void LOF_DATA_LocalUser_set_mobileno(LOF_DATA_LocalUserType* user , const char* mobileno1)
{
	strcpy(user->mobileno , mobileno1);
}

void LOF_DATA_LocalUser_set_password(LOF_DATA_LocalUserType *user, const char *password)
{
	strcpy(user->password, password);
	user->password[strlen(password)] = '\0';
}

void LOF_DATA_LocalUser_set_sip(LOF_DATA_LocalUserType* user , LOF_SIP_FetionSipType* sip1)
{
	LOF_debug_info("Set a initialized Sip Struct to User");
	user->sip = sip1;
}

void LOF_DATA_LocalUser_set_config(LOF_DATA_LocalUserType* user , LOF_TOOL_ConfigType* config1)
{
	LOF_debug_info("Set a initialized Config Struct to User");
	user->config = config1;
}

void LOF_DATA_LocalUser_set_verification_code(LOF_DATA_LocalUserType* user , const char* code)
{
	user->verification->code = (char*)malloc(strlen(code) + 1);
	memset(user->verification->code , 0 , strlen(code) + 1);
	strcpy(user->verification->code , code);
}

int LOF_DATA_LocalUser_init_config(LOF_DATA_LocalUserType *user)
{
	assert(user != NULL);
	assert(user->config != NULL);
	assert(*(user->userId) != '\0');
	return LOF_TOOL_Config_initialize(user->config, user->userId);
}

void LOF_DATA_LocalUser_free(LOF_DATA_LocalUserType* user)
{
	if(user->ssic != NULL)
		free(user->ssic);
	if(user->customConfig != NULL)
		free(user->customConfig);
	if(user->verification != NULL)
		LOF_DATA_Verification_free(user->verification);
	if(user->sip != NULL) 
		LOF_SIP_FetionSip_free(user->sip);
	if(user->config != NULL)
		LOF_TOOL_Config_free(user->config);
	free(user);
}
int LOF_DATA_LocalUser_set_status(LOF_DATA_LocalUserType* user , LOF_USER_StatusType state)
{
	LOF_SIP_SipHeaderType* eheader;
	LOF_SIP_FetionSipType* sip = user->sip;
	char* body;
	char* res;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETPRESENCE);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_set_status_body(state);
	res = LOF_SIP_to_string(sip , body);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	user->status = state;
	free(body);
	free(res);
	LOF_debug_info("User status changed to %d" , state);
	return 1;
}
int LOF_DATA_LocalUser_set_moodphrase(LOF_DATA_LocalUserType* user , const char* moodphrase)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	LOF_debug_info("Start seting moodphrase");
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETUSERINFO);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_set_moodphrase_body(user->customConfigVersion
									  , user->customConfig
									  , user->personalVersion
									  , moodphrase);
	res = LOF_SIP_to_string(sip , body);
	free(body);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res) ; 
	res = LOF_SIP_get_response(sip);
	ret = LOF_SIP_get_code(res);
	if(ret == 200){
		LOF_SIP_parse_set_moodphrase_response(user , res);
		free(res);
		LOF_debug_info("Set moodphrase success");
		return 1;
	}else{
		free(res);
		LOF_debug_error("Set moodphrase failed , errno :" , ret);
		return -1;
	}

}
int LOF_DATA_LocalUser_update_info(LOF_DATA_LocalUserType* user)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader = NULL;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	LOF_debug_info("Start Updating User Information");
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETUSERINFO);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_update_information_body(user);
	res = LOF_SIP_to_string(sip , body);
	free(body);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res) ; 
	res = LOF_SIP_get_response(sip);
	ret = LOF_SIP_get_code(res);

	if(ret == 200){
		free(res);
		LOF_debug_info("Update information success");
		return 1;
	}else{
		free(res);
		LOF_debug_error("Update information failed , errno :" , ret);
		return -1;
	}
}
int LOF_DATA_LocalUser_keep_alive(LOF_DATA_LocalUserType* user)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader = NULL;
	int ret;
	char *res = NULL , *body = NULL;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_REGISTER);
	//LOF_debug_info("sent a keep alive request");
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_KEEPALIVE);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_keep_alive_body();
	res = LOF_SIP_to_string(sip , body);
	free(body);
	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res); 
	return ret;
}
LOF_DATA_BuddyGroupType* LOF_DATA_BuddyGroup_new()
{
	LOF_DATA_BuddyGroupType* list = (LOF_DATA_BuddyGroupType*)malloc(sizeof(LOF_DATA_BuddyGroupType));
	memset(list , 0 , sizeof(LOF_DATA_BuddyGroupType));
	list->pre = list;
	list->next = list;
	return list;
}
void LOF_DATA_BuddyGroup_list_append(LOF_DATA_BuddyGroupType* head , LOF_DATA_BuddyGroupType* group)
{
	head->next->pre = group;
	group->next = head->next;
	group->pre = head;
	head->next = group;
}

void LOF_DATA_BuddyGroup_list_prepend(LOF_DATA_BuddyGroupType* head , LOF_DATA_BuddyGroupType* group)
{
	head->pre->next = group;
	group->next = head;
	group->pre = head->pre;
	head->pre = group;
}

void LOF_DATA_BuddyGroup_list_remove(LOF_DATA_BuddyGroupType *group)
{
	group->next->pre = group->pre;
	group->pre->next = group->next;
}

void LOF_DATA_BuddyGroup_remove(LOF_DATA_BuddyGroupType* head , int groupid)
{
	LOF_DATA_BuddyGroupType *gl_cur;
	foreach_grouplist(head , gl_cur){
		if(gl_cur->groupid == groupid){
			gl_cur->pre->next = gl_cur->next;
			gl_cur->next->pre = gl_cur->pre;
			free(gl_cur);
			break;
		}
	}
}
LOF_DATA_BuddyGroupType* LOF_DATA_BuddyGroup_list_find_by_id(LOF_DATA_BuddyGroupType* head , int id)
{
	LOF_DATA_BuddyGroupType *gl_cur;
	foreach_grouplist(head , gl_cur){
		if(gl_cur->groupid == id){
			return gl_cur;
		}
	}
	return NULL;
}
LOF_DATA_VerificationType* LOF_DATA_Verification_new()
{
	LOF_DATA_VerificationType* ver = (LOF_DATA_VerificationType*)malloc(sizeof(LOF_DATA_VerificationType));
	memset(ver , 0 , sizeof(LOF_DATA_VerificationType));
	ver->algorithm = NULL;
	ver->type = NULL;
	ver->text = NULL;
	ver->tips = NULL;
	return ver;
}
void LOF_DATA_Verification_free(LOF_DATA_VerificationType* ver)
{
	if(ver != NULL){
		free(ver->algorithm);
		free(ver->type);
		free(ver->text);
		free(ver->tips);
		free(ver->guid);
		free(ver->code);
	}
	free(ver);
}

int LOF_DATA_LocalUser_upload_avatar(LOF_DATA_LocalUserType* user , const char* filename)
{
	char http[1024];
	unsigned char buf[1024];
	char res[1024];
	char code[4];
	char* ip = NULL;
	FILE* f = NULL;
	LOF_TOOL_ConfigType *config = user->config;
	char* server = config->avatarServerName;
	char* portraitPath = config->avatarServerPath;
	LOF_CONNECTION_ProxyType *proxy = config->proxy;
	long filelength;
	int n;
	LOF_CONNECTION_FetionConnectionType* tcp;

	ip = LOF_TOOL_get_ip_by_name(server);
	if(ip == NULL){
		LOF_debug_error("Parse server ip address failed , %s" , server);
		return -1;
	}

	f = fopen(filename , "r");
	fseek(f , 0 , SEEK_END);
	filelength = ftell(f);
	rewind(f);
	LOF_debug_info("uploading avatar....");
	sprintf(http , "POST /%s/setportrait.aspx HTTP/1.1\r\n"
		    	   "Cookie: ssic=%s\r\n"
				   "Accept: */*\r\n"
		    	   "Host: %s\r\n"
		    	   "Content-Length: %ld\r\n"
		    	   "Content-Type: image/jpeg\r\n"
		    	   "User-Agent: IIC2.0/PC 4.0.0000\r\n"
				   "Connection: Keep-Alive\r\n"
				   "Cache-Control: no-cache\r\n\r\n"
		  		  , portraitPath , user->ssic , server , filelength);

	tcp = LOF_CONNECTION_FetionConnection_new();
	if(proxy != NULL && proxy->proxyEnabled)
		LOF_CONNECTION_FetionConnection_connect_with_proxy(tcp , ip , 80 , proxy);
	else
		LOF_CONNECTION_FetionConnection_connect(tcp , ip , 80);

	LOF_CONNECTION_FetionConnection_send(tcp , http , strlen(http));

	memset(buf , 0 , sizeof(buf));
	int ret;
	while((n = fread(buf , 1 , sizeof(buf) , f))){
		ret = LOF_CONNECTION_FetionConnection_send(tcp , buf , n) ;
		if(ret == -1){
			fclose(f);
			return -1;
		}
		memset(buf , 0 , sizeof(buf));
	}
	fclose(f);

	memset(res, 0, sizeof(res));
	LOF_CONNECTION_FetionConnection_recv(tcp , res , sizeof(res));
	memset(code, 0, sizeof(code));
	strncpy(code , res + 9 , 3);
	if(strcmp(code , "200") == 0){
		LOF_debug_info("Upload portrait success");
		return 1;
	}else{
		LOF_debug_error("Upload portrait failed");
		return -1;
	}
}

int LOF_DATA_LocalUser_set_sms_status(LOF_DATA_LocalUserType *user , int days)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *eheader;
	char buffer[2048];
	char code[16];
	char *body;
	char *res;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETUSERINFO);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_set_sms_status_body(days);
	res = LOF_SIP_to_string(sip , body);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	memset(buffer , 0 , sizeof(buffer));
	LOF_CONNECTION_FetionConnection_recv(sip->tcp , buffer , sizeof(buffer));

	res = strstr(buffer , " ") + 1;
	memset(code , 0 , sizeof(code));
	strncpy(code , res , 3);
	if(strcmp(code , "200") == 0){
		LOF_SIP_parse_set_sms_status_response(user , buffer);
		LOF_debug_info("set sms online status to %d days[%s]"
					   	, days , user->smsOnLineStatus);
		return 1;
	}else{
		LOF_debug_error("failed to set sms online status");
		return -1;
	}

	return 1;
}

int LOF_DATA_LocalUser_download_avatar(LOF_DATA_LocalUserType* user , const char* sipuri)
{
    char uri[256];
	char *server = user->config->avatarServerName;
	char *portraitPath = user->config->avatarServerPath;
	sprintf(uri , "/%s/getportrait.aspx" , portraitPath);

	return LOF_DATA_LocalUser_download_avatar_with_uri(user , sipuri , server , uri);
}

int LOF_DATA_LocalUser_download_avatar_with_uri(LOF_DATA_LocalUserType *user , const char *sipuri
       	, const char *server , const char *portraitpath)
{
	char buf[2048] , *ip , *pos = NULL;
	FILE *f = NULL;
	char filename[256];
	char *encodedSipuri , *encodedSsic , replyCode[4] = { 0 };
	char *friendSid = NULL;
	LOF_TOOL_ConfigType *config = user->config;
	LOF_CONNECTION_FetionConnectionType* tcp = NULL;
	int i = 0 , isFirstChunk = 0 , chunkLength = 0 , imageLength = 0 , receivedLength = 0;
	int ret;

	ip = LOF_TOOL_get_ip_by_name(server);
	if(ip == NULL)
	{
		LOF_debug_error("Parse server ip address failed , %s" , server);
		return -1;
	}
	if(! sipuri || *sipuri == '\0')
		return -1;
	friendSid = LOF_SIP_get_sid_by_sipuri(sipuri);
	if(friendSid == NULL)
		return -1;
/*	open a file to write ,if not exist then create one*/
	sprintf(filename , "%s/%s.jpg" , config->iconPath ,  friendSid);
	free(friendSid);
	encodedSipuri = LOF_TOOL_http_connection_encode_url(sipuri);
	encodedSsic = LOF_TOOL_http_connection_encode_url(user->ssic);
	sprintf(buf , "GET %s?Uri=%s"
			  "&Size=120&c=%s HTTP/1.1\r\n"
			  "User-Agent: IIC2.0/PC "LOF_PROTO_VERSION"\r\n"
			  "Accept: image/pjpeg;image/jpeg;image/bmp;"
			  "image/x-windows-bmp;image/png;image/gif\r\n"
			  "Host: %s\r\nConnection: Keep-Alive\r\n\r\n"
			  , portraitpath , encodedSipuri , encodedSsic , server);

	tcp = LOF_CONNECTION_FetionConnection_new();
	if(config->proxy != NULL && config->proxy->proxyEnabled)
		ret = LOF_CONNECTION_FetionConnection_connect_with_proxy(tcp , ip , 80 , config->proxy);
	else
		ret = LOF_CONNECTION_FetionConnection_connect(tcp, ip, 80);
	if(ret < 0){
		LOF_debug_error("connect to avatar server:%s",
				strerror(errno));
		return -1;
	}
	free(ip);
	ret = LOF_CONNECTION_FetionConnection_send(tcp , buf , strlen(buf));
	if(ret < 0)
		return -1;

	//read reply

	/* 200 OK begin to download protrait ,
	 * 302 need to redirect ,404 not found */
	for(;;){
		memset(buf, 0, sizeof(buf));
		chunkLength = LOF_CONNECTION_FetionConnection_recv(tcp , buf , sizeof(buf) -1);
		if(chunkLength < 0)
			break;
		if(isFirstChunk == 0)
		{	
			/* check the code num for the first segment*/
			memcpy(replyCode , buf + 9 , 3);
			switch(atoi(replyCode))
			{   
				/*	no protrait for current user found
				 * ,just return a error */
				case 404:
					goto end;
					break;
				/*write the image bytes of the first segment into file*/
				case 200:
					f = fopen(filename , "wb+");
					if( f == NULL )
					{
						LOF_debug_error("Write user portrait to local disk failed");
						return -1;
					}
					pos = (char*)buf;
					imageLength = LOF_TOOL_http_connection_get_body_length(pos);
					receivedLength = chunkLength - LOF_TOOL_http_connection_get_head_length(pos) -4;
					for(i = 0 ; i < chunkLength ; i++ )
						if( buf[i] == '\r' && buf[i+1] == '\n'
							&&buf[i+2] == '\r' && buf[i+3] == '\n' )
						{
							fwrite(buf + i + 4 , chunkLength - i -4 , 1 ,f);
							fflush(f);
							break;
						}
					if(receivedLength == imageLength)
						goto end;
					break;
				default:
					goto redirect;
					break;
			};
			isFirstChunk ++;
		}
		else
		{
			if(strcmp(replyCode , "200") == 0){
				fwrite(buf , chunkLength , 1 , f);
				fflush(f);
			}
			receivedLength += chunkLength;
			if(receivedLength == imageLength)
				break;
		}
	}
	if(strcmp(replyCode , "200") == 0)
	{
		fclose(f);
		f = NULL;
		LOF_CONNECTION_FetionConnection_free(tcp);
		tcp = NULL;
		return 0;
	}
redirect:
	if(strcmp(replyCode , "302") == 0)
		ret = LOF_DATA_LocalUser_download_avatar_again(filename , buf , config->proxy);
end:
	if(f != NULL)
		fclose(f);
	LOF_CONNECTION_FetionConnection_free(tcp);
	tcp = NULL;
	if(ret < 0)
		return -1;
	return 1;
}

int LOF_DATA_LocalUser_download_avatar_again(const char* filepath , const char* buf , LOF_CONNECTION_ProxyType* proxy)
{
	char location[1024] = { 0 };
	char httpHost[50] = { 0 };
	char httpPath[512] = { 0 };
	char http[1024] = { 0 };
	char replyCode[5] = { 0 };
	FILE* f = NULL;
	LOF_CONNECTION_FetionConnectionType* tcp = NULL;
	char* ip = NULL;
	char* pos = strstr(buf , "Location: ") ;
	int chunkLength = 0 , imageLength = 0 , receivedLength = 0;
	int i , n = 0;
	int ret;
	
	int isFirstChunk = 0;
	unsigned char img[2048] = { 0 };

	if(pos == NULL)
		return -1;
	pos += 10;
	n = strlen(pos) - strlen(strstr(pos , "\r\n"));
	strncpy(location , pos , n );
	pos = location + 7;
	n = strlen(pos) - strlen(strstr(pos , "/"));
	strncpy(httpHost , pos , n);
	pos += n;
	strcpy(httpPath , pos);
	sprintf(http , "GET %s HTTP/1.1\r\n"
				   "User-Agent: IIC2.0/PC 3.3.0370\r\n"
			 	   "Accept: image/pjpeg;image/jpeg;image/bmp;"
				   "image/x-windows-bmp;image/png;image/gif\r\n"
				   "Cache-Control: private\r\n"
				   "Host: %s\r\n"
				   "Connection: Keep-Alive\r\n\r\n" , httpPath , httpHost);
	ip = LOF_TOOL_get_ip_by_name(httpHost);
	if(ip == NULL){
		LOF_debug_error("Parse portrait server ip address failed , %s" , httpHost);
		return -1;
	}
	tcp = LOF_CONNECTION_FetionConnection_new();

	if(proxy != NULL && proxy->proxyEnabled)
		ret = LOF_CONNECTION_FetionConnection_connect_with_proxy(tcp , ip , 80 , proxy);
	else
		ret = LOF_CONNECTION_FetionConnection_connect(tcp , ip , 80);

	if(ret < 0)
		return -1;

	free(ip);
	ret = LOF_CONNECTION_FetionConnection_send(tcp , http , strlen(http));
	if(ret < 0)
		return -1;
	//read portrait data
	f = fopen(filepath , "wb+");
	for(;;){
		memset(img, 0 , sizeof(img));
		chunkLength = LOF_CONNECTION_FetionConnection_recv(tcp , img , sizeof(img)-1);
		if(chunkLength <= 0)
			break;
		if(isFirstChunk ++ == 0)
		{
			char* pos = (char*)(img);
			strncpy(replyCode , pos + 9 , 3 );
			if(strcmp(replyCode , "404") == 0){
				fclose(f);
				f = NULL;
				goto end;
			}
			imageLength = LOF_TOOL_http_connection_get_body_length(pos);
			receivedLength = chunkLength - LOF_TOOL_http_connection_get_head_length(pos) - 4;
			for(i = 0 ; i < chunkLength ; i ++)
				if( img[i] == '\r' && img[i+1] == '\n'
					&&img[i+2] == '\r' && img[i+3] == '\n' )
				{
					fwrite(img + i +4 , chunkLength - i - 4 , 1 ,f);
					break;
				}
			if(receivedLength == imageLength)
			{
				fclose(f);
				f = NULL;
				goto end;
			}
		}
		else
		{
			fwrite(img , chunkLength , 1 , f);
			receivedLength += chunkLength;
			if(receivedLength == imageLength)
				break;
		}
		memset(img , 0 , sizeof(img));
	}
	if(f != NULL)
		fclose(f);
end:
LOF_CONNECTION_FetionConnection_free(tcp);
	tcp = NULL;
	return 0;
}
LOF_DATA_BuddyContactType* LOF_SIP_parse_presence_body(const char* body , LOF_DATA_LocalUserType* user)
{
	xmlDocPtr doc;
	xmlNodePtr node , cnode;
	xmlChar* pos;
	LOF_DATA_BuddyContactType* contact;
	LOF_DATA_BuddyContactType* contactres;
	LOF_DATA_BuddyContactType* contactlist = user->contactList;
	LOF_DATA_BuddyContactType* currentContact;

	contactres = LOF_DATA_BuddyContact_new();

	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "c");
	while(node != NULL)
	{
		pos = xmlGetProp(node , BAD_CAST "id");
		currentContact = LOF_DATA_BuddyContact_list_find_by_userid(contactlist , (char*)pos);
		if(currentContact == NULL)
		{
			/*not a valid information*/
			/*debug_error("User %s is not a valid user" , (char*)pos);*/
			node = node->next;
			continue;
		}
		cnode = node->xmlChildrenNode;
		if(xmlHasProp(cnode , BAD_CAST "sid"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "sid");
			strcpy(currentContact->sId ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "m"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "m");
			strcpy(currentContact->mobileno ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "l"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "l");
			currentContact->scoreLevel = atoi((char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "n"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "n");
			strcpy(currentContact->nickname ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "i"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "i");
			strcpy(currentContact->impression ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "p"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "p");
			if(strcmp(currentContact->portraitCrc, (char*)pos) == 0
					|| strcmp((char*)pos, "0") == 0)
				currentContact->imageChanged = 0;
			else
				currentContact->imageChanged = 1;
			strcpy(currentContact->portraitCrc ,  (char*)pos);
			xmlFree(pos);
		}else{
			currentContact->imageChanged = 0;
		}

		if(xmlHasProp(cnode , BAD_CAST "c"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "c");
			strcpy(currentContact->carrier , (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "cs"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "cs");
			currentContact->carrierStatus = atoi((char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "s"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "s");
			currentContact->serviceStatus = atoi((char*)pos);
			xmlFree(pos);
		}
#if 0
		if(xmlHasProp(cnode , BAD_CAST "sms")){
			pos = xmlGetProp(cnode , BAD_CAST "sms");
			xmlFree(pos);
		}
#endif
		cnode = LOF_TOOL_xml_goto_node(node , "pr");
		if(xmlHasProp(cnode , BAD_CAST "dt"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "dt");
			strcpy(currentContact->devicetype ,  *((char*)pos) == '\0' ? "PC" : (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(cnode , BAD_CAST "b"))
		{
			pos = xmlGetProp(cnode , BAD_CAST "b");
			currentContact->status = atoi((char*)pos);
			xmlFree(pos);
		}
		contact = LOF_DATA_BuddyContact_new();
		memset(contact , 0 , sizeof(contact));
		memcpy(contact , currentContact , sizeof(LOF_DATA_BuddyContactType));
		LOF_DATA_BuddyContact_list_append(contactres , contact);
		LOF_debug_info("BuddyContact Updated For %s.",contact->nickname);
		node = node->next;
	}
	xmlFreeDoc(doc);
	return contactres;
}
LOF_DATA_BuddyContactType* LOF_SIP_parse_syncuserinfo_body(const char* body , LOF_DATA_LocalUserType* user)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar* pos;
	LOF_DATA_BuddyContactType* contactlist = user->contactList;
	LOF_DATA_BuddyContactType* currentContact = NULL;

	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "buddy");
	if(node == NULL)
		return NULL;
	while(node){
		if(xmlHasProp(node , BAD_CAST "action")){
			pos = xmlGetProp(node , BAD_CAST "action");
			if(xmlStrcmp(pos , BAD_CAST "add") != 0){
				xmlFree(pos);
				node = node->next;
				continue;
			}
			xmlFree(pos);
		}
		
		pos = xmlGetProp(node , BAD_CAST "user-id");
		currentContact = LOF_DATA_BuddyContact_list_find_by_userid(contactlist , (char*)pos);
		//currentContact = fetion_contact_new();
		LOF_debug_info("synchronize user information");
		if(currentContact == NULL)
		{
			/*not a valid information*/
			LOF_debug_error("User %s is not a valid user" , (char*)pos);
			return NULL;
		}
		if(xmlHasProp(node , BAD_CAST "uri"))
		{
			pos = xmlGetProp(node , BAD_CAST "uri");
			strcpy(currentContact->sipuri ,  (char*)pos);
			xmlFree(pos);
		}
		if(xmlHasProp(node , BAD_CAST "relation-status"))
		{
			pos = xmlGetProp(node , BAD_CAST "relation-status");
			currentContact->relationStatus = atoi((char*)pos);
			if(atoi((char*)pos) == 1){
				LOF_debug_info("User %s accepted your request" , currentContact->userId);
			}else{
				LOF_debug_info("User %s refused your request" , currentContact->userId);
			}
			xmlFree(pos);
		}
		xmlFreeDoc(doc);
		return currentContact;
		node = node->next;
	}
	xmlFreeDoc(doc);
	return currentContact;
}

void LOF_DATA_LocalUser_save(LOF_DATA_LocalUserType *user)
{
	char path[256];
	char sql[4096];
	char password[4096];
	char impression[4096];
	sqlite3 *db;
	LOF_TOOL_ConfigType *config = user->config;

	sprintf(path, "%s/data.db",
				   	config->userPath);

	LOF_debug_info("Save user information");
	if(sqlite3_open(path, &db)){
		LOF_debug_error("open data.db:%s",
					sqlite3_errmsg(db));
		return;
	}

	sprintf(sql, "delete from user;");
	if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
		sprintf(sql, "create table user ("
					"sId,userId,mobileno,password,sipuri,"
					"publicIp,lastLoginIp,lastLoginTime,"
					"personalVersion, contactVersion,"
					"nickname,impression,country,province,"
					"city,gender,smsOnLineStatus,"
					"customConfigVersion, customConfig,"
					"boundToMobile);");
		if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
			LOF_debug_error("create table user:%s", sqlite3_errmsg(db));
			sqlite3_close(db);
			return;
		}
	}
	sprintf(password, "%s", user->password);
	sprintf(impression, "%s", user->impression);
	LOF_TOOL_escape_sql(password);
	LOF_TOOL_escape_sql(impression);
	snprintf(sql, sizeof(sql)-1, "insert into user "
				"values ('%s','%s','%s','%s','%s',"
				"'%s','%s','%s','%s','%s',"
				"'%s','%s','%s','%s','%s',%d,'%s',"
				" '%s', '%s',%d);",
				user->sId, user->userId, user->mobileno,
				password, user->sipuri, user->publicIp,
				user->lastLoginIp, user->lastLoginTime,
				user->personalVersion, user->contactVersion,
				user->nickname, impression, user->country,
				user->province, user->city, user->gender,
				user->smsOnLineStatus, user->customConfigVersion,
				user->customConfig, user->boundToMobile);
	if(sqlite3_exec(db, sql, NULL, NULL, NULL))
		LOF_debug_error("update user:%s\n%s", sqlite3_errmsg(db), sql);

	sqlite3_close(db);
}

void LOF_DATA_LocalUser_load(LOF_DATA_LocalUserType *user)
{
	char path[256];
	char sql[4096];
	char **sqlres;
	sqlite3 *db;
	int ncols, nrows;
	LOF_TOOL_ConfigType *config = user->config;

	sprintf(path, "%s/data.db",config->userPath);

	LOF_debug_info("Load user information");
	if(sqlite3_open(path, &db)){
		LOF_debug_error("open data.db:%s", sqlite3_errmsg(db));
		return;
	}

	sprintf(sql, "select * from user;");
	if(sqlite3_get_table(db, sql, &sqlres, &nrows, &ncols, NULL)){
		sqlite3_close(db);
		return;
	}

	if(nrows == 0 || ncols == 0){
		sqlite3_close(db);
		return;
	}

	strcpy(user->sId , 				sqlres[ncols]);
	strcpy(user->userId , 			sqlres[ncols+1]);
	strcpy(user->mobileno , 		sqlres[ncols+2]);
	strcpy(user->password , 		sqlres[ncols+3]);
	strcpy(user->sipuri , 			sqlres[ncols+4]);
	strcpy(user->publicIp , 		sqlres[ncols+5]);
	strcpy(user->lastLoginIp , 		sqlres[ncols+6]);
	strcpy(user->lastLoginTime , 	sqlres[ncols+7]);
	strcpy(user->personalVersion , 	sqlres[ncols+8]);
	strcpy(user->contactVersion , 	sqlres[ncols+9]);
	strcpy(user->nickname , 		sqlres[ncols+10]);
	strcpy(user->impression , 		sqlres[ncols+11]);
	strcpy(user->country , 			sqlres[ncols+12]);
	strcpy(user->province , 		sqlres[ncols+13]);
	strcpy(user->city , 			sqlres[ncols+14]);
	user->gender = 					atoi(sqlres[ncols+15]);
	strcpy(user->smsOnLineStatus ,  sqlres[ncols+16]);
	strcpy(user->customConfigVersion ,  sqlres[ncols+17]);
	user->customConfig = (char*)malloc(strlen(sqlres[ncols+18])+1);
	memset(user->customConfig, 0, strlen(sqlres[ncols+18])+1);
	strcpy(user->customConfig, sqlres[ncols+18]);
	user->boundToMobile = atoi(sqlres[ncols+19]);

	LOF_TOOL_unescape_sql(user->password);
	LOF_TOOL_unescape_sql(user->impression);
	
	sqlite3_free_table(sqlres);
	sqlite3_close(db);
}

char* LOF_SIP_generate_set_status_body(LOF_USER_StatusType state)
{
	char s[16];
	char data[] = "<args></args>";
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(data , strlen(data));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "presence" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "basic" , NULL);
	snprintf(s, sizeof(s) - 1 , "%d" , state);
	xmlNewProp(node , BAD_CAST "value" , BAD_CAST s);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}
char* LOF_SIP_generate_set_moodphrase_body(const char* customConfigVersion
		, const char* customConfig , const char* personalVersion
		,  const char* moodphrase)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node , cnode;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "userinfo" , NULL);
	cnode = xmlNewChild(node , NULL , BAD_CAST "personal" , NULL);
	xmlNewProp(cnode , BAD_CAST "impresa" , BAD_CAST moodphrase);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST personalVersion);
	cnode = xmlNewChild(node , NULL , BAD_CAST "custom-config" , BAD_CAST customConfig);
	xmlNewProp(cnode , BAD_CAST "type" , BAD_CAST "PC");
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST customConfigVersion);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}
char* LOF_SIP_generate_update_information_body(LOF_DATA_LocalUserType* user)
{
	char args[] = "<args></args>";
	char gender[5];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node , cnode;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "userinfo" , NULL);
	cnode = xmlNewChild(node , NULL , BAD_CAST "personal" , NULL);
	xmlNewProp(cnode , BAD_CAST "impresa" , BAD_CAST user->impression);
	xmlNewProp(cnode , BAD_CAST "nickname" , BAD_CAST user->nickname);
	sprintf(gender , "%d" , user->gender);
	xmlNewProp(cnode , BAD_CAST "gender" , BAD_CAST gender);
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST "0");
	cnode = xmlNewChild(node , NULL , BAD_CAST "custom-config" , BAD_CAST user->customConfig);
	xmlNewProp(cnode , BAD_CAST "type" , BAD_CAST "PC");
	xmlNewProp(cnode , BAD_CAST "version" , BAD_CAST user->customConfigVersion);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}
char* LOF_SIP_generate_keep_alive_body()
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "credentials" , NULL);
	xmlNewProp(node , BAD_CAST "domains" , BAD_CAST "fetion.com.cn");
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}
void LOF_SIP_parse_set_moodphrase_response(LOF_DATA_LocalUserType* user , const char* sipmsg)
{
	char *pos;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "version");
	memset(user->personalVersion, 0, sizeof(user->personalVersion));
	strcpy(user->personalVersion , (char*)res);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "impresa");
	memset(user->impression, 0, sizeof(user->impression));
	strcpy(user->impression , (char*)res);
	xmlFree(res);
	node = node->next;
	res = xmlGetProp(node , BAD_CAST "version");
	memset(user->customConfigVersion, 0, sizeof(user->customConfigVersion));
	strcpy(user->customConfigVersion , (char*)res);	
	xmlFree(res);
	res = xmlNodeGetContent(node);
	free(user->customConfig);
	user->customConfig = (char*)malloc(strlen((char*)res) + 1);
	memset(user->customConfig, 0, strlen((char*)res) + 1);
	strcpy(user->customConfig , (char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
}

char* LOF_SIP_generate_set_sms_status_body(int days)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	char status[16];

	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "userinfo" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "personal" , NULL);
	sprintf(status , "%d.00:00:00" , days);
	xmlNewProp(node , BAD_CAST "sms-online-status" , BAD_CAST status);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}

void LOF_SIP_parse_set_sms_status_response(LOF_DATA_LocalUserType *user , const char *sipmsg)
{
	char *pos;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "personal");
	if(!node)
		return;
	if(!xmlHasProp(node , BAD_CAST "sms-online-status"))
		return;
	res = xmlGetProp(node , BAD_CAST "sms-online-status");
	strcpy(user->smsOnLineStatus , (char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
}
