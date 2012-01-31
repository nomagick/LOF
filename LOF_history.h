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

#ifndef LOF_HISTORY_H
#define LOF_HISTORY_H

#define LOF_HISTORY_TODAY 1
#define LOF_HISTORY_WEEK  2
#define LOF_HISTORY_MONTH 3
#define LOF_HISTORY_ALL   4

extern LOF_DATA_HistoryType* LOF_DATA_History_message_new(const char* name,
		const char* userid, struct tm time,
		const char* msg , const int issend);

extern void LOF_DATA_History_message_free(LOF_DATA_HistoryType* history);

extern LOF_DATA_FetionHistoryType* LOF_DATA_FetionHistory_new(LOF_DATA_LocalUserType* user);

extern void LOF_DATA_FetionHistory_free(LOF_DATA_FetionHistoryType* fhistory);

extern void LOF_DATA_FetionHistory_add(LOF_DATA_FetionHistoryType* fhistory , LOF_DATA_HistoryType* history);

extern LOF_TOOL_FxListType* LOF_DATA_FetionHistory_get_list(LOF_TOOL_ConfigType* config,
		const char* userid , int count);

extern LOF_TOOL_FxListType* LOF_DATA_History_get_e_list(LOF_TOOL_ConfigType *config,
		const char *userid , int type);

extern int LOF_DATA_History_export(LOF_TOOL_ConfigType *config , const char *myid
		, const char *userid , const char *filename);

extern int LOF_DATA_History_delete(LOF_TOOL_ConfigType *config, const char *userid);

#endif
