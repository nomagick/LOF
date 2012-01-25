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

#ifndef LOF_CONNECTION_H
#define LOF_CONNECTION_H




extern LOF_CONNECTION_FetionConnectionType* LOF_CONNECTION_FetionConnection_new(void);

extern LOF_CONNECTION_FetionConnectionType* LOF_CONNECTION_FetionConnection_new_with_port(const int port);

extern LOF_CONNECTION_FetionConnectionType* LOF_CONNECTION_FetionConnection_new_with_ip_and_port(const char* ipaddress , const int port);

extern int LOF_CONNECTION_FetionConnection_connect(LOF_CONNECTION_FetionConnectionType* connection
		, const char* ipaddress , const int port);

extern int LOF_CONNECTION_FetionConnection_connect_with_proxy(LOF_CONNECTION_FetionConnectionType* connection
		, const char* ipaddress , const int port , LOF_CONNECTION_ProxyType *proxy);

extern int LOF_CONNECTION_FetionConnection_send(LOF_CONNECTION_FetionConnectionType* connetions , const void* bufs , int lens);

extern int LOF_CONNECTION_FetionConnection_recv(LOF_CONNECTION_FetionConnectionType* connection , void* recv , int len);

extern void LOF_CONNECTION_FetionConnection_close(LOF_CONNECTION_FetionConnectionType *connection);

extern int LOF_CONNECTION_FetionConnection_select_read(LOF_CONNECTION_FetionConnectionType* connection);

extern int LOF_CONNECTION_FetionConnection_recv_dont_wait(LOF_CONNECTION_FetionConnectionType* connection , void* buf , int len);
extern int LOF_CONNECTION_FetionConnection_getname(LOF_CONNECTION_FetionConnectionType* connection , char **ip , int *port);

extern int LOF_CONNECTION_FetionConnection_ssl_connection_start(LOF_CONNECTION_FetionConnectionType* conn);

extern char* LOF_CONNECTION_FetionConnection_ssl_connection_get(LOF_CONNECTION_FetionConnectionType* conn , const char* buf);

extern char *LOF_CONNECTION_http_connection_get_response(LOF_CONNECTION_FetionConnectionType *conn);

extern int LOF_TOOL_http_connection_get_body_length(const char* http);

extern int LOF_TOOL_http_connection_get_head_length(const char* http);

extern char* LOF_TOOL_http_connection_encode_url(const char* url);

extern void LOF_CONNECTION_FetionConnection_free(LOF_CONNECTION_FetionConnectionType* conn);

extern char* LOF_TOOL_get_ip_by_name(const char* hostname);

extern char *LOF_TOOL_get_local_ip(void);

extern char *LOF_TOOL_hexip_to_dotip(const char *ip);
#endif
