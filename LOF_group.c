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
/*
static char* generate_contact_info_by_no_body(const char* no);
static char* generate_get_members_body(const char *pguri);
static void pg_group_member_append(PGGroupMember *head , PGGroupMember *newmem);
static void pg_group_append(PGGroup *head , PGGroup *n);
static void pg_group_prepend(PGGroup *head , PGGroup *n);
*/
LOF_DATA_PGGroupType* LOF_DATA_PGGroup_new(const char *pguri , int identity)
{
	LOF_DATA_PGGroupType *pggroup = (LOF_DATA_PGGroupType*)malloc(sizeof(LOF_DATA_PGGroupType));
	if(pggroup == NULL){
		return NULL;
	}
	memset(pggroup , 0 , sizeof(LOF_DATA_PGGroupType));
	if(pguri)
	    strcpy(pggroup->pguri , pguri);
	pggroup->member = LOF_DATA_PGGroupMember_new();
	if(pggroup->member == NULL){
		free(pggroup);
		return NULL;
	}
	pggroup->identity = identity;
	pggroup->next = pggroup->pre = pggroup;

	return pggroup;
}

void LOF_DATA_PGGroup_append(LOF_DATA_PGGroupType *head , LOF_DATA_PGGroupType *n)
{
	head->next->pre = n;
	n->pre = head;
	n->next = head->next;
	head->next = n;
}

void LOF_DATA_PGGroup_prepend(LOF_DATA_PGGroupType *head , LOF_DATA_PGGroupType *n)
{
	head->pre->next = n;
	n->pre = head->pre;
	n->next = head;
	head->pre = n;
}

LOF_DATA_PGGroupType* LOF_DATA_PGGroup_parse_list(const char *in)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;
	LOF_DATA_PGGroupType *pggroup , *newpg;
	
	doc = xmlReadMemory(in , strlen(in) , NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "group");
	if(!node)
	    return NULL;

	pggroup = LOF_DATA_PGGroup_new(NULL , 0);
	if(pggroup == NULL){
		return NULL;
	}
	while(node){
	    	newpg = LOF_DATA_PGGroup_new(NULL , 0);
		res = xmlGetProp(node , BAD_CAST "uri");
		strcpy(newpg->pguri , (char*)res);
		xmlFree(res);
		if(xmlHasProp(node , BAD_CAST "identity")){
			res = xmlGetProp(node , BAD_CAST "identity");
			newpg->identity = atoi((char*)res);
			xmlFree(res);
		}
		if(newpg->identity == 1)
			LOF_DATA_PGGroup_prepend(pggroup , newpg);
		else
			LOF_DATA_PGGroup_append(pggroup , newpg);
		node = node->next;
	}
	xmlFreeDoc(doc);

	return pggroup;
}

int LOF_DATA_PGGroup_get_list(LOF_DATA_LocalUserType *user)
{
	LOF_SIP_FetionSipType *sip;
	LOF_SIP_SipHeaderType *eheader;
	const char *body = "<args><group-list /></args>";
	char *res;
	extern int callid;

	sip = user->sip;

	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_PGGETGROUPLIST);
	if(eheader == NULL){
		return -1;
	}
	
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	LOF_SIP_FetionSip_add_header(sip , eheader);

	user->pgGroupCallId = callid;

	res = LOF_SIP_to_string(sip , body);
	if(res == NULL){
		return -1;
	}

	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));

	free(res);
	return ret;
}

char *LOF_SIP_PGGroup_generate_get_info_body(LOF_DATA_PGGroupType *pg)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodePtr node1;
	LOF_DATA_PGGroupType *cur;
	char body[] = "<args></args>";


	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "groups" , NULL);
	xmlNewProp(node , BAD_CAST "attributes" , BAD_CAST "all");
	foreach_pg_group(pg , cur){
		node1 = xmlNewChild(node , NULL , BAD_CAST "group" , NULL);
		xmlNewProp(node1 , BAD_CAST "uri" , BAD_CAST cur->pguri);
	}
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}

int LOF_DATA_PGGroup_get_info(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg)
{
	LOF_SIP_FetionSipType *sip;
	LOF_SIP_SipHeaderType *eheader;
	char *res;
	char *body;
	extern int callid;

	sip = user->sip;
	
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_PGGETGROUPINFO);
	if(eheader == NULL){
		return -1;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	user->groupInfoCallId = callid;

	body = LOF_SIP_PGGroup_generate_get_info_body(pg);
	if(body == NULL){
		return -1;
	}
	res = LOF_SIP_to_string(sip , body);
	if(res == NULL){
		return -1;
	}
	free(body);

	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	return ret;
}

int LOF_DATA_PGGroup_parse_info(LOF_DATA_PGGroupType *pg , const char *sipmsg)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlChar *res;
	LOF_DATA_PGGroupType *pgcur;
	char *pos;

	pos = strstr(sipmsg , "\r\n\r\n") + 4;

	doc = xmlReadMemory(pos , strlen(pos)
		, NULL , "UTF-8" , XML_PARSE_RECOVER);
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "group");
	if(!node)
	    return -1;
	while(node != NULL){
		res = xmlGetProp(node , BAD_CAST "uri");		
		foreach_pg_group(pg , pgcur){
			if(xmlStrcmp(res , BAD_CAST pgcur->pguri) == 0)
			    break;
		}
		if(xmlHasProp(node , BAD_CAST "status-code")){
			res = xmlGetProp(node , BAD_CAST "status-code");
			pgcur->statusCode = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "name")){
			res = xmlGetProp(node , BAD_CAST "name");
			strcpy(pgcur->name , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "category")){
			res = xmlGetProp(node , BAD_CAST "category");
			pgcur->category = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "current-member-count")){
			res = xmlGetProp(node , BAD_CAST "current-member-count");
			pgcur->currentMemberCount = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "limit-member-count")){
			res = xmlGetProp(node , BAD_CAST "limit-member-count");
			pgcur->limitMemberCount = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "group-rank")){
			res = xmlGetProp(node , BAD_CAST "group-rank");
			pgcur->groupRank = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "max-rank")){
			res = xmlGetProp(node , BAD_CAST "max-rank");
			pgcur->maxRank = atoi((char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "bulletin")){
			res = xmlGetProp(node , BAD_CAST "bulletin");
			strcpy(pgcur->bulletin , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "introduce")){
			res = xmlGetProp(node , BAD_CAST "introduce");
			strcpy(pgcur->summary , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "create-time")){
			res = xmlGetProp(node , BAD_CAST "create-time");
			strcpy(pgcur->createTime , (char*)res);
			xmlFree(res);
		}
		if(xmlHasProp(node , BAD_CAST "get-group-portrait-hds")){
			res = xmlGetProp(node , BAD_CAST "get-group-portrait-hds");
			strcpy(pgcur->getProtraitUri , (char*)res);
			xmlFree(res);
		}
		node = node->next;
	}
	return 0;
}

char *LOF_SIP_PGGroup_generate_pg_subscribe_body(LOF_DATA_PGGroupType *pg)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node0;
	xmlNodePtr node;
	xmlNodePtr node1;
	char body[] = "<args></args>";


	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node0 = xmlNewChild(node , NULL , BAD_CAST "subscription" , NULL);
	node = xmlNewChild(node0 , NULL , BAD_CAST "groups" , NULL);
	node1 = xmlNewChild(node , NULL , BAD_CAST "group" , NULL);
	xmlNewProp(node1 , BAD_CAST "uri" , BAD_CAST pg->pguri);
	node = xmlNewChild(node0 , NULL , BAD_CAST "presence" , NULL);

	node1 = xmlNewChild(node , NULL , BAD_CAST "basic" , NULL);
	xmlNewProp(node1 , BAD_CAST "attributes" , BAD_CAST "all");

	node1 = xmlNewChild(node , NULL , BAD_CAST "member" , NULL);
	xmlNewProp(node1 , BAD_CAST "attributes" , BAD_CAST "identity");

	node1 = xmlNewChild(node , NULL , BAD_CAST "management" , NULL);
	xmlNewProp(node1 , BAD_CAST "attributes" , BAD_CAST "all");
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}

int LOF_DATA_PGGroup_subscribe(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg)
{
	LOF_SIP_FetionSipType *sip;
	LOF_SIP_SipHeaderType *eheader;
	char *res;
	char *body;

	sip = user->sip;
	
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_PGPRESENCE);
	if(eheader == NULL){
		return -1;
	}
	LOF_SIP_FetionSip_add_header(sip , eheader);
	
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SUBSCRIPTION);
	body = LOF_SIP_PGGroup_generate_pg_subscribe_body(pg);
	if(body == NULL){
		return -1;
	}
	res = LOF_SIP_to_string(sip , body);
	if(res == NULL){
		free(body);
		return -1;
	}
	free(body);
	
	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	return ret;
}

int LOF_DATA_PGGroup_get_group_members(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg)
{
	LOF_SIP_FetionSipType *sip;
	LOF_SIP_SipHeaderType *nheader;
	char *body;
	char *res;

	sip = user->sip;
	
	nheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_PGGETGROUPMEMBERS);
	if(nheader == NULL){
		return -1;
	}
	LOF_SIP_FetionSip_add_header(sip , nheader);
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	pg->getMembersCallId = sip->callid;
	
	body = LOF_SIP_PGGroup_generate_get_members_body(pg->pguri);
	if(body == NULL){
		return -1;
	}
	res = LOF_SIP_to_string(sip , body);
	if(res == NULL){
		free(body);
		return -1;
	}
	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));

	free(res);
	return ret;
}

int LOF_DATA_PGGroup_parse_member_list(LOF_DATA_PGGroupType *pggroup , const char *sipmsg)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodePtr cnode;
	xmlChar *res;
	LOF_DATA_PGGroupType *pgcur;
	LOF_DATA_PGGroupMemberType *member;
	char *pos;

	if(strstr(sipmsg , "\r\n\r\n") == NULL){
		fprintf(stderr , "FATAL ERROR\n");
		return -1;
	}

	pos = strstr(sipmsg , "\r\n\r\n") + 4;

	doc = xmlParseMemory(pos , strlen(pos));
	if(doc == NULL){
		return -1;
	}
	node = xmlDocGetRootElement(doc);
	if(node == NULL){
		xmlFreeDoc(doc);
		return -1;
	}
	node = node->xmlChildrenNode->xmlChildrenNode;
	while(node != NULL){
		if(xmlHasProp(node , BAD_CAST "uri")){
			res = xmlGetProp(node , BAD_CAST "uri");
			foreach_pg_group(pggroup , pgcur){
				if(xmlStrcmp(res , BAD_CAST pgcur->pguri) == 0)
				    break;
			}	    
		}else{
			xmlFreeDoc(doc);
			return -1;
		}
		cnode = node->xmlChildrenNode;
		while(cnode != NULL){
		    	member = LOF_DATA_PGGroupMember_new();
			if(xmlHasProp(cnode , BAD_CAST "uri")){
				res = xmlGetProp(cnode , BAD_CAST "uri");
				strcpy(member->sipuri , (char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(cnode , BAD_CAST "iicnickname")){
				res = xmlGetProp(cnode , BAD_CAST "iicnickname");
				strcpy(member->nickname , (char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(cnode , BAD_CAST "identity")){
				res = xmlGetProp(cnode , BAD_CAST "identity");
				member->identity = atoi((char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(cnode , BAD_CAST "user-id")){
				res = xmlGetProp(cnode , BAD_CAST "user-id");
				strcpy(member->userId , (char*)res);
				xmlFree(res);
			}
			LOF_DATA_PGGroupMember_append(pgcur->member , member);
			cnode = cnode->next;
		}
		node = node->next;
	}
	xmlFreeDoc(doc);
	return 0;
}

LOF_DATA_PGGroupMemberType *LOF_DATA_PGGroupMember_new()
{
	LOF_DATA_PGGroupMemberType *pgmem = (LOF_DATA_PGGroupMemberType*)malloc(sizeof(LOF_DATA_PGGroupMemberType));
	if(pgmem == NULL){
		return NULL;
	}
	memset(pgmem , 0 , sizeof(LOF_DATA_PGGroupMemberType));
	pgmem->contact = NULL;
	pgmem->next = pgmem->pre = pgmem;

	return pgmem;
}

void LOF_DATA_PGGroupMember_append(LOF_DATA_PGGroupMemberType *head , LOF_DATA_PGGroupMemberType *newmem)
{
	head->next->pre = newmem;
	newmem->next = head->next;
	newmem->pre = head;
	head->next = newmem;
}
#if 0
void LOF_DATA_PGGroupMember_prepend(LOF_DATA_PGGroupMemberType *head , LOF_DATA_PGGroupMemberType *newmem)
{
	head->pre->next = newmem;
	newmem->pre = head->pre;
	newmem->next = head;
	head->pre = newmem;
}
#endif

int LOF_DATA_PGGroup_parse_member(LOF_DATA_PGGroupType *pg , const char *sipmsg)
{
	xmlDocPtr doc;
	xmlNodePtr node;
	xmlNodePtr mnode;
	xmlChar *res;
	LOF_DATA_PGGroupType *pgcur;
	LOF_DATA_PGGroupMemberType *member;
	char *pos;

	pos = strstr(sipmsg , "\r\n\r\n") + 4;

	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = LOF_TOOL_xml_goto_node(node , "group");
	while(node != NULL){
		res = xmlGetProp(node , BAD_CAST "uri");
		foreach_pg_group(pg , pgcur){
			if(xmlStrcmp(res , BAD_CAST pgcur->pguri) == 0)
			    break;
		}
		mnode = node->xmlChildrenNode;
		if(!mnode){
			node = node->next;
			continue;
		}
		while(mnode != NULL){
			res = xmlGetProp(mnode , BAD_CAST "uri");
		    	foreach_pg_member(pgcur->member , member){
				if(xmlStrcmp(res , BAD_CAST member->sipuri) == 0)
				    break;
			}
			if(xmlHasProp(mnode , BAD_CAST "identity")){
				res = xmlGetProp(mnode , BAD_CAST "identity");
				member->identity = atoi((char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(mnode , BAD_CAST "state")){
				res = xmlGetProp(mnode , BAD_CAST "state");
				member->status = atoi((char*)res);
				xmlFree(res);
			}
			if(xmlHasProp(mnode , BAD_CAST "client-type")){
				res = xmlGetProp(mnode , BAD_CAST "client-type");
				strcpy(member->clientType , (char*)res);
				xmlFree(res);
			}
	    		mnode = mnode->next;	    
		}

		node = node->next;
	}
	xmlFreeDoc(doc);
	return 0;
}

int LOF_DATA_PGGroup_update_group_info(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	char *sid;
	extern int callid;
	LOF_DATA_PGGroupMemberType *memcur;
	int ret;

	if(pg == NULL || LOF_DATA_PGGroup_get_member_count(pg) == 0)
		return 0;
	pg->hasDetails = 1;
	foreach_pg_member(pg->member , memcur){		
		eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_GETCONTACTINFO);
		if(eheader == NULL){
			return -1;
		}
		LOF_SIP_FetionSip_add_header(sip , eheader);
		
		LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
		memcur->getContactInfoCallId = callid;
		sid = LOF_SIP_get_sid_by_sipuri(memcur->sipuri);
		if(sid == NULL){
			return -1;
		}
		body = LOF_SIP_PGGroup_generate_contact_info_by_no_body(sid);
		if(body == NULL){
			free(sid);
			return -1;
		}
		free(sid);
		res = LOF_SIP_to_string(sip , body);
		if(res == NULL){
			free(body);
			return -1;
		}
		free(body);
		ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
		free(res); res = NULL;
		if(ret == -1){
			return -1;
		}
	}
	return 0;
}

int LOF_DATA_PGGroup_get_member_count(LOF_DATA_PGGroupType *pg)
{
	int count = 0;
	LOF_DATA_PGGroupMemberType *memcur;
	foreach_pg_member(pg->member , memcur){
		count ++;
	}
	return count;
}
LOF_DATA_BuddyContactType* LOF_DATA_PGGroup_parse_contact_info(const char* xml)
{
	LOF_DATA_BuddyContactType* contact;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	char *pos;
	contact = LOF_DATA_BuddyContact_new();
	if(contact == NULL){
		return NULL;
	}
	doc = xmlParseMemory(xml , strlen(xml));
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
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->country , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->province , pos , n);
		pos = strstr(pos , ".") + 1;
		n = strlen(pos) - strlen(strstr(pos , "."));
		strncpy(contact->city , pos , n);
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

int LOF_DATA_PGGroup_send_invitation(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *theader;
	LOF_SIP_SipHeaderType *kheader1;
	LOF_SIP_SipHeaderType *kheader2;
	LOF_SIP_SipHeaderType *kheader3;
	LOF_SIP_SipHeaderType *kheader4;
	LOF_SIP_SipHeaderType *kheader5;
	extern int callid;
	const char *body = "s=session m=message";
	char *res;
	
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_INVITATION);
	theader = LOF_SIP_SipHeader_new("T" , pg->pguri);
	if(theader == NULL){
		goto theader_error;
	}
	kheader1 = LOF_SIP_SipHeader_new("K" , "text/html-fragment");
	if(kheader1 == NULL){
		goto kheader1_error;
	}
	kheader2 = LOF_SIP_SipHeader_new("K" , "multiparty");
	if(kheader1 == NULL){
		goto kheader2_error;
	}
	kheader3 = LOF_SIP_SipHeader_new("K" , "nudge");
	if(kheader1 == NULL){
		goto kheader3_error;
	}
	kheader4 = LOF_SIP_SipHeader_new("K" , "share-background");
	if(kheader1 == NULL){
		goto kheader4_error;
	}
	kheader5 = LOF_SIP_SipHeader_new("K" , "fetion-show");
	if(kheader1 == NULL){
		goto kheader5_error;
	}

	pg->inviteCallId = callid;	

	LOF_SIP_FetionSip_add_header(sip , theader);
	LOF_SIP_FetionSip_add_header(sip , kheader1);
	LOF_SIP_FetionSip_add_header(sip , kheader2);
	LOF_SIP_FetionSip_add_header(sip , kheader3);
	LOF_SIP_FetionSip_add_header(sip , kheader4);
	LOF_SIP_FetionSip_add_header(sip , kheader5);

	res = LOF_SIP_to_string(sip , body);
	if(res == NULL){
		return -1;
	}

	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	return ret;
	
kheader5_error:
	free(kheader4);
kheader4_error:
	free(kheader3);
kheader3_error:
	free(kheader2);
kheader2_error:
	free(kheader1);
kheader1_error:
	free(theader);
theader_error:
	return -1;
}

int LOF_DATA_PGGroup_send_invite_ack(LOF_DATA_LocalUserType *user , const char *sipmsg)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *theader;
	char callid[16];
	char touri[64];
	char *res;

	memset(callid , 0 , sizeof(callid));
	memset(touri , 0 , sizeof(touri));
	LOF_SIP_get_attr(sipmsg , "I" , callid);
	LOF_SIP_get_attr(sipmsg , "T" , touri);

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_ACKNOWLEDGE);
	theader = LOF_SIP_SipHeader_new("T" , touri);
	if(theader == NULL){
		return -1;
	}
	LOF_SIP_FetionSip_set_callid(sip , atoi(callid));
	LOF_SIP_FetionSip_add_header(sip , theader);

	res = LOF_SIP_to_string(sip , NULL);
	if(res == NULL){
		return -1;
	}
	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));

	free(res);
	return ret;
}

int LOF_DATA_PGGroup_send_message(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg , const char *message)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *theader;
	LOF_SIP_SipHeaderType *cheader;
	LOF_SIP_SipHeaderType *kheader;
	char *res;

	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
	theader = LOF_SIP_SipHeader_new("T" , pg->pguri);
	if(theader == NULL){
		goto theader_error;
	}
	cheader = LOF_SIP_SipHeader_new("C" , "text/html-fragment");
	if(cheader == NULL){
		goto cheader_error;
	}
	kheader = LOF_SIP_SipHeader_new("K" , "SaveHistory");
	if(kheader == NULL){
		goto kheader_error;
	}

	LOF_SIP_FetionSip_add_header(sip , theader);
	LOF_SIP_FetionSip_add_header(sip , cheader);
	LOF_SIP_FetionSip_add_header(sip , kheader);

	LOF_SIP_FetionSip_set_callid(sip , pg->inviteCallId);

	res = LOF_SIP_to_string(sip , message);
	if(res == NULL){
		return -1;
	}
	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	return ret;


kheader_error:
	free(cheader);
cheader_error:
	free(theader);
theader_error:
	return -1;
}

int LOF_DATA_PGGroup_send_sms(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg , const char *message)
{
	LOF_SIP_FetionSipType *sip = user->sip;
	LOF_SIP_SipHeaderType *theader;
	LOF_SIP_SipHeaderType *eheader;
	char *res;


	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_MESSAGE);
	theader = LOF_SIP_SipHeader_new("T" , pg->pguri);
	if(theader == NULL){
		goto theader_error;
	}
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_PGSENDCATSMS);
	if(eheader == NULL){
		goto eheader_error;
	}

	LOF_SIP_FetionSip_add_header(sip , theader);
	LOF_SIP_FetionSip_add_header(sip , eheader);

	res = LOF_SIP_to_string(sip , message);
	if(res == NULL){
		return -1;
	}
	int ret = LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	return ret;


eheader_error:
	free(theader);
theader_error:
	return -1;
}

char* LOF_SIP_PGGroup_generate_get_members_body(const char *pguri)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char body[] = "<args></args>";
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "groups" , NULL);
	xmlNewProp(node , BAD_CAST "attributes" , BAD_CAST "member-uri;member-nickname;member-iicnickname;member-identity;member-t6svcid");
	node = xmlNewChild(node , NULL , BAD_CAST "group" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST pguri);
	xmlDocDumpMemory(doc , &buf , NULL);
	xmlFreeDoc(doc);
	if(buf == NULL){
		return NULL;
	}
	return LOF_TOOL_xml_convert(buf);
}

char* LOF_SIP_PGGroup_generate_contact_info_by_no_body(const char* no)
{
	xmlChar *buf;
	xmlDocPtr doc;
	xmlNodePtr node;
	char uri[32];
	char body[] = "<args></args>";
	memset(uri, 0, sizeof(uri));
	sprintf(uri , "sip:%s" , no);
	doc = xmlParseMemory(body , strlen(body));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contact" , NULL);
	xmlNewProp(node , BAD_CAST "uri" , BAD_CAST uri);
	xmlDocDumpMemory(doc , &buf , NULL);
	if(buf == NULL){
		return NULL;
	}
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(buf);
}
