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

LOF_TOOL_FxListType* LOF_TOOL_FxList_new(void* data)
{
	LOF_TOOL_FxListType* fxlist = (LOF_TOOL_FxListType*)malloc(sizeof(LOF_TOOL_FxListType));
	memset(fxlist , 0 , sizeof(LOF_TOOL_FxListType));
	fxlist->pre = fxlist;
	fxlist->data = data;
	fxlist->next = fxlist;
	return fxlist;
}

void LOF_TOOL_FxList_free(LOF_TOOL_FxListType *fxitem)
{
	if(fxitem != NULL)
		free(fxitem);
}

void LOF_TOOL_FxList_append(LOF_TOOL_FxListType *fxlist , LOF_TOOL_FxListType *fxitem)
{
	LOF_TOOL_FxListType *ptr;
	ptr=fxlist;
	while(ptr->next!=ptr)ptr=ptr->next;
	ptr->next = fxitem;
	fxitem->pre = ptr;
}

void LOF_TOOL_FxList_prepend(LOF_TOOL_FxListType *fxlist , LOF_TOOL_FxListType *fxitem)
{
	fxlist->pre->next = fxitem;
	fxitem->pre = fxlist->pre;
	fxitem->next = fxlist;
	fxlist->pre = fxitem;
}

void LOF_TOOL_FxList_remove(LOF_TOOL_FxListType *fxitem)
{
	if((fxitem->next)!=fxitem){(fxitem->pre)->next=fxitem->next;}else
	(fxitem->pre)->next = fxitem->pre;
	free(fxitem->data);
	free(fxitem);
}
void LOF_TOOL_FxList_del(LOF_TOOL_FxListType **fxitem)
{
	LOF_TOOL_FxListType* backupptr;
	if (fxitem == NULL||*fxitem==NULL) return;
	if (((*fxitem)->pre) == *fxitem){
		if (((*fxitem)->next) == *fxitem){
			free((*fxitem)->data);
			free(*fxitem);
			*fxitem = NULL;
			return;
		}
		else{
			backupptr = ((*fxitem)->next);
			free((*fxitem)->data);
			free(*fxitem);
			(*fxitem) = backupptr;
			(*fxitem)->pre = *fxitem;
			return;
		}
	}
	else{
		if (((*fxitem)->next) != *fxitem){

			((*fxitem)->pre)->next = ((*fxitem)->next);
			((*fxitem)->next)->pre = ((*fxitem)->pre);
			free((*fxitem)->data);
			free(*fxitem);

			return;
		}
		else{
			((*fxitem)->pre)->next = ((*fxitem)->pre);
			free((*fxitem)->data);
			free(*fxitem);
			return;
		}
	}
	LOF_debug_error("Did Nothing While Deleting A FxList.");
	return;
}
