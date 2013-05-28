#ifndef LOF_EXTRA_H
#define LOF_EXTRA_H
#define LOF_COMMAND_MAX_TIME 30
#define LOF_MAX_CONCURRENT 3

#include <fcntl.h>

extern LOF_TOOL_StopWatchType* LOF_TOOL_StopWatch_new();
extern void LOF_TOOL_StopWatch_start(LOF_TOOL_StopWatchType* the_watch);
extern void LOF_TOOL_StopWatch_stop(LOF_TOOL_StopWatchType* the_watch);
extern int LOF_TOOL_StopWatch_read(LOF_TOOL_StopWatchType* the_watch);
extern void LOF_TOOL_AutoKeepAlive(LOF_DATA_LocalUserType* user,LOF_TOOL_StopWatchType* the_watch,int waitsec);
typedef enum {
		LOF_COMMAND_STATUS_NOT_HANDLED = 	0,
		LOF_COMMAND_STATUS_ON_HOLD = 	 2 ,
		LOF_COMMAND_STATUS_SUCCESS = 	1 ,
		LOF_COMMAND_STATUS_FAIL = 		-1
	} LOF_TOOL_Command_StatusType;
typedef enum {
			LOF_COMMAND_SEND_MSG = 1,
			LOF_COMMAND_SEND_SMS = 	 2 ,
			LOF_COMMAND_SEND_NUDGE = 	3 ,
			LOF_COMMAND_ADD_BUDDY = 	4,
			LOF_COMMAND_RM_BUDDY = 5,
			LOF_COMMAND_MV_BUDDY = 6,
			LOF_COMMAND_CP_BUDDY = 7,
			LOF_COMMAND_DUMP_STATUS=8,
			LOF_COMMAND_DUMP_MSG = 9,
			LOF_COMMAND_DUMP_CONTACT = 10
		} LOF_TOOL_Command_InstructionType;
typedef struct {
	LOF_TOOL_Command_StatusType Status;
	LOF_TOOL_Command_InstructionType Instruction;
	int CommandId;
	int Progress;
	int Retry;
	int Callid;
	LOF_TOOL_StopWatchType* Timer;
	char* Pram1;
	char* Pram2;
	char* BackSipMsg;
}LOF_TOOL_CommandType;
extern LOF_TOOL_CommandType* LOF_TOOL_Command_new(LOF_TOOL_Command_InstructionType command,const char* pram1,const char* pram2,int outerID);
extern int LOF_TOOL_Command_main(LOF_TOOL_FxListType** Command_List,LOF_TOOL_FxListType* ConversationList,LOF_TOOL_FxListType** MSG_List,int* ResultTunnel);
extern int LOF_TOOL_Command_ack_sipc40 (LOF_TOOL_FxListType* Command_List, char* sipc40msg);
extern int LOF_TOOL_Command_arrange(LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** Command_List,const char* command,const char* pram1,const char* pram2,int outerID);
extern void LOF_TOOL_Command_destroy(LOF_TOOL_CommandType* The_Command);

extern int LOF_TOOL_Command_exec_send_msg(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* ConversationList);
extern int LOF_TOOL_Command_exec_send_sms(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* ConversationList);
extern int LOF_TOOL_Command_exec_dump_msg(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* MSGList,LOF_DATA_LocalUserType* User);
extern int LOF_TOOL_Command_exec_dump_status(LOF_TOOL_CommandType* The_Command,LOF_DATA_LocalUserType* User);
extern int LOF_TOOL_Command_exec_dump_contact(LOF_TOOL_CommandType* The_Command,LOF_DATA_LocalUserType* User);

extern int LOF_CallBack_Message (LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List , char* sipmsg);
extern int LOF_CallBack_Invitation(LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List ,char* sipmsg);
extern int LOF_CallBack_Presence(LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List , char* sipmsg);
extern int LOF_File_prepare(LOF_DATA_LocalUserType* user,const char* fname);
extern int LOF_FIFO_prepare(LOF_DATA_LocalUserType* user,const char* fname);
extern int LOF_TOOL_Command_ParseStr(LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** cmdlist,char* str);
extern int LOF_FIFO_open(LOF_DATA_LocalUserType* user,const char* fname,int write);
extern int LOF_FILE_MSG_dump(LOF_TOOL_FxListType* msg_list,FILE* filehandle);
extern int LOF_FILE_Contact_dump(LOF_DATA_LocalUserType* user,FILE* filehandle);
extern int LOF_FILE_Status_dump(LOF_DATA_LocalUserType* user,FILE* filehandle);
extern int LOF_TOOL_Command_ParseJson(int* fifo,LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** cmdlist);
#endif
