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

#ifndef LOF_GROUP_H
#define LOF_GROUP_H

#define foreach_pg_group(head , cur) \
    for(cur = head; (cur = cur->next) != head ;)

/**
 * send get group list request to server
 * @param user Global User object
 */
extern int LOF_DATA_PGGroup_get_list(LOF_DATA_LocalUserType *user);

/**
 * parse the response string for get group list request
 * @param sipmsg The response string
 * @return head of group list
 */
extern LOF_DATA_PGGroupType* LOF_DATA_PGGroup_parse_list(const char *in);

/**
 * send a get group information request to server
 * @param user Global User object
 * @param pg To specify which group to get information for
 */
extern int LOF_DATA_PGGroup_get_info(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg);

/**
 * send a group subscribtion request to server
 * @param user Global User object
 * @param pg To specify which group to subscribe
 */
extern int LOF_DATA_PGGroup_subscribe(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg);

/**
 * parse the response string of get group info request
 * @param pg The head of the group list
 * @param sipmsg The response string to be parsed
 */
extern int LOF_DATA_PGGroup_parse_info(LOF_DATA_PGGroupType *pg , const char *sipmsg);

/**
 * send a get group members request to server
 * @param user Global User object
 * @param pg To specify which group to get members for
 */
extern int LOF_DATA_PGGroup_get_group_members(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg);

/**
 * parse the response string of get members request
 * @param pggroup The head of group list
 * @param sipmsg The response string
 */
extern int LOF_DATA_PGGroup_parse_member_list(LOF_DATA_PGGroupType *pggroup , const char *sipmsg);


#define foreach_pg_member(head , cur) \
    for(cur = head; (cur = cur->next)!= head ;)

/**
 * Construct a PGGroupMember object
 */
extern LOF_DATA_PGGroupMemberType *LOF_DATA_PGGroupMember_new();

/**
 * parse the member presence information pushed from the server
 * @param pg The head of the group list
 * @param the notification sip message
 */
extern int LOF_DATA_PGGroup_parse_member(LOF_DATA_PGGroupType *pg , const char *sipmsg);

/**
 * send update group member information request to server
 * and sip message containing user information will be pushed from the server
 * @param user Global User object
 * @param To specify which group will update its member information
 */
extern int LOF_DATA_PGGroup_update_group_info(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg);

/**
 * get member count of a specified group
 * @param pg The group to be calculated
 * @return the member count of the specified group
 */
extern int LOF_DATA_PGGroup_get_member_count(LOF_DATA_PGGroupType *pg);

/**
 * parse the body of respones string pushed from the server after function 
 * "pg_group_update_group_info()" was called
 * @param the information sip message
 * @return A new contact parsed from the xml
 */
extern LOF_DATA_BuddyContactType* LOF_DATA_PGGroup_parse_contact_info(const char* xml);

/**
 * send a dialog invitation to a specified group
 * @param user Global User object
 * @param pg To specfy which group the invitation will be sent to 
 */
extern int LOF_DATA_PGGroup_send_invitation(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg);

/**
 * send a group message to a specified group
 * @param user Global User object
 * @param pg To specify whicl group the message will be sent to
 * @param message The message body
 */
extern int LOF_DATA_PGGroup_send_message(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg , const char *message)
;

/**
 * send a group sms to a specified group
 * @param user Global User object
 * @param pg To specify whicl group the message will be sent to
 * @param message The message body
 */
extern int LOF_DATA_PGGroup_send_sms(LOF_DATA_LocalUserType *user , LOF_DATA_PGGroupType *pg , const char *message)
;

/**
 * send an acknowledge message after receiving the dialog invitation response message
 * @param user Global User object
 * @param sipmsg The dialog invitation response message
 */
extern int LOF_DATA_PGGroup_send_invite_ack(LOF_DATA_LocalUserType *user , const char *sipmsg);

extern char* LOF_SIP_PGGroup_generate_get_members_body(const char *pguri);
extern char* LOF_SIP_PGGroup_generate_contact_info_by_no_body(const char* no);
extern void LOF_DATA_PGGroupMember_append(LOF_DATA_PGGroupMemberType *head , LOF_DATA_PGGroupMemberType *newmem);

#endif
