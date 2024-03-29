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

#ifndef LOF_SIP_H
#define LOF_SIP_H



/**
 * create a FetionSip object and initialize it 
 * @param tcp The tcp connection object that FetionSip used to send and recv message
 * @param sid Login user`s fetion number
 * @return The FetionSip object created;
 */
extern LOF_SIP_FetionSipType* LOF_SIP_FetionSip_new(LOF_CONNECTION_FetionConnectionType* tcp , const char* sid);

/**
 * clone a new FetionSip object with the existed FetionSip object
 * @param sip The FetionSip object to be cloned from
 * @return The new FetionSip object just cloned
 */
extern LOF_SIP_FetionSipType* LOF_SIP_FetionSip_clone(LOF_SIP_FetionSipType* sip);

/**
 * create a sip header in type of in the form of "T:sip:*****@fetion.com.cn"
 * @note this function can just be used when you want to expand the
 * fetion protocol stack,otherwise just call the protocol functions ,
 * they can meet most of your demand.
 * @param name The name of the header , in the example above , name is "T"
 * @param value The value of the header , int the example above , value is "sip:*****@fetion.con.cn"
 * @return The sip header just created
 */
extern LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_new(const char* name , const char* value);

/**
 * set the type of sip , only need to be called before 
 * constructing a new sip request message
 * such as message with type 'M' , "I" , "S",etc...
 * @note this function can just be used when you want to expand the 
 * fetion protocol stack,otherwise just call the protocol functions ,
 * they can meet most of your demand.
 * @param sip The FetionSip object to be set.
 * @param type The new sip type.
 */
extern void LOF_SIP_FetionSip_set_type(LOF_SIP_FetionSipType *sip, LOF_TOOL_SipType type);

/**
 * set the sip`s callid with the given callid instead of using the autoincrement callid
 * @note this function can just be used when you want to exannd the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 * @param sip The sip object to be sest.
 * @param callid The new callid
 */
extern void LOF_SIP_FetionSip_set_callid(LOF_SIP_FetionSipType* sip , int callid);

/** 
 * @note this function can just be used when you want to expand the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 */
extern LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_authentication_new(const char* response);

/** 
 * @note this function can just be used when you want to expand the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 */
extern LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_ack_new(const char* code , const char* algorithm , const char* type , const char* guid);

/** 
 * @note this function can just be used when you want to expand the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 */
extern LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_event_new(int eventType);

/** 
 * @note this function can just be used when you want to expand the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 */
extern LOF_SIP_SipHeaderType* LOF_SIP_SipHeader_credential_new(const char* credential);

/** 
 * @note this function can just be used when you want to expand the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 */
extern void LOF_SIP_FetionSip_add_header(LOF_SIP_FetionSipType* sip , LOF_SIP_SipHeaderType* header);

/** 
 * @note this function can just be used when you want to expand the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 */
extern char* LOF_SIP_to_string(LOF_SIP_FetionSipType* sip , const char* body);

/**
 * free the resource of the sip object after use
 */
extern void LOF_SIP_FetionSip_free(LOF_SIP_FetionSipType* sip);

/**
 * A very commonly used function.It convert the sipuri to sid
 * @param sipuri The sip uri to get sid from
 * @return sid get from the sipuri. Need to be freed after use.
 */
extern char* LOF_SIP_get_sid_by_sipuri(const char* sipuri);

/**
 * Another very commonly used function.It convert the fetion group uri
 * to sid , when pgsipuri is 'sip:PG444444@fetion.com.cn' , it returns 444444
 * @return Need to be freed after use.
 */
extern char* LOF_SIP_get_pgid_by_sipuri(const char *pgsipuri);

/**
 * get the attribute value with the given attribute name in a sip message
 * @param sipmsg The sip message to be parsed 
 * @param name The attribute name
 * @param retulst The attribute value
 * @return 1 if success , or else -1
 */
extern int LOF_SIP_get_attr(const char* sip , const char* name , char* result);

/**
 * get the length of the sip message body which is in form of xml
 * @return the length of the xml body , 0 if no xml body
 */
extern int LOF_SIP_get_length(const char* sip);

/**
 * get the code of the reponse sip message , for example
 * the input message is "SIP-C/4.0 200 OK ...." , it returns 200
 */
extern int LOF_SIP_get_code(const char* sip);

/**
 * get the type of the sip message 
 */
extern int LOF_SIP_get_type(const char* sip);

/** 
 * @note this function can just be used when you want to expand the 
 * fetion protocal stack,otherwise just call the protocal functions ,
 * they can meet most of your demand.
 */
extern void LOF_SIP_get_auth_attr(const char* auth , char** ipaddress , int* port , char** credential);

/**
 * get the response message when a request message already sent
 * @return The response message sent from the sipc server,need to be freed after use.
 */
extern char* LOF_SIP_get_response(LOF_SIP_FetionSipType* sip);

/**
 * set a new tcp connection object for the specified sip object
 */
extern void LOF_SIP_FetionSip_set_connection(LOF_SIP_FetionSipType* sip , LOF_CONNECTION_FetionConnectionType* conn);
/**
 * listen the sipc message channel,and returns the sip message 
 * pushed from the server.
 * @note this function should be put at a loop
 * @return the SipMsg object which contains the message related information
 */
extern LOF_DATA_SipMsgType* LOF_SIP_FetionSip_listen(LOF_SIP_FetionSipType *sip, int *error);

extern int LOF_SIP_keep_alive(LOF_SIP_FetionSipType* sip);

extern void LOF_DATA_SipMsg_free(LOF_DATA_SipMsgType* msg);

extern void LOF_DATA_SipMsg_append(LOF_DATA_SipMsgType* msglist , LOF_DATA_SipMsgType* msg);

extern void LOF_SIP_parse_notification(const char* sip , int* type , int* event , char** xml);

extern void LOF_SIP_parse_message(LOF_SIP_FetionSipType* sip , const char* sipmsg , LOF_DATA_FetionMessageType** msg);

extern void LOF_SIP_parse_invitation(LOF_SIP_FetionSipType* sip , LOF_CONNECTION_ProxyType *proxy , const char* sipmsg
		, LOF_SIP_FetionSipType** conversionSip , char** sipuri);

extern void LOF_SIP_parse_addbuddyapplication(const char* sipmsg
		, char** sipuri , char** userid , char** desc , int* phrase);

extern void LOF_SIP_parse_incoming(LOF_SIP_FetionSipType* sip
		, const char* sipmsg , char** sipuri
		, LOF_EVENT_IncomingType* type , LOF_EVENT_IncomingActionType *action);

extern void LOF_SIP_parse_userleft(const char* sipmsg , char** sipuri);

extern int LOF_SIP_parse_shareaccept(LOF_SIP_FetionSipType *sip
		, const char* sipmsg , LOF_DATA_ShareType *share);

extern void LOF_SIP_parse_sysmsg(const char* sipmsg , int *type
		, int* showonce , char **content , char **url);

extern int LOF_SIP_parse_sipc(const char *sipmsg , int *callid , char **xml);

extern struct tm LOF_TOOL_convert_date(const char* date);

extern inline void
LOF_SIP_set_conn(LOF_SIP_FetionSipType *sip, LOF_CONNECTION_FetionConnectionType *conn);
extern char *LOF_TOOL_generate_action_accept_body(LOF_DATA_ShareType *share);
extern LOF_DATA_SipMsgType *LOF_DATA_SipMsg_new(void);
extern void LOF_DATA_SipMsg_set_msg(LOF_DATA_SipMsgType *sipmsg, const char *msg, int n);
#endif
