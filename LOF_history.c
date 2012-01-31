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

#include "LOF_openfetion.h"
#include <errno.h>

LOF_DATA_HistoryType* LOF_DATA_History_message_new(const char* name,
		const char* userid, struct tm time,
		const char* msg , const int issend)
{
	LOF_DATA_HistoryType* history = (LOF_DATA_HistoryType*)malloc(sizeof(LOF_DATA_HistoryType));
	memset(history , 0 , sizeof(LOF_DATA_HistoryType));

	strcpy(history->name , name);
	strcpy(history->userid , userid);
	strftime(history->sendtime,
			sizeof(history->sendtime),
			"%Y-%m-%d %H:%M:%S" , &time);
	snprintf(history->message,
			sizeof(history->message) - 1,
			"%s" , msg);
	history->issend = issend;

	return history;
}

void LOF_DATA_History_message_free(LOF_DATA_HistoryType* history)
{
	free(history);
}
LOF_DATA_FetionHistoryType* LOF_DATA_FetionHistory_new(LOF_DATA_LocalUserType* user)
{
	LOF_DATA_FetionHistoryType* fhistory;
	LOF_TOOL_ConfigType* config = user->config;
	char filepath[128];
	
	fhistory = (LOF_DATA_FetionHistoryType*)malloc(sizeof(LOF_DATA_FetionHistoryType));
	memset(fhistory , 0 , sizeof(LOF_DATA_FetionHistoryType));
	fhistory->user = user;
	sprintf(filepath, "%s/data.db",
			config->userPath);
	if(sqlite3_open(filepath, &(fhistory->db)))
		LOF_debug_error("open data.db:%s", sqlite3_errmsg(fhistory->db));
	
	return fhistory;
}

void LOF_DATA_FetionHistory_free(LOF_DATA_FetionHistoryType* fhistory)
{
	if(fhistory){
		if(fhistory->db)
			sqlite3_close(fhistory->db);
		free(fhistory);
	}
}
void LOF_DATA_FetionHistory_add(LOF_DATA_FetionHistoryType* fhistory , LOF_DATA_HistoryType* history)
{
	sqlite3 *db;
	char sql[4096];
	char sql1[4096];
	db = fhistory->db;

	if(!db){
		LOF_debug_error("db is closed,write history FAILED");
		return;
	}

	LOF_TOOL_escape_sql(history->message);
	snprintf(sql, sizeof(sql), "insert into history values"
			" (NULL,'%s','%s','%s',datetime('%s'),%d);",
			history->name, history->userid,
			history->message, history->sendtime,
			history->issend);

	if(sqlite3_exec(db, sql, 0, 0, NULL)){
		snprintf(sql1, sizeof(sql1),"create table history ("
				"id INTEGER PRIMARY KEY AUTOINCREMENT,"
				"name TEXT,userid TEXT,message TEXT,"
				"updatetime TEXT,issend INTEGER);");
		if(sqlite3_exec(db, sql1, 0, 0, NULL)){
			LOF_debug_error("create table history:%s",sqlite3_errmsg(db));
			return;
		}
		if(sqlite3_exec(db, sql, 0, 0, NULL))
			LOF_debug_error("%s\n%s",sqlite3_errmsg(db), sql);
	}

}

LOF_TOOL_FxListType* LOF_DATA_FetionHistory_get_list(LOF_TOOL_ConfigType* config,
		const char* userid , int count)
{
	sqlite3 *db;
	char sql[4096];
	char path[256];
	char **res;
	int nrows, ncols, start, i;
	LOF_TOOL_FxListType *hislist, *pos;
	LOF_DATA_HistoryType *his;

	snprintf(path, sizeof(path),"%s/data.db", config->userPath);

	hislist = LOF_TOOL_FxList_new(NULL);

	LOF_debug_info("Load chat history with %s",
			userid);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("open data.db:%s", sqlite3_errmsg(db));
		return hislist;
	}

	snprintf(sql, sizeof(sql),"select * from history"
			" where userid='%s' order"
			" by id desc limit %d;",
			userid, count);

	if(sqlite3_get_table(db, sql, &res, &nrows, &ncols, NULL)){
		sqlite3_close(db);
		return hislist;
	}

	for(i = 0; i < nrows; i ++){
		start = ncols + i * ncols;
		his = (LOF_DATA_HistoryType*)malloc(sizeof(LOF_DATA_HistoryType));
		memset(his , 0 , sizeof(LOF_DATA_HistoryType));
		strcpy(his->name, res[start+1]);
		strcpy(his->userid, res[start+2]);
		strcpy(his->message, res[start+3]);
		if(res[start+4])
			strcpy(his->sendtime, res[start+4]);
		his->issend = atoi(res[start+5]);
		LOF_TOOL_unescape_sql(his->message);
		pos = LOF_TOOL_FxList_new(his);
		LOF_TOOL_FxList_prepend(hislist , pos);
	}
	
	sqlite3_free_table(res);
	sqlite3_close(db);
	return hislist;
}

LOF_TOOL_FxListType* LOF_DATA_History_get_e_list(LOF_TOOL_ConfigType *config,
		const char *userid , int type)
{
	sqlite3 *db;
	char sql[4096];
	char path[256];
	char condition[256];
	char **res;
	int nrows, ncols, start, i;
	LOF_TOOL_FxListType *hislist, *pos;
	LOF_DATA_HistoryType *his;

	snprintf(path, sizeof(path),"%s/data.db", config->userPath);

	hislist = LOF_TOOL_FxList_new(NULL);

	LOF_debug_info("Load chat history with %s", userid);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("open data.db:%s", sqlite3_errmsg(db));
		return hislist;
	}

	switch(type){
		case LOF_HISTORY_TODAY:
			snprintf(condition, sizeof(condition),
				"strftime('%%Y',updatetime) == strftime('%%Y','now') and "
				"strftime('%%m',updatetime) == strftime('%%m','now') and "
				"strftime('%%d',updatetime) == strftime('%%d','now') ");
			break;
		case LOF_HISTORY_WEEK:
			snprintf(condition, sizeof(condition),
				"strftime('%%Y',updatetime) == strftime('%%Y','now') and "
				"strftime('%%W',updatetime) == strftime('%%W','now') ");
			break;
		case LOF_HISTORY_MONTH:
			snprintf(condition, sizeof(condition),
				"strftime('%%Y',updatetime) == strftime('%%Y','now') and "
				"strftime('%%m',updatetime) == strftime('%%m','now') ");
			break;
		case LOF_HISTORY_ALL:
			sprintf(condition, "1==1");
			break;
		default:
			break;
	};

	snprintf(sql, sizeof(sql),"select * from history"
			" where userid='%s' and %s order"
			" by id desc;",
			userid, condition);

	if(sqlite3_get_table(db, sql, &res, &nrows, &ncols, NULL)){
		sqlite3_close(db);
		return hislist;
	}

	for(i = 0; i < nrows; i ++){
		start = ncols + i * ncols;
		his = (LOF_DATA_HistoryType*)malloc(sizeof(LOF_DATA_HistoryType));
		memset(his , 0 , sizeof(LOF_DATA_HistoryType));
		strcpy(his->name, res[start+1]);
		strcpy(his->userid, res[start+2]);
		strcpy(his->message, res[start+3]);
		if(res[start+4])
			strcpy(his->sendtime, res[start+4]);
		his->issend = atoi(res[start+5]);
		LOF_TOOL_unescape_sql(his->message);
		pos = LOF_TOOL_FxList_new(his);
		LOF_TOOL_FxList_prepend(hislist , pos);
	}
	
	sqlite3_free_table(res);
	return hislist;
}

int LOF_DATA_History_export(LOF_TOOL_ConfigType *config , const char *myid
		, const char *userid , const char *filename)
{
	sqlite3 *db;
	char sql[4096];
	char text[4096];
	char path[256];
	char **res;
	int nrows, ncols, start, i;
	FILE *f;

	if(!(f = fopen(filename, "w+"))){
		LOF_debug_error("export chat history FAILED");
		return -1;
	}

	sprintf(path, "%s/data.db",
				   	config->userPath);

	LOF_debug_info("Export chat history with %s",
			userid);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("open data.db:%s",
					sqlite3_errmsg(db));
		fclose(f);
		return -1;
	}

	sprintf(sql, "select * from history"
			" where userid='%s' order"
			" by id;",
			userid);

	if(sqlite3_get_table(db, sql, &res, &nrows, &ncols, NULL)){
		sqlite3_close(db);
		fclose(f);
		return -1;
	}

	for(i = 0; i < nrows; i ++){
		start = ncols * (i + 1);
		sprintf(text, "%s(%s) %s\n",
				res[start+1], 
				atoi(res[start+5]) ? myid : res[start+2],
				res[start+4]);
		strcpy(sql, res[start+3]);
		LOF_TOOL_unescape_sql(sql);
		strcat(text , sql);
		strcat(text , "\n\n");
		fwrite(text , strlen(text) , 1 , f);
		fflush(f);
	}
	
	sqlite3_free_table(res);
	sqlite3_close(db);
	fclose(f);
	return 1;
}

int LOF_DATA_History_delete(LOF_TOOL_ConfigType *config, const char *userid)
{
	sqlite3 *db;
	char sql[4096];
	char path[256];

	snprintf(path, sizeof(path),"%s/data.db", config->userPath);

	LOF_debug_info("Delete chat history with %s", userid);
	if(sqlite3_open(path, &db)){
		LOF_debug_error("open data.db:%s", sqlite3_errmsg(db));
		return -1;
	}
	sprintf(sql, "delete from history where userid = '%s'", userid);
	if(sqlite3_exec(db, sql, 0, 0, NULL)){
		LOF_debug_error("delete history with %s failed:%s",
				userid, sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	sqlite3_close(db);
	return 1;
}
