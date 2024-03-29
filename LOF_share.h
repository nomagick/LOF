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

#ifndef LOF_SHARE_H
#define LOF_SHARE_H

#define LOF_MAX_FILE_SIZE 2097151

extern LOF_DATA_ShareType *LOF_DATA_Share_new_with_path(const char *sipuri , const char *absolutePath);

extern LOF_DATA_ShareType *LOF_DATA_Share_new(const char *sipuri);

extern void LOF_DATA_Share_request(LOF_SIP_FetionSipType *sip , LOF_DATA_ShareType *share);

//extern void fetion_share_response_accept(FetionSip *sip , Share *share);
extern char* LOF_SIP_generate_share_request_body(LOF_DATA_ShareType *share);
extern void LOF_DATA_Share_start_transfer(LOF_SIP_FetionSipType *sip);
extern char* LOF_TOOL_compute_md5(const char *absolutePath);
extern long long LOF_DATA_Share_get_filesize(const char *absolutePath);
#endif
