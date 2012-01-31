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

#ifndef LOF_CONFIG_H
#define LOF_CONFIG_H



extern LOF_TOOL_ConfigType* LOF_TOOL_Config_new();

extern LOF_TOOL_FxListType* LOF_TOOL_Config_get_phrase(LOF_TOOL_ConfigType* config);

extern int LOF_TOOL_Config_initialize(LOF_TOOL_ConfigType* config , const char* userid);
extern int LOF_DATA_LocalUser_download_configuration(LOF_DATA_LocalUserType* user);

//extern int fetion_config_load_size(Config *config);

//extern int fetion_config_save_size(Config *config);

//extern int fetion_config_get_use_status_icon(Config *config);

//extern int fetion_config_set_use_status_icon(Config *config);

extern int LOF_TOOL_Config_load(LOF_DATA_LocalUserType *user);

extern LOF_CONNECTION_ProxyType* LOF_TOOL_Config_load_proxy();

extern void LOF_TOOL_Config_save_proxy(LOF_CONNECTION_ProxyType *proxy);

extern int LOF_TOOL_Config_save(LOF_DATA_LocalUserType *user);

//extern char* fetion_config_get_city_name(const char* province , const char* city);

//extern char* fetion_config_get_province_name(const char* province);
extern LOF_DATA_LocalUserListType* LOF_DATA_LocalUserList_find_by_no(LOF_DATA_LocalUserListType* list , const char* no);

extern LOF_TOOL_FxListType* LOF_TOOL_Config_get_phrase(LOF_TOOL_ConfigType* config);

extern void LOF_TOOL_Phrase_free(LOF_TOOL_PhraseType* phrase);

extern void LOF_TOOL_Config_free(LOF_TOOL_ConfigType *config);

/*user list*/
#define foreach_userlist(head , ul) \
	for(ul = head ; (ul = ul->next) != head;)

extern LOF_DATA_LocalUserListType* LOF_DATA_LocalUserList_new(const char *no,
    	const char *password, const char *userid,
	const char *sid, int laststate , int islastuser);

extern void LOF_DATA_LocalUserList_append(LOF_DATA_LocalUserListType* head , LOF_DATA_LocalUserListType* ul);

extern void LOF_DATA_LocalUserList_save(LOF_TOOL_ConfigType* config , LOF_DATA_LocalUserListType* ul);

extern void LOF_DATA_LocalUserList_set_lastuser_by_no(LOF_DATA_LocalUserListType* ul , const char* no);

extern int LOF_DATA_LocalUserList_remove(LOF_TOOL_ConfigType *config, const char *no);



extern LOF_DATA_LocalUserListType* LOF_DATA_LocalUserList_load(LOF_TOOL_ConfigType* config);

extern void LOF_DATA_LocalUserList_update_userid(LOF_TOOL_ConfigType *config,
		const char *no, const char *userid);

extern void LOF_DATA_LocalUserList_free(LOF_DATA_LocalUserListType *list);

extern char* LOF_SIP_generate_configuration_body(LOF_DATA_LocalUserType* user);
extern LOF_DATA_LocalUserListType* LOF_DATA_LocalUserList_find_by_no(LOF_DATA_LocalUserListType* list , const char* no);

extern xmlNodePtr LOF_TOOL_xml_goto_node(xmlNodePtr node , const char* name);

extern char* LOF_TOOL_xml_convert(xmlChar* in);

extern void LOF_TOOL_escape_sql(char *in);

extern void LOF_TOOL_unescape_sql(char *inn);

#endif 
