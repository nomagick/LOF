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

#ifndef LOF_MESSAGE_H
#define LOF_MESSAGE_H

extern LOF_DATA_FetionMessageType* LOF_DATA_FetionMessage_new();

extern int LOF_DATA_FetionMessage_set_sipuri(LOF_DATA_FetionMessageType* msg , const char* sipuri);

extern int LOF_DATA_FetionMessage_set_pguri(LOF_DATA_FetionMessageType* msg , const char* sipuri);

extern int LOF_DATA_FetionMessage_set_message(LOF_DATA_FetionMessageType* msg , const char* message);

extern void LOF_DATA_FetionMessage_set_time(LOF_DATA_FetionMessageType* msg , struct tm sendtime);

extern void LOF_DATA_FetionMessage_set_callid(LOF_DATA_FetionMessageType* msg , int callid);

extern void LOF_DATA_FetionMessage_free(LOF_DATA_FetionMessageType* msg);

extern LOF_DATA_UnackedListType *LOF_DATA_UnackedList_new(LOF_DATA_FetionMessageType *message);

extern void LOF_DATA_UnackedList_append(LOF_DATA_UnackedListType *head	, LOF_DATA_UnackedListType *newnode);

extern void LOF_DATA_UnackedList_remove(LOF_DATA_UnackedListType *head,LOF_DATA_UnackedListType *delnode);

extern char* LOF_TOOL_contruct_message_sip(const char *sid, LOF_DATA_FetionMessageType *msg);

#define unacked_list_empty(head) (head->next == head)


#define foreach_unacked_list(head,c) \
		for(c=head;(c=c->next)!=head;)



#endif
