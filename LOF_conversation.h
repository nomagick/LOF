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

#ifndef LOF_CONVERSION_H
#define LOF_CONVERSION_h

/**
 * construct a conversation object
 * @uesr Global User object
 * @sipuri To specify the user in the conversation
 */
extern LOF_USER_ConversationType* LOF_USER_Conversation_new(LOF_DATA_LocalUserType* user,
		  const char* sipuri , LOF_SIP_FetionSipType* sip);
extern LOF_USER_ConversationType* LOF_USER_Conversation_new_sid(LOF_DATA_LocalUserType* user,
		  const char* sid , LOF_SIP_FetionSipType* sip);
/**
 * send a message to the user in this conversation
 * @note before you send a message to an online buddy , in other word ,
 * the buddy`s state is larger than 0 , then you need to send an invitation 
 * to the buddy first using function "fetion_conversation_invite_friend()"
 * or else the message will fail to send
 * @param conv To specify the conversation to send a sms to 
 * @param msg the message to be sent
 */
extern int LOF_USER_Conversation_send_sms(LOF_USER_ConversationType* conversation , const char* msg);

extern int LOF_USER_Conversation_send_sms_with_reply(LOF_USER_ConversationType *conv, const char *msg);

/**
 *  send a sms to yourself
 */
extern int LOF_USER_Conversation_send_sms_to_myself(LOF_USER_ConversationType* conversation,
	   	const char* message);

extern int LOF_USER_Conversation_send_sms_to_myself_with_reply(LOF_USER_ConversationType* conversation,
			   	const char* message);

/**
 * send a message directly to the user`s phone in the specified conversation
 * @param conversation To specify the user to whom the message will be sent
 * @param message The message body
 * @return 1 if success , or else -1
 */
extern int LOF_USER_Conversation_send_sms_to_phone(LOF_USER_ConversationType* conversation,
	   	const char* message);

/**
 * send a message directly to the user`s phone in the specified conversation
 * and parse the response message.
 * @param conversation To specify the buddy to whom the message will be sent
 * @param message The message body
 * @param daycount Will be filled with the count of messages you send in the current day
 * @param mountcount Will be filled with the count of messages you send in the current month
 * @return 1 if success , or else -1
 */
extern int LOF_USER_Conversation_send_sms_to_phone_with_reply(LOF_USER_ConversationType* conversation
		, const char* message , int* daycount , int* monthcount);

/**
 * invite an online buddy to a conversation
 * @param conversation To specify the buddy to whom the message will be sent
 * @return 1 if success , or else -1
 */
extern int LOF_USER_Conversation_invite_friend(LOF_USER_ConversationType* conversation);

/**
 * send a window nudge to a buddy
 * @param conversation To specify the buddy to whom the nudge will be sent
 * @return 1 if success , or else -1
 */
extern int LOF_USER_Conversation_send_nudge(LOF_USER_ConversationType* conversation);

extern void LOF_USER_Conversation_parse_send_sms(const char* xml , int* daycount , int* mountcount);
extern char* LOF_SIP_generate_send_nudge_body();
extern char* LOF_SIP_generate_invite_friend_body(const char* sipuri);

#endif
