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

#ifndef LOF_DIRECTSMS_H
#define LOF_DIRECTSMS_H

#define LOF_PIC_SUCCESS 1
#define LOF_PIC_ERROR -1
#define LOF_UNKNOW_ERROR -2

#define LOF_SEND_SMS_SUCCESS 1
#define LOF_SEND_SMS_NEED_AUTHENTICATION -1
#define LOF_SEND_SMS_OTHER_ERROR -2

#define LOF_DSMS_OPTION_SUCCESS 1
#define LOF_DSMS_OPTION_FAILED -1

extern void
LOF_SIP_DRMS_parse_option_verification(LOF_DATA_LocalUserType *user , const char *in);

extern int LOF_DRMS_send_option(LOF_DATA_LocalUserType *user , const char *response);
extern int
LOF_SIP_DRMS_parse_subscribe_response(const char *in , char **error);
extern int LOF_DRMS_send_subscribe(LOF_DATA_LocalUserType *user , const char *code , char **error);
extern int LOF_DRMS_send_sms(LOF_DATA_LocalUserType *user
		, const char *to , const char *msg);



#endif
