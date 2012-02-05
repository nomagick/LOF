#ifndef LOF_EXTRA_H
#define LOF_EXTRA_H
#define LOF_TOOL_SLEEP_TARGET_SECOND 2.0



typedef struct {clock_t begin, end;}LOF_TOOL_StopWatchType;
extern LOF_TOOL_StopWatchType* LOF_TOOL_StopWatch_new();
extern void LOF_TOOL_StopWatch_start(LOF_TOOL_StopWatchType* the_watch);
extern void LOF_TOOL_StopWatch_stop(LOF_TOOL_StopWatchType* the_watch);
void LOF_TOOL_AutoSleep(LOF_TOOL_StopWatchType* the_watch,int usleeptime);
typedef enum {
		LOF_COMMAND_STATUS_NOT_HANDALED = 	0,
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
	int Progress;
	int Callid;
	char* Pram1;
	char* Pram2;
	char* BackSipMsg;
}LOF_TOOL_CommandType;
extern LOF_TOOL_CommandType* LOF_TOOL_Command_new(LOF_TOOL_Command_InstructionType command,const char* pram1,const char* pram2);
#endif
