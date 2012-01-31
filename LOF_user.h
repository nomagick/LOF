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

#ifndef LOF_USER_H
#define LOF_USER_H
LOF_DATA_UnackedListType *LOF_GLOBAL_unackedlist;
/*#define STATE_ONLINE       400
#define STATE_RIGHTBACK    300
#define STATE_AWAY         100
#define STATE_BUSY         600
#define STATE_OUTFORLUNCH  500
#define STATE_ONTHEPHONE   150
#define STATE_MEETING      850
#define STATE_DONOTDISTURB 800
#define STATE_HIDDEN       0
#define STATE_NOTAPRESENCE -1

#define USER_AUTH_NEED_CONFIRM(u) ((u)->loginStatus == 421 || (u)->loginStatus == 420)
#define USER_AUTH_ERROR(u)        ((u)->loginStatus == 401 || (u)->loginStatus == 400 || (u)->loginStatus == 404)
*/
/**
 * create a User object and initialize it with number and password
 * @no Phone number or Fetion number
 * @password Nothing special , just your fetion password
 * @return The user object created
 */
extern LOF_DATA_LocalUserType* LOF_DATA_LocalUser_new(const char* no , const char* password);

extern void LOF_DATA_LocalUser_set_userid(LOF_DATA_LocalUserType* user, const char* userid1);

extern void LOF_DATA_LocalUser_set_sid(LOF_DATA_LocalUserType* user, const char* sId1);

extern void LOF_DATA_LocalUser_set_mobileno(LOF_DATA_LocalUserType* user , const char* mobileno1);

extern void LOF_DATA_LocalUser_set_password(LOF_DATA_LocalUserType *user, const char *password);

extern void LOF_DATA_LocalUser_set_sip(LOF_DATA_LocalUserType* user , LOF_SIP_FetionSipType* sip1);

extern void LOF_DATA_LocalUser_set_config(LOF_DATA_LocalUserType* user , LOF_TOOL_ConfigType* config1);

extern void LOF_DATA_LocalUser_set_verification_code(LOF_DATA_LocalUserType* user , const char* code);

extern int LOF_DATA_LocalUser_init_config(LOF_DATA_LocalUserType *user);

extern void LOF_DATA_LocalUser_free(LOF_DATA_LocalUserType* user);

/**
 * upload portrait
 * @param user Global User object
 * @param filename The absolute filepath of the portrait file to be uploaded
 * @return 1 if success , or else -1
 */
extern int LOF_DATA_LocalUser_upload_avatar(LOF_DATA_LocalUserType* user , const char* filename);

/**
 * download portrait of specified user
 * @param user Global User object
 * @param sipuri sip uri of the user whose portrait will be downloaded
 * @return 1 if success, or else -1
 */
extern int LOF_DATA_LocalUser_download_avatar(LOF_DATA_LocalUserType* user , const char* sipuri);

/**
 * download portrait of specified sipuri with
 * the specified server name and server path of the portrait server
 * @param user Global User object
 * @param sipuri sip uri of the user(or Feiton Group) whose portrait will be downloaded
 * @param server The host name of the portrait server
 * @param portraitPath The uri path of the portrait server,like '/HD00S/getportrait.aspx'
 * @return 1 if success , or else -1
 */
extern int LOF_DATA_LocalUser_download_avatar_with_uri(LOF_DATA_LocalUserType *user , const char *sipuri
       	, const char *server , const char *portraitpath);

/**
 * modify the user`s online state such as 'Online' , 'Busy' , etc..
 * @param user Global User object
 * @param state The online state type
 * @return 1 if success , or else -1
 */
extern int LOF_DATA_LocalUser_set_status(LOF_DATA_LocalUserType* user , LOF_USER_StatusType state);

/**
 * modify the user`s personal signature
 * @param user Global User object
 * @param moodphrase The new personal signature string
 * @return 1 if success , or else -1
 */
extern int LOF_DATA_LocalUser_set_moodphrase(LOF_DATA_LocalUserType* user , const char* moodphrase);

/**
 * reload the user`s detail information from the sipc server
 * @param user Global User object
 * @return 1 if  success , or else -1
 */
extern int LOF_DATA_LocalUser_update_info(LOF_DATA_LocalUserType* user);

/**
 * send a keep-alive message to the sipc server to tell that the client 
 * is not offline.This function should be called periodically.
 * @param user Global User object
 * @return 1 if success , or else -1
 */
extern int LOF_DATA_LocalUser_keep_alive(LOF_DATA_LocalUserType* user);

/**
 * traverse the two-way linked list of user group
 * @param head the list header
 * @param gl the list cursor
 */
#define foreach_grouplist(head , gl) \
	for(gl = head ; (gl = gl->next) != head ;)

/**
 * construct a double-linked list to store the group information
 */
extern LOF_DATA_BuddyGroupType* LOF_DATA_BuddyGroup_new();

/**
 * append a new group to the group list
 */
extern void LOF_DATA_BuddyGroup_list_append(LOF_DATA_BuddyGroupType* head , LOF_DATA_BuddyGroupType* group);

/**
 * prepend a new group to the group list
 */
extern void LOF_DATA_BuddyGroup_list_prepend(LOF_DATA_BuddyGroupType* head , LOF_DATA_BuddyGroupType* group);

/**
 * remove a specified group from the group list
 * @param group the group to be removed
 */
extern void LOF_DATA_BuddyGroup_list_remove(LOF_DATA_BuddyGroupType *group);

/**
 * remove a group from the list with the specified group id
 */
extern void LOF_DATA_BuddyGroup_remove(LOF_DATA_BuddyGroupType* head , int groupid);

/**
 * find a group from the list with the specified group id
 */
extern LOF_DATA_BuddyGroupType* LOF_DATA_BuddyGroup_list_find_by_id(LOF_DATA_BuddyGroupType* head , int id);

extern LOF_DATA_VerificationType* LOF_DATA_Verification_new();

extern void LOF_DATA_Verification_free(LOF_DATA_VerificationType* ver);

extern LOF_DATA_BuddyContactType* LOF_SIP_parse_presence_body(const char* body , LOF_DATA_LocalUserType* user);

extern LOF_DATA_BuddyContactType* LOF_SIP_parse_syncuserinfo_body(const char* body , LOF_DATA_LocalUserType* user);

extern int LOF_DATA_LocalUser_set_sms_status(LOF_DATA_LocalUserType *user , int days);

extern void LOF_DATA_LocalUser_save(LOF_DATA_LocalUserType *user);

extern void LOF_DATA_LocalUser_load(LOF_DATA_LocalUserType *user);


#endif
