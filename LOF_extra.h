#ifndef LOF_EXTRA_H
#define LOF_EXTRA_H
#define LOF_COMMAND_MAX_TIME 30

int LOF_GLOBAL_CommandId = 1;

typedef struct {clock_t begin, end;}LOF_TOOL_StopWatchType;
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
extern LOF_TOOL_CommandType* LOF_TOOL_Command_new(LOF_TOOL_Command_InstructionType command,const char* pram1,const char* pram2);
extern int LOF_TOOL_Command_exec_send_msg(LOF_TOOL_CommandType* The_Command,LOF_TOOL_FxListType* ConversationList);
extern int LOF_TOOL_Command_main(LOF_TOOL_FxListType** Command_List,LOF_TOOL_FxListType* ConversationList);
extern int LOF_TOOL_Command_ack_sipc40 (LOF_TOOL_FxListType* Command_List, char* sipc40msg);
extern int LOF_TOOL_Command_arrange(LOF_DATA_LocalUserType* user,LOF_TOOL_FxListType** Command_List,const char* command,const char* pram1,const char* pram2);
extern void LOF_TOOL_Command_destroy(LOF_TOOL_CommandType* The_Command);
extern int LOF_CallBack_Message (LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List , char* sipmsg);
extern int LOF_CallBack_Invitation(LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List ,char* sipmsg);
extern int LOF_CallBack_Presence(LOF_TOOL_FxListType* ConversationListPtr , LOF_TOOL_FxListType** Command_List , char* sipmsg);


#endif
