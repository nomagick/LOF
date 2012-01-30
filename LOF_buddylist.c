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

int LOF_BUDDY_list_create(LOF_DATA_LocalUserType* user , const char* name)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_CREATEBUDDYLIST);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_create_buddylist_body(name);
	res = LOF_SIP_to_string(sip , body);
	free(body);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res) ; 
	res = LOF_SIP_get_response(sip);
	ret = LOF_SIP_get_code(res);
	if(ret == 200)
	{
		ret = LOF_SIP_parse_create_buddylist_response(user , res);
		free(res);
		LOF_debug_info("Create buddy list success");
		return ret;
	}
	else
	{
		free(res);
		LOF_debug_error("Create buddy list failed , errno :" , ret);
		return -1;
	}
}
int LOF_BUDDY_list_delete(LOF_DATA_LocalUserType* user , int id)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_DELETEBUDDYLIST);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_delete_buddylist_body(id);
	res = LOF_SIP_to_string(sip , body);
	free(body);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	res = LOF_SIP_get_response(sip);
	ret = LOF_SIP_get_code(res);
	free(res);
	if(ret == 200)
	{
		LOF_DATA_BuddyGroup_remove(user->groupList , id);
		LOF_debug_info("Delete buddy list success");
		return 1;
	}
	else
	{
		LOF_debug_error("Delete buddy list failed , errno:%d" , ret);
		return -1;
	}
}
int LOF_BUDDY_list_edit(LOF_DATA_LocalUserType* user , int id , const char* name)
{
	LOF_SIP_FetionSipType* sip = user->sip;
	LOF_SIP_SipHeaderType* eheader;
	char *res , *body;
	int ret;
	LOF_SIP_FetionSip_set_type(sip , LOF_SIP_SERVICE);
	eheader = LOF_SIP_SipHeader_event_new(LOF_SIP_EVENT_SETBUDDYLISTINFO);
	LOF_SIP_FetionSip_add_header(sip , eheader);
	body = LOF_SIP_generate_edit_buddylist_body(id , name);
	res = LOF_SIP_to_string(sip , body);
	free(body);
	LOF_CONNECTION_FetionConnection_send(sip->tcp , res , strlen(res));
	free(res);
	res = LOF_SIP_get_response(sip);
	ret = LOF_SIP_get_code(res);
	free(res);
	if(ret == 200)
	{
		LOF_debug_info("Set buddy list name to %s success" , name);
		return 1;
	}
	else
	{
		LOF_debug_error("Set buddy list name to %s failed , errno:%d" , name , ret);
		return -1;
	}
}

char* LOF_SIP_generate_create_buddylist_body(const char* name)
{
	char args[] = "<args></args>";
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy-lists" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy-list" , NULL);
	xmlNewProp(node , BAD_CAST "name" , BAD_CAST name);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}
char* LOF_SIP_generate_edit_buddylist_body(int id , const char* name)
{
	char args[] = "<args></args>";
	char ids[128];
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy-lists" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy-list" , NULL);
	xmlNewProp(node , BAD_CAST "name" , BAD_CAST name);
	memset(ids, 0, sizeof(ids));
	snprintf(ids, sizeof(ids) - 1 , "%d" , id);
	xmlNewProp(node , BAD_CAST "id" , BAD_CAST ids);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);

}
char* LOF_SIP_generate_delete_buddylist_body(int id)
{
	char args[] = "<args></args>";
	char ida[4];
	memset(ida, 0, sizeof(ida));
	sprintf(ida , "%d" , id);
	xmlChar *res;
	xmlDocPtr doc;
	xmlNodePtr node;
	doc = xmlParseMemory(args , strlen(args));
	node = xmlDocGetRootElement(doc);
	node = xmlNewChild(node , NULL , BAD_CAST "contacts" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy-lists" , NULL);
	node = xmlNewChild(node , NULL , BAD_CAST "buddy-list" , NULL);
	xmlNewProp(node , BAD_CAST "id" , BAD_CAST ida);
	xmlDocDumpMemory(doc , &res , NULL);
	xmlFreeDoc(doc);
	return LOF_TOOL_xml_convert(res);
}
int LOF_SIP_parse_create_buddylist_response(LOF_DATA_LocalUserType* user , const char* sipmsg)
{
	char *pos;
	LOF_DATA_BuddyGroupType* group;
	xmlChar* res;
	xmlDocPtr doc;
	xmlNodePtr node;
	int groupid;
	pos = strstr(sipmsg , "\r\n\r\n") + 4;
	doc = xmlParseMemory(pos , strlen(pos));
	node = xmlDocGetRootElement(doc);
	node = node->xmlChildrenNode;
	res = xmlGetProp(node , BAD_CAST "version");
	strcpy(user->contactVersion , (char*)res);
	xmlFree(res);
	node = node->xmlChildrenNode->xmlChildrenNode;
	group = LOF_DATA_BuddyGroup_new();
	res = xmlGetProp(node , BAD_CAST "name");
	strcpy(group->groupname , (char*)res);
	xmlFree(res);
	res = xmlGetProp(node , BAD_CAST "id");
	group->groupid = atoi((char*)res);
	groupid = group->groupid;
	xmlFree(res);
	xmlFreeDoc(doc);
	LOF_DATA_BuddyGroup_list_append(user->groupList , group);
	return groupid;
}
