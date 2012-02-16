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
/*private */
/*FUCK YOU PRIVATE*/
/*char* generate_subscribe_body(const char* version);
char* generate_contact_info_body(const char* userid);
char* generate_contact_info_by_no_body(const char* no , NumberType nt);
char* generate_set_mobileno_perssion(const char* userid , int show);
char* generate_set_displayname_body(const char* userid , const char* name);
char* generate_move_to_group_body(const char *userid, const char *groupids);
char* generate_delete_buddy_body(const char* userid);
char* generate_add_buddy_body(const char* no , NumberType notype
								, int buddylist , const char* localname
								, const char* desc , int phraseid);
static char* generate_handle_contact_request_body(const char* sipuri
								, const char* userid , const char* localname
								, int buddylist , int result );
static Contact* parse_handle_contact_request_response(const char* sipmsg);
static Contact* parse_add_buddy_response(const char* sipmsg , int* statuscode);
static int parse_set_mobileno_permission_response(User* user , const char* sipmsg);
static Contact* parse_contact_info_by_no_response(const char* sipmsg);
static int has_special_word(const char *in);
static char *generate_group_body(const char *userid, const char *buddylist);*/

int LOF_SIP_parse_set_mobileno_permission_response(LOF_DATA_LocalUserType* user , const char* sipmsg)
{
	char *pos;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "contact-list-version");
	memset(user->contactVersion, 0, sizeof(user->contactVersion));
	strcpy(user->contactVersion , (char*)res);
	xmlFree(res);
	xmlFreeDoc(doc);
	return 0;
}
int LOF_TOOL_has_special_word(const char *in)
{
	int i = 0;
	int inlength=(int)strlen(in);
	for(;i< inlength; i++){
		if(in[i] == '\'')
			return 1;
	}
	return 0;
}

LOF_DATA_BuddyContactType* LOF_DATA_BuddyContact_new()
{
	LOF_DATA_BuddyContactType* list = (LOF_DATA_BuddyContactType*)malloc(sizeof(LOF_DATA_BuddyContactType));
	if(list == NULL){
		return NULL;
	}
	memset(list , 0 , sizeof(LOF_DATA_BuddyContactType));
	list->imageChanged = LOF_AVATAR_NOT_INITIALIZED;
	list->status = LOF_STATUS_HIDDEN;
	list->pre = list;
	list->next = list;
	return list;
}

void LOF_DATA_BuddyContact_list_append(LOF_DATA_BuddyContactType* cl , LOF_DATA_BuddyContactType* contact)
{
	cl->next->pre = contact;
	contact->next = cl->next;
	contact->pre = cl;
	cl->next = contact;
}

LOF_DATA_BuddyContactType* LOF_DATA_BuddyContact_list_find_by_userid(LOF_DATA_BuddyContactType* contactlist , const char* userid)
{
	LOF_DATA_BuddyContactType* cl_cur;
	foreach_contactlist(contactlist , cl_cur){
		if(strcmp(cl_cur->userId , userid) == 0)
			return cl_cur;
	}
	return NULL;
}
LOF_DATA_BuddyContactType* LOF_DATA_BuddyContact_list_find_by_sid(LOF_DATA_BuddyContactType* contactlist , const char* sid)
{
	LOF_DATA_BuddyContactType* cl_cur;
//	LOF_debug_info("Doing Find By Sid, which is %s.",sid);
	foreach_contactlist(contactlist , cl_cur){
		if(strcmp(cl_cur->sId , sid) == 0)
			return cl_cur;
	}
	LOF_debug_error("Contact Not Found.");
	return NULL;
}
LOF_DATA_BuddyContactType* LOF_DATA_BuddyContact_list_find_by_sipuri(LOF_DATA_BuddyContactType* contactlist , const char* sipuri)
{
	LOF_DATA_BuddyContactType *cl_cur;
	char *sid , *sid1;
	foreach_contactlist(contactlist , cl_cur){
		sid = LOF_SIP_get_sid_by_sipuri(cl_cur->sipuri);
		sid1 = LOF_SIP_get_sid_by_sipuri(sipuri);
		if(strcmp(sid , sid1) == 0){
			free(sid);
			free(sid1);
			return cl_cur;
		}
		free(sid);
		free(sid1);
	}
	return NULL;
}

LOF_DATA_BuddyContactType *LOF_DATA_BuddyContact_list_find_by_mobileno(LOF_DATA_BuddyContactType *contactlist, const char *mobileno)
{
	LOF_DATA_BuddyContactType *cl_cur;
	foreach_contactlist(contactlist, cl_cur) {
		if(strcmp(cl_cur->mobileno, mobileno) == 0)
			return cl_cur;
	}
	return NULL;
}

void LOF_DATA_BuddyContact_list_remove_by_userid(LOF_DATA_BuddyContactType* contactlist , const char* userid)
{
	LOF_DATA_BuddyContactType *cl_cur;
	foreach_contactlist(contactlist , cl_cur){
		if(strcmp(cl_cur->userId , userid) == 0){
			cl_cur->pre->next = cl_cur->next;
			cl_cur->next->pre = cl_cur->pre;
			free(cl_cur);
			break;
		}
	}
}

void LOF_DATA_BuddyContact_list_remove(LOF_DATA_BuddyContactType *contact)
{
	contact->next->pre = contact->pre;
	contact->pre->next = contact->next;
}

void LOF_DATA_BuddyContact_list_free(LOF_DATA_BuddyContactType* contact)
{
	LOF_DATA_BuddyContactType *cl_cur , *del_cur;
	for(cl_cur = contact->next ; cl_cur != contact ;){
		cl_cur->pre->next = cl_cur->next;
		cl_cur->next->pre = cl_cur->pre;
		del_cur = cl_cur;
		cl_cur = cl_cur->next;
		free(del_cur);
	}
	free(contact);
}

int LOF_DATA_BuddyContact_subscribe_only(LOF_DATA_LocalUserType* user)
{
	char *res, *body;
	LOF_SIP_FetionSipType* sip;
	LOF_SIP_SipHeaderType* eheader;

	sip = user->sip;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SUBSCRIPTION);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_PRESENCE);
	if(eheader == NULL){
		return -1;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_subscribe_body("0");
	if(body == NULL){
		free(eheader);
		return -1;
	}
	res = LOF_SIP_to_string(sip , body);
	if(res == NULL){
		free(eheader);
		free(body);
		return -1;
	}
	free(body);
	LOF_debug_info("Start subscribe contact list");
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	return 0;
}

LOF_DATA_BuddyContactType* LOF_DATA_BuddyContact_get_contact_info(LOF_DATA_LocalUserType* user , const char* userid)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	char *res , *body , *pos;
	char *cur;
	LOF_DATA_BuddyContactType* contact;
	xmlChar* cs;
	xmlDocPtr doc;
	xmlNodePtr node;
	contact = LOF_DATA_BuddyContact_list_find_by_userid(user->contactList , userid);
	body = LOF_SIP_generate_contact_info_body(userid);
	if(body == NULL) return NULL;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	LOF_SIP_SipHeaderType* eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_GETCONTACTINFO);
	if(eheader == NULL){
		free(body);
		return NULL;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(res == NULL){	
		free(res);
		return NULL;
	}
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	res = LOF_SIP_get_response(sip);
	if(res == NULL){
		return NULL;
	}

	pos = strstr(res , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	if(!doc){
		return NULL;
	}
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	if(xmlHasProp(node , BAD_CAST "carrier-region")){
		cs = xmlGetProp(node , BAD_CAST "carrier-region");
		pos = (char*)cs;

		for(cur = contact->country;*pos && *pos != '.';*cur ++ = *pos ++);
		*cur = '\0'; pos ++;
		for(cur = contact->province;*pos && *pos != '.';*cur ++ = *pos ++);
		*cur = '\0'; pos ++;
		for(cur = contact->city;*pos && *pos != '.';*cur ++ = *pos ++);
		*cur = '\0';
		xmlFree(cs);
		free(res);
	}
	return contact;
}

int LOF_DATA_BuddyContact_has_ungrouped(LOF_DATA_BuddyContactType *contactlist)
{
	LOF_DATA_BuddyContactType *cur;

	foreach_contactlist(contactlist , cur){
		if(cur->groupid == LOF_BUDDY_LIST_NOT_GROUPED)
		    return 1;
	}
	return 0;

}

int LOF_DATA_BuddyContact_has_strangers(LOF_DATA_BuddyContactType *contactlist)
{
	LOF_DATA_BuddyContactType *cur;

	foreach_contactlist(contactlist , cur){
		if(cur->groupid == LOF_BUDDY_LIST_STRANGER)
		    return 1;
	}
	return 0;

}

LOF_DATA_BuddyContactType* LOF_DATA_BuddyContact_get_contact_info_by_no(LOF_DATA_LocalUserType* user , const char* no , LOF_TOOL_NumberType nt)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	LOF_DATA_BuddyContactType* contact;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_GETCONTACTINFO);
	if(eheader == NULL){
		return NULL;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_contact_info_by_no_body(no , nt);
	if(body == NULL){
		return NULL;
	}
	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(res == NULL){
		return NULL;
	}
	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res); res = NULL; 
	if(ret < 0)
		return NULL;

/*	res = LOF_SIP_get_response(sip);
	if(res == NULL){
		return NULL;
	}
	ret = LOF_SIP_get_code(res);
	
	if(ret == 200){
 		contact = LOF_SIP_parse_contact_info_by_no_response(res);
		free(res);
		LOF_debug_info("Get user information by mobile number success");
		return contact;
	}else{
		free(res);
		LOF_debug_error("Get user information by mobile number failed , errno :" , ret);
		return NULL;
	}*/
	return NULL;
}
int LOF_DATA_BuddyContact_set_mobileno_permission(LOF_DATA_LocalUserType* user , const char* userid , int show)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETCONTACTINFO);
	if(eheader == NULL)	return -1;
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_set_mobileno_perssion(userid , show);
	if(body == NULL) return -1;
	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(res == NULL)	return -1;
	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res) ; 
	if(ret < 0)	return -1;

	res = LOF_SIP_get_response(sip);
	if(res == NULL)	return -1;
	ret = LOF_SIP_get_code(res);
	if(ret == 200){
		LOF_SIP_parse_set_mobileno_permission_response(user , res);
		free(res);
		LOF_debug_info("Get user information by mobile number success");
		return 0;
	}else{
		free(res);
		LOF_debug_error("Get user information by mobile number failed , errno :" , ret);
		return -1;
 	}
}
int LOF_DATA_BuddyContact_set_displayname(LOF_DATA_LocalUserType* user , const char* userid , const char* name)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETCONTACTINFO);
	if(eheader == NULL){
		return -1;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_set_displayname_body(userid , name);
	if(body == NULL){
		return -1;
	}
	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(res == NULL){
		return -1;
	}
	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	if(ret < 0)
		return -1;
	res = LOF_SIP_get_response(sip);
	if(res == NULL){
		return -1;
	}
	ret = LOF_SIP_get_code(res);
	free(res);

	if(ret == 200){
		LOF_debug_info("Set buddy(%s)`s localname to %s success" , userid , name);
		return 0;
	}else{
		LOF_debug_info("Set buddy(%s)`s localname to %s failed" , userid , name);
		return -1;
	}
}

int LOF_DATA_BuddyContact_move_to_group(LOF_DATA_LocalUserType *user, const char *userid, int old_bl, int new_bl)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *eheader;
	LOF_DATA_BuddyContactType    *cnt;
	char        bls[1024] = { 0, };
	char *res, *body;
	int ret;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETCONTACTINFO);
	LOF_SIP_FetionSip_add_header(sip , eheader);

	cnt = LOF_DATA_BuddyContact_list_find_by_userid(user->contactList, userid);
	foreach_groupids(cnt->groupids) {
		if(group_id == old_bl) continue;
		sprintf(bls + strlen(bls), "%d;", group_id);
	} end_groupids(cnt->groupids)
	sprintf(bls + strlen(bls), "%d", new_bl);
	sprintf(cnt->groupids, "%s", bls);

	body = LOF_SIP_generate_move_to_group_body(userid, bls);

	if(!body) return -1;
	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(!res) return -1;
	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);

	if(ret < 0)	return -1;

	return 0;
}

int LOF_DATA_BuddyContact_copy_to_group(LOF_DATA_LocalUserType *user, const char *userid, int buddylist)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *eheader;
	LOF_DATA_BuddyContactType   *cnt;
	char *res, *body, newbl[48] = { 0,};
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	if (!(eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETCONTACTINFO))) return -1;
	LOF_SIP_FetionSip_add_header(sip , eheader);
	
	cnt = LOF_DATA_BuddyContact_list_find_by_userid(user->contactList, userid);
	
	foreach_groupids(cnt->groupids) {
		if(group_id == buddylist) return -1;
		sprintf(newbl + strlen(newbl), "%d;", group_id);
	} end_groupids(cnt->groupids)
	sprintf(newbl + strlen(newbl), "%d", buddylist);
	sprintf(cnt->groupids, "%s", newbl);
	 
	body = LOF_SIP_generate_group_body(userid, newbl);
	res = LOF_SIP_to_string(sip, body);
	free(body);
	if(!res) return -1;

	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);

	if(ret < 0)	return -1;

	res = LOF_SIP_get_response(sip);
	if(!res) return -1;

	ret = LOF_SIP_get_code(res);
	free(res);

	return 0;
}

int LOF_DATA_BuddyContact_remove_from_group(LOF_DATA_LocalUserType *user, const char *userid, int buddylist)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *eheader;
	LOF_DATA_BuddyContactType   *cnt;
	char *res, *body, newbl[48] = { 0,};
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	if (!(eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETCONTACTINFO))) return -1;
	LOF_SIP_FetionSip_add_header(sip , eheader);
	
	cnt = LOF_DATA_BuddyContact_list_find_by_userid(user->contactList, userid);
	
	foreach_groupids(cnt->groupids) {
		if(group_id == buddylist) continue;
		sprintf(newbl + strlen(newbl), "%d;", group_id);
	} end_groupids(cnt->groupids)
	if(newbl[strlen(newbl) - 1] == ';') newbl[strlen(newbl) - 1] = '\0';
	sprintf(cnt->groupids, "%s", newbl);
	 
	body = LOF_SIP_generate_group_body(userid, newbl);
	res = LOF_SIP_to_string(sip, body);
	free(body);
	if(!res) return -1;

	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);

	if(ret < 0)	return -1;

	res = LOF_SIP_get_response(sip);
	if(!res) return -1;

	ret = LOF_SIP_get_code(res);
	free(res);

	return 0;
}

int LOF_DATA_BuddyContact_delete_buddy(LOF_DATA_LocalUserType* user , const char* userid)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_DELETEBUDDY);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_delete_buddy_body(userid);
	if(body == NULL) return -1;
	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(res == NULL)	return -1;
#if 0
	if(LOF_BUDDY_del_localbuddy(user, userid) == -1)
		return -1;
#endif

	ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);

	if(ret < 0)	return -1;

	res = LOF_SIP_get_response(sip);
	if(res == NULL)	return -1;
	ret = LOF_SIP_get_code(res);
	free(res);

	return 0;
}
LOF_DATA_BuddyContactType* LOF_DATA_BuddyContact_add_buddy(LOF_DATA_LocalUserType* user , const char* no
								, LOF_TOOL_NumberType notype , int buddylist
								, const char* localname , const char* desc
								, int phraseid , int* statuscode)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader = NULL;
	LOF_SIP_SipHeaderType* ackheader = NULL;
	char *res = NULL;
	char *body = NULL;
	int ret;
	LOF_DATA_BuddyContactType* contact;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_ADDBUDDY);
	if(eheader == NULL){
		return NULL;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	if(user->verification != NULL && user->verification->algorithm != NULL)	
	{
		ackheader = LOF_SIP_SipHeader_ack_new(user->verification->code
											, user->verification->algorithm
											, user->verification->type
											, user->verification->guid);
		if(ackheader == NULL){
			return NULL;
		}
		LOF_SIP_FetionSip_add_header(sip , ackheader);
	}
	body = LOF_SIP_generate_add_buddy_body(no , notype , buddylist , localname , desc , phraseid);
	if(body == NULL){
		return NULL;
	}

	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(res == NULL){
		return NULL;
	}
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	res = LOF_SIP_get_response(sip);
	if(res == NULL){
		return NULL;
	}
	ret = LOF_SIP_get_code(res);
	*statuscode = ret;
	int rtv;
	switch(ret)
	{
		case 200 :
			contact = LOF_SIP_parse_add_buddy_response(res , statuscode);
			LOF_DATA_Verification_free(user->verification);
			user->verification = NULL;
			free(res);
			if(contact == NULL){
				LOF_debug_info("Add buddy(%s) failed" , no);
				return NULL;
			}
			LOF_DATA_BuddyContact_list_append(user->contactList , contact);
			LOF_debug_info("Add buddy(%s) success" , no);
			return contact;
		case 421 : 
		case 420 :
			rtv = LOF_SIP_parse_add_buddy_verification(user , res);
			free(res);
			if(rtv != 0){
				LOF_debug_info("Add buddy(%s) falied , need verification, but parse error" , no);
				return NULL;
			}
			LOF_debug_info("Add buddy(%s) falied , need verification" , no);
			return NULL;
		default:
			free(res);
			LOF_debug_info("Add buddy(%s) failed" , no);
			return NULL;
	}
}

LOF_DATA_BuddyContactType* LOF_SIP_handle_contact_request(LOF_DATA_LocalUserType* user
		, const char* sipuri , const char* userid
		, const char* localname , int buddylist , int result)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_DATA_BuddyContactType* contact;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_HANDLECONTACTREQUEST);
	if(eheader == NULL){
		return NULL;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_handle_contact_request_body(sipuri , userid , localname , buddylist , result);
	if(body == NULL){
		return NULL;
	}
	res = LOF_SIP_to_string(sip , body);
	free(body);
	if(res == NULL){
		return NULL;
	}
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	res = LOF_SIP_get_response(sip);
	if(res == NULL){
		return NULL;
	}
	ret = LOF_SIP_get_code(res);
	switch(ret)
	{
		case 200 :
			contact = LOF_SIP_parse_handle_contact_request_response(res);
			free(res);
			if(contact == NULL){
				LOF_debug_info("handle contact request from (%s) failed" , userid);
				return NULL;
			}
			LOF_DATA_BuddyContact_list_append(user->contactList , contact);
			LOF_debug_info("handle contact request from (%s) success" , userid);
			return contact;
		default:
			free(res);
			LOF_debug_info("handle contact request from (%s) failed" , userid);
			return NULL;
	}
	return NULL;
}

char* LOF_SIP_generate_subscribe_body(const char* version)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "subscription" , NULL);
	xmlNewProp(node , BAD_CAST "self" , BAD_CAST "v4default;mail-count");
	xmlNewProp(node , BAD_CAST "buddy" , BAD_CAST "v4default");
	xmlNewProp(node , BAD_CAST "version" , BAD_CAST version);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}
char* LOF_SIP_generate_contact_info_body(const char* userid)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
	
}
char* LOF_SIP_generate_contact_info_by_no_body(const char* no , LOF_TOOL_NumberType nt)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char uri[32];
	char body[] = "<args></args>";
	if(nt == LOF_MOBILE_NO)
		sprintf(uri , "tel:%s" , no);
	else
		sprintf(uri , "sip:%s" , no);
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST uri);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}
char* LOF_SIP_generate_set_mobileno_perssion(const char* userid , int show)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char permission[32];
	char body[] = "<args></args>";
	sprintf(permission , "identity=%d" , show);
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "permission" , BAD_CAST permission);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}
char* LOF_SIP_generate_handle_contact_request_body(const char* sipuri
		, const char* userid , const char* localname
		, int buddylist , int result )
{
	char args[] = "<args></args>";
	char result_s[4];
	char buddylist_s[4];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddies" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST sipuri);
	sprintf(result_s , "%d" , result);
	sprintf(buddylist_s , "%d" , buddylist);
	xmlNewProp(node , BAD_CAST "result" , BAD_CAST result_s);
	xmlNewProp(node , BAD_CAST "buddy-lists" , BAD_CAST buddylist_s);
	xmlNewProp(node , BAD_CAST "expose-mobile-no" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "expose-name" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "local-name" , BAD_CAST localname);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}
char* LOF_SIP_generate_set_displayname_body(const char* userid , const char* name)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "local-name" , BAD_CAST name);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}

char* LOF_SIP_generate_move_to_group_body(const char *userid, const char *groupids)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "buddy-lists" , BAD_CAST groupids);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}

char* LOF_SIP_generate_delete_buddy_body(const char* userid)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddies" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy" , NULL);
	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}

char* LOF_SIP_generate_add_buddy_body(const char* no
		, LOF_TOOL_NumberType notype , int buddylist
		, const char* localname , const char* desc , int phraseid)
{
	char args[] = "<args></args>";
	char uri[48];
	char phrase[4];
	char groupid[4];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddies" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy" , NULL);

	if(notype == LOF_FETION_NO)
		sprintf(uri , "sip:%s" , no);
	else
		sprintf(uri , "tel:%s" , no);

	sprintf(phrase , "%d" , phraseid);
	sprintf(groupid , "%d" , buddylist);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST uri);
	xmlNewProp(node , BAD_CAST "local-name" , BAD_CAST localname);
	xmlNewProp(node , BAD_CAST "buddy-lists" , BAD_CAST groupid);
	xmlNewProp(node , BAD_CAST "desc" , BAD_CAST desc);
	xmlNewProp(node , BAD_CAST "expose-mobile-no" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "expose-name" , BAD_CAST "1");
	xmlNewProp(node , BAD_CAST "addbuddy-phrase-id" , BAD_CAST phrase);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}

char* LOF_SIP_generate_group_body(const char *userid, const char *buddylist)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);

	xmlNewProp(node , BAD_CAST "user-id" , BAD_CAST userid);
	xmlNewProp(node , BAD_CAST "buddy-lists" , BAD_CAST buddylist);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}


LOF_DATA_BuddyContactType* LOF_SIP_parse_contact_info_by_no_response(const char* sipmsg)
{
	char *pos;
	LOF_DATA_BuddyContactType* contact;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	contact = LOF_DATA_BuddyContact_new();
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	if(xmlHasProp(node , BAD_CAST "uri"))
	{
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(contact->sipuri , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "user-id"))
	{
		res = xmlGetProp(node , BAD_CAST "user-id");
		strcpy(contact->userId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "sid"))
	{
		res = xmlGetProp(node , BAD_CAST "sid");
		strcpy(contact->sId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "nickname"))
	{
		res = xmlGetProp(node , BAD_CAST "nickname");
		strcpy(contact->nickname , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "gender"))
	{
		res = xmlGetProp(node , BAD_CAST "gender");
		contact->gender = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "birth-date"))
	{
		res = xmlGetProp(node , BAD_CAST "birth-date");
		strcpy(contact->birthday , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "impresa"))
	{
		res = xmlGetProp(node , BAD_CAST "impresa");
		strcpy(contact->impression , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "mobile-no"))
	{
		res = xmlGetProp(node , BAD_CAST "mobile-no");
		strcpy(contact->mobileno , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "carrier-region"))
	{
		int n;
		res = xmlGetProp(node , BAD_CAST "carrier-region");
		pos = (char*)res;
                if (!(pos == NULL || strcmp(pos, "") == 0)) {
                    n = strlen(pos) - strlen(strstr(pos , "."));
                    strncpy(contact->country , pos , n);
                    pos = strstr(pos , ".") + 1;
                    n = strlen(pos) - strlen(strstr(pos , "."));
                    strncpy(contact->province , pos , n);
                    pos = strstr(pos , ".") + 1;
                    n = strlen(pos) - strlen(strstr(pos , "."));
                    strncpy(contact->city , pos , n);
                }
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "portrait-crc"))
	{
		res = xmlGetProp(node , BAD_CAST "portrait-crc");
		strcpy(contact->portraitCrc , (char*)res);
		xmlFree(res);
	}
	xmlFreeDoc(doc);
	return contact;
}
LOF_DATA_BuddyContactType* LOF_SIP_parse_add_buddy_response(const char* sipmsg , int* statuscode)
{
	char *pos;
	LOF_DATA_BuddyContactType* contact;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	contact = LOF_DATA_BuddyContact_new();
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "buddy");

	if(node == NULL)
	{
		*statuscode = 400;
		free(contact);
		return NULL;
	}
	if(xmlHasProp(node , BAD_CAST "uri"))
	{
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(contact->sipuri , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "user-id"))
	{
		res = xmlGetProp(node , BAD_CAST "user-id");
		strcpy(contact->userId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "mobile-no"))
	{
		res = xmlGetProp(node , BAD_CAST "mobile-no");
		strcpy(contact->mobileno , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "local-name"))
	{
		res = xmlGetProp(node , BAD_CAST "local-name");
		strcpy(contact->localname , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "buddy-lists"))
	{
		res = xmlGetProp(node , BAD_CAST "buddy-lists");
		contact->groupid = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "status-code"))
	{
		res = xmlGetProp(node , BAD_CAST "status-code");
		*statuscode = atoi((char*)res);
		xmlFree(res);
	}
	else
	{
		*statuscode = 200;
	}
	if(xmlHasProp(node , BAD_CAST "basic-service-status"))
	{
		res = xmlGetProp(node , BAD_CAST "basic-service-status");
		contact->serviceStatus = atoi((char*)res);
		xmlFree(res);
	}
	contact->relationStatus = LOF_CMCC_STATUS_NOT_AUTHENTICATED;
	xmlFreeDoc(doc);
	return contact;
}
LOF_DATA_BuddyContactType* LOF_SIP_parse_handle_contact_request_response(const char* sipmsg)
{
	char *pos = NULL;
	LOF_DATA_BuddyContactType* contact = NULL;
	xmlChar* res = NULL;
	xmlDocPtr doc;
	xmlNodePtr node;
	contact = LOF_DATA_BuddyContact_new();

	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "buddy");
	if(xmlHasProp(node , BAD_CAST "uri"))
	{
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(contact->sipuri , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "user-id"))
	{
		res = xmlGetProp(node , BAD_CAST "user-id");
		strcpy(contact->userId , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "local-name"))
	{
		res = xmlGetProp(node , BAD_CAST "local-name");
		strcpy(contact->localname , (char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "buddy-lists"))
	{
		res = xmlGetProp(node , BAD_CAST "buddy-lists");
		contact->groupid = atoi((char*)res);
		xmlFree(res);
	}
	if(xmlHasProp(node , BAD_CAST "relation-status")){
		res = xmlGetProp(node , BAD_CAST "relation-status");
		contact->relationStatus = atoi((char*)res);
		xmlFree(res);
	}else{
		contact->relationStatus = LOF_RELATION_STATUS_AUTHENTICATED;
	}
	xmlFreeDoc(doc);
	return contact;
}
int LOF_SIP_parse_add_buddy_verification(LOF_DATA_LocalUserType* user , const char* str)
{
	char* xml = NULL;
	char w[128];
	int n = 0;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res = NULL;
	LOF_DATA_VerificationType *ver = NULL;

	ver = (LOF_DATA_VerificationType*)malloc(sizeof(LOF_DATA_VerificationType));
	memset(ver , 0 , sizeof(sizeof(LOF_DATA_VerificationType)));

	memset(w, 0, sizeof(w));
	LOF_SIP_get_attr(str , "W" , w);
	xml = strstr(w , "algorithm=") + 11;
	n = strlen(xml) - strlen(strstr(xml , "\""));
	ver->algorithm = (char*)malloc(n + 1);
	memset(ver->algorithm, 0, n + 1);
	strncpy(ver->algorithm , xml , n);
	xml = strstr(w , "type=") + 6;
	n = strlen(xml) - strlen(strstr(xml , "\""));
	ver->type = (char*)malloc(n + 1);
	memset(ver->type, 0, n +1);
	strncpy(ver->type , xml , n);

	xml = strstr(str , "\r\n\r\n");
	doc = xmlParseMemory(xml , strlen(xml));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "text");
	n = xmlStrlen(res) + 1;
	ver->text = (char*)malloc(n);
	memset(ver->text, 0, n);
	strncpy(ver->text , (char*)res , n - 1);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "tips");
	n = xmlStrlen(res) + 1;
	ver->tips = (char*)malloc(n);
	memset(ver->tips, 0, n);
	strncpy(ver->tips , (char*)res , n - 1);
	xmlFree(res);
	user->verification = ver;
	return 0;
}

void LOF_DATA_BuddyContact_load(LOF_DATA_LocalUserType *user, int *gcount, int *bcount)
{
	char path[256];
	char sql[4096];
	sqlite3 *db;
	char **sqlres;
	int ncols, nrows, i, j, start;
	LOF_DATA_BuddyContactType *pos;
	LOF_DATA_BuddyGroupType *gpos;
	LOF_TOOL_ConfigType *config = user->config;

	LOF_debug_info("Load contact list");

	*gcount = 0;
	*bcount = 0;

	sprintf(path , "%s/data.db" , config->userPath);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("failed to load contact list");
		return;
	}

	sprintf(sql, "select * from groups order by groupid;");
	if(sqlite3_get_table(db, sql, &sqlres, &nrows, &ncols, NULL)){
		sqlite3_close(db);
		return;
	}

	*gcount = nrows;

	for(i = 0; i < nrows; i++){
		gpos = LOF_DATA_BuddyGroup_new();
		for(j = 0; j < ncols; j++){
			start = ncols + i * ncols;
			gpos->groupid = atoi(sqlres[start]);
			strcpy(gpos->groupname, sqlres[start+1]);
		}
		LOF_DATA_BuddyGroup_list_append(user->groupList, gpos);
	}
	sqlite3_free_table(sqlres);

	sprintf(sql, "select * from contacts_2_2_0;");
	if(sqlite3_get_table(db, sql, &sqlres, &nrows, &ncols, NULL)){
		sqlite3_close(db);
		return;
	}

	*bcount = nrows;
	
	for(i = 0; i < nrows; i++){
		pos = LOF_DATA_BuddyContact_new();
		for(j = 0; j < ncols; j++){
			start = ncols + i * ncols;
			strcpy(pos->userId, 	sqlres[start]);
			strcpy(pos->sId, 		sqlres[start+1]);
			strcpy(pos->sipuri, 	sqlres[start+2]);
			strcpy(pos->localname,  sqlres[start+3]);
			strcpy(pos->nickname, 	sqlres[start+4]);
			strcpy(pos->impression, sqlres[start+5]);
			strcpy(pos->mobileno, 	sqlres[start+6]);
			strcpy(pos->devicetype, sqlres[start+7]);
			strcpy(pos->portraitCrc,sqlres[start+8]);
			strcpy(pos->birthday, 	sqlres[start+9]);
			strcpy(pos->country, 	sqlres[start+10]);
			strcpy(pos->province, 	sqlres[start+11]);
			strcpy(pos->city, 		sqlres[start+12]);
			pos->identity = 		atoi(sqlres[start+13]);
			pos->scoreLevel = 		atoi(sqlres[start+14]);
			pos->serviceStatus = 	atoi(sqlres[start+15]);
			pos->carrierStatus = 	atoi(sqlres[start+16]);
			pos->relationStatus = 	atoi(sqlres[start+17]);
			strcpy(pos->carrier,	sqlres[start+18]);
			pos->groupid = 			atoi(sqlres[start+19]);
			pos->gender = 			atoi(sqlres[start+20]);
			strcpy(pos->groupids,   sqlres[start+21]);
		}
		LOF_DATA_BuddyContact_list_append(user->contactList, pos);
	}
	sqlite3_close(db);
	sqlite3_free_table(sqlres);
}

void LOF_DATA_BuddyContact_save(LOF_DATA_LocalUserType *user)
{
	char path[256];
	char sql[4096];
	sqlite3 *db;
	LOF_DATA_BuddyContactType *pos;
	LOF_DATA_BuddyGroupType *gpos;
	LOF_TOOL_ConfigType *config = user->config;

	LOF_debug_info("Save contact list");

	sprintf(path , "%s/data.db" , config->userPath);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("failed to save user list");
		return;
	}
	/* begin transaction */
	if(sqlite3_exec(db, "BEGIN TRANSACTION;", 0,0, NULL)){
		LOF_debug_error("begin transaction :%s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}

	sprintf(sql, "delete from groups");
	if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
		sprintf(sql, "create table groups (groupid,groupname)");
		if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
			LOF_debug_error("create table groups:%s", sqlite3_errmsg(db));
			sqlite3_close(db);
			return;
		}
	}
	foreach_grouplist(user->groupList, gpos){
		snprintf(sql, sizeof(sql)-1, "insert into groups "
				"values (%d,'%s');", gpos->groupid,
				gpos->groupname);
		if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
			LOF_debug_error("insert group info:%s", sqlite3_errmsg(db));
			continue;
		}
	}
	
	sprintf(sql, "delete from contacts_2_2_0;");
	if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
		sprintf(sql, "create table contacts_2_2_0 (userId,"
						"sId,sipuri,localname,nickname,"
						"impression,mobileno,devicetype,"
						"portraitCrc,birthday,country,"
						"province,city,identity,scoreLevel,"
						"serviceStatus,carrierStatus,"
						"relationStatus,carrier,groupid,gender,groupids);");
		if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
			LOF_debug_error("create table contacts:%s", sqlite3_errmsg(db));
			sqlite3_close(db);
			return;
		}
	}
	foreach_contactlist(user->contactList, pos){
		snprintf(sql, sizeof(sql)-1, "insert into contacts_2_2_0 "
				"values ('%s','%s','%s','%s','%s','%s',"
				"'%s','%s','%s','%s','%s','%s','%s','%d',%d,"
				"%d,%d,%d,'%s',%d,%d,'%s');",
				pos->userId, pos->sId, pos->sipuri,
				LOF_TOOL_has_special_word(pos->localname) ? "": pos->localname,
				LOF_TOOL_has_special_word(pos->nickname)? "": pos->nickname,
				LOF_TOOL_has_special_word(pos->impression)?"": pos->impression,
			   	pos->mobileno,
				pos->devicetype, pos->portraitCrc,
				pos->birthday, pos->country, pos->province,
				pos->city, pos->identity, pos->scoreLevel,
				pos->serviceStatus, pos->carrierStatus,
				pos->relationStatus, pos->carrier,
				pos->groupid, pos->gender, pos->groupids);
		if(sqlite3_exec(db, sql, NULL, NULL, NULL))
			LOF_debug_error("insert contact %s:%s\n%s",
					pos->userId, sqlite3_errmsg(db), sql);
	}
	/* begin transaction */
	if(sqlite3_exec(db, "END TRANSACTION;", 0,0, NULL)){
		LOF_debug_error("end transaction :%s", sqlite3_errmsg(db));
		sqlite3_close(db);
		return;
	}
	sqlite3_close(db);
	LOF_debug_info("Save contact list successfully");
}

void LOF_DATA_BuddyContact_update(LOF_DATA_LocalUserType *user, LOF_DATA_BuddyContactType *contact)
{
	char path[256];
	char sql[4096];
	sqlite3 *db;
	LOF_TOOL_ConfigType *config = user->config;

	LOF_debug_info("Update contact information");

	sprintf(path , "%s/data.db" , config->userPath);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("failed to load user list");
		return;
	}

	snprintf(sql, sizeof(sql)-1, "update contacts_2_2_0 set "
			"userId='%s',sId='%s',sipuri='%s',"
			"localname='%s',nickname='%s',"
			"impression='%s',mobileno='%s',"
			"devicetype='%s',portraitCrc='%s',"
			"birthday='%s',country='%s',"
			"province='%s',city='%s',"
			"identity=%d,scoreLevel=%d,"
			"serviceStatus=%d,carrierStatus=%d,"
			"relationStatus=%d,carrier='%s',"
			"groupid=%d,gender=%d,groupids='%s' where userId='%s'",
			contact->userId, contact->sId, contact->sipuri,
			contact->localname, contact->nickname,
			contact->impression, contact->mobileno,
			contact->devicetype, contact->portraitCrc,
			contact->birthday, contact->country, contact->province,
			contact->city, contact->identity, contact->scoreLevel,
			contact->serviceStatus, contact->carrierStatus,
			contact->relationStatus, contact->carrier,
			contact->groupid, contact->gender, contact->groupids, contact->userId);

	if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
		LOF_debug_error("update contact %s:%s", contact->userId, sqlite3_errmsg(db));
		sprintf(sql, "create table contacts_2_2_0 (userId,"
					"sId,sipuri,localname,nickname,"
					"impression,mobileno,devicetype,"
					"portraitCrc,birthday,country,"
					"province,city,identity,scoreLevel,"
					"serviceStatus,carrierStatus,"
					"relationStatus,carrier,groupid,gender,groupids);");
		if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
			LOF_debug_error("create table contacts:%s", sqlite3_errmsg(db));
			sqlite3_close(db);
			return;
		}

		snprintf(sql, sizeof(sql)-1, "insert into contacts_2_2_0 "
					"values ('%s','%s','%s','%s','%s','%s',"
					"'%s','%s','%s','%s','%s','%s','%s','%d',%d,"
					"%d,%d,%d,'%s',%d,%d,'%s');",
					contact->userId, contact->sId, contact->sipuri,
					contact->localname, contact->nickname,
					contact->impression, contact->mobileno,
					contact->devicetype, contact->portraitCrc,
					contact->birthday, contact->country, contact->province,
					contact->city, contact->identity, contact->scoreLevel,
					contact->serviceStatus, contact->carrierStatus,
					contact->relationStatus, contact->carrier,
					contact->groupid, contact->gender, contact->groupids);

		if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
			LOF_debug_error("insert contacts:%s", sqlite3_errmsg(db));
			sqlite3_close(db);
			return;
		}
	}
	sqlite3_close(db);
}

int LOF_DATA_BuddyContact_del_localbuddy(LOF_DATA_LocalUserType *user, const char *userid)
{
	char path[256];
	char sql[4096];
	sqlite3 *db;
	LOF_TOOL_ConfigType *config = user->config;

	sprintf(path , "%s/data.db" , config->userPath);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("failed to delete localbuddy");
		return -1;
	}

	sprintf(sql, "delete from contacts_2_2_0 where "
			"userid='%s';", userid);
	if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
		LOF_debug_error("failed to delete localbuddy:%s",sqlite3_errmsg(db));
		return -1;
	}
	return 0;
}

int LOF_DATA_BuddyContact_del_localgroup(LOF_DATA_LocalUserType *user, const char *groupid)
{
	char path[256];
	char sql[4096];
	sqlite3 *db;
	LOF_TOOL_ConfigType *config = user->config;

	sprintf(path , "%s/data.db" , config->userPath);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("failed to delete localgroup");
		return -1;
	}

	sprintf(sql, "delete from groups where "
			"id='%s';", groupid);
	if(sqlite3_exec(db, sql, NULL, NULL, NULL)){
		LOF_debug_error("failed to delete localgroup:%s",sqlite3_errmsg(db));
		return -1;
	}
	return 0;
}


