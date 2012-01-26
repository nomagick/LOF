/*
 * LOF_types.h
 *
 *  Created on: 2012-1-23
 *      Author: avastms
 */

#ifndef LOF_TYPES_H_
#define LOF_TYPES_H_
/* inline function to trace program track */
/*NO IDEA START*/
#ifdef UNUSED
#elif defined(__GNUC__)
# 	define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#else
#	define UNUSED(x) x
#endif
/*NO IDEA END*/

#define LOF_FETION_NAME "OpenFetion"
#define LOF_FETION_VERSION ""
#define LOF_PROTO_VERSION "4.3.0980"
#define LOF_NAVIGATION_URI "nav.fetion.com.cn"
#define LOF_PGGROUP_SPACE_URI "http://group.feixin.10086.cn/space/Index/"
#define LOF_LOGIN_TYPE_FETIONNO    			1
#define LOF_LOGIN_TYPE_MOBILENO    			0
#define LOF_BOUND_MOBILE_ENABLE    			1
#define LOF_BOUND_MOBILE_DISABLE   			0
#define LOF_BASIC_SERVICE_NORMAL   			1
#define LOF_BASIC_SERVICE_ABNORMAL 			0
#define LOF_CARRIER_STATUS_OFFLINE			-1
#define LOF_CARRIER_STATUS_NORMAL  			0
#define LOF_CARRIER_STATUS_DOWN    			1
#define LOF_CARRIER_STATUS_CLOSED           2
#define LOF_RELATION_STATUS_AUTHENTICATED   1
#define LOF_RELATION_STATUS_UNAUTHENTICATED 0

#define LOF_SERVICE_DOWN_MESSAGE "您的手机已停机，目前无法使用此功能，请缴费后重试。"
/**
 * some other buddylists
 */
typedef enum {
	LOF_BUDDY_LIST_NOT_GROUPED = 0 ,
	LOF_BUDDY_LIST_STRANGER =   -1 ,
	LOF_BUDDY_LIST_PGGROUP =      -2
} LOF_USER_BuddyListType;

/**
 * Presence statuses
 */
typedef enum {
	LOF_STATUS_ONLINE = 		 400 ,
	LOF_STATUS_RIGHTBACK = 	 300 ,
	LOF_STATUS_AWAY = 		 100 ,
	LOF_STATUS_BUSY = 		 600 ,
	LOF_STATUS_OUTFORLUNCH =  500 ,
	LOF_STATUS_ONTHEPHONE = 	 150 ,
	LOF_STATUS_MEETING = 	 850 ,
	LOF_STATUS_DONOTDISTURB = 800 ,
	LOF_STATUS_HIDDEN = 		 0 ,
	LOF_STATUS_OFFLINE =      -1
} LOF_USER_StatusType;
#define LOF_USER_AUTH_NEED_CONFIRM(u) ((u)->loginStatus == 421 || (u)->loginStatus == 420)
#define LOF_USER_AUTH_ERROR(u)        ((u)->loginStatus == 401 || (u)->loginStatus == 400 || (u)->loginStatus == 404)
/**
 * Type used to indicate whether user`s avatar has been changed
 */
typedef enum {
	LOF_AVATAR_NOT_INITIALIZED = -1 ,		/* portrait has not been initialized */
	LOF_AVATAR_NOT_CHANGED ,					/* portrait does not change 		 */
	LOF_AVATAR_CHANGED ,						/* portrait has been changed 		 */
	LOF_AVATAR_SET
} LOF_USER_AvatarStatusType;

/**
 * Type to indicate user`s service status
 */
typedef enum {
	LOF_CMCC_STATUS_NORMAL = 1 ,					/* normal status											 */
	LOF_CMCC_STATUS_OFFLINE ,					/* user offline , you were deleted from his list or out of service*/
	LOF_CMCC_STATUS_NOT_AUTHENTICATED ,			/* user has not accept your add buddy request				 */
	LOF_CMCC_STATUS_SMS_ONLINE ,					/* user has not start fetion service						 */
	LOF_CMCC_STATUS_REJECTED ,					/* user rejected your add buddy request,wait to be deleted 	 */
	LOF_CMCC_STATUS_SERVICE_CLOSED , 			/* user has closed his fetion service 						 */
	LOF_CMCC_STATUS_NOT_BOUND					/* user didn`t bound the fetion number to any cell phone number 		 */
} LOF_CMCC_ServiceStatusType;

/**
 * Two-way linked list that any type can be sticked in
 */
typedef struct LOF_fxlist {
	struct LOF_fxlist *pre;
	void          *data;
	struct LOF_fxlist *next;
} LOF_TOOL_FxListType;

/**
 * Fetion Connection
 */
typedef struct {
	int socketfd;						/* socket file descriptor*/
	char local_ipaddress[16];			/* local ip address      */
	int local_port;						/* local port			 */
	char remote_ipaddress[16];			/* remote ip address	 */
	int remote_port;					/* remote port 			 */
	SSL* ssl;							/* SSL handle			 */
	SSL_CTX* ssl_ctx;					/* SSL ctx struct 		 */
} LOF_CONNECTION_FetionConnectionType;

/**
 * Sip header that in form of "name: value" such as "AK: ak-value"
 */
typedef struct LOF_sipheader {
	char              name[8];			/* sip header namne*/
	char             *value;			/* sip header value*/
	struct LOF_sipheader *next;				/* next sip header */
} LOF_SIP_SipHeaderType;

/**
 * Sip type include some common attributes
 */
typedef struct {
	int type;							/* sip message type						  */
	char from[20];						/* sender`s fetion no ,in sip it`s "F: "  */
	int callid;
	int sequence;						/* sequence number , in sip it`s "Q: "    */
	int threadCount;					/* listening threads count using this sip */
	char sipuri[48];					/* outer sipuri used when listening       */
	LOF_SIP_SipHeaderType* header;					/* some othre header list				  */
	LOF_CONNECTION_FetionConnectionType* tcp;				/* fetion connection used to send message */
} LOF_SIP_FetionSipType;

/**
 * Sip message list that parsed from received chunk
 */
typedef struct LOF_sipmsg {
	char          *message;
	struct LOF_sipmsg *next;
} LOF_DATA_SipMsgType;

/**
 * Contact lists information (Two-way linked list)
 */
typedef struct LOF_contact {
	char userId[16];					/* userid used since v4 protocal      				*/
	char sId[16];						/* fetion no					      				*/
	char sipuri[48];					/* sipuri like 'sip:100@fetion.com.cn'				*/
	char localname[256];					/* name set by yourself				  				*/
	char nickname[256];					/* user`s nickname					    			*/
	char impression[2048];				/* user`s mood phrase				    			*/
	char mobileno[12];					/* mobile phone number				    			*/
	char devicetype[10];				/* user`s client type , like PC and J2ME,etc		*/
	char portraitCrc[12];				/* a number generated by crc algorithm 				*/
	char birthday[16];					/* user`s bitrhday									*/
	char country[6];					/* user`s country`s simplified form,like CN 		*/
	char province[6];					/* user`s province`s simplified form,like bj		*/
	char city[6];						/* user`s city`s code ,like 10 for beijing			*/
	int identity;						/* whethere to show mobileno to this user   		*/
	int scoreLevel;						/* user`s score level,unused now					*/
	int serviceStatus;					/* basic service status 							*/
	int carrierStatus;
	int relationStatus;
	char carrier[16];
	LOF_USER_StatusType status;					/* state type like online,busy,etc					*/
	char groupids[48];
	int groupid;						/* buddylist id										*/
	int gender;							/* gender 1 for male 2 for female,0 for private		*/
	int imageChanged;					/* whether user`s portrait has changed				*/
	int dirty;                          /* whether the contact just read from the server is
										   newer than that in the local disk */
	struct LOF_contact* next;
	struct LOF_contact* pre;
} LOF_DATA_BuddyContactType;

/**
 * Buddy lists information (Two-way linked list)
 */
typedef struct LOF_group {
	char groupname[32];					/* current buddy list name  */
	int groupid;						/* current buddy list Id	*/
	int dirty;
	struct LOF_group *next;
	struct LOF_group *pre;
} LOF_DATA_BuddyGroupType;

/*No Idea Start*/
typedef struct LOF_pggroupmember {
	char sipuri[64];
	char nickname[256];
	char clientType[64];
	char userId[16];
	int status;
	int identity;
	int getContactInfoCallId;
	LOF_DATA_BuddyContactType *contact;
	struct LOF_pggroupmember *next;
	struct LOF_pggroupmember *pre;
} LOF_DATA_PGGroupMemberType;


typedef struct LOF_pggroup {
	char pguri[64];
	char name[256];
	int statusCode;
	int category;
	int currentMemberCount;
	int limitMemberCount;
	int groupRank;
	int maxRank;
	int identity;

	int hasAcked;
	int hasDetails;
	int hasImage;
	int inviteCallId;
	int getMembersCallId;

	char createTime[48];
	char bulletin[1024];
	char summary[1024];
	char getProtraitUri[1024];
	LOF_DATA_PGGroupMemberType *member;
	struct LOF_pggroup *next;
	struct LOF_pggroup *pre;
} LOF_DATA_PGGroupType;

/*No Idea End*/

/**
 * Verification information used for picture code confirm
 */
typedef struct {
	char *algorithm;
	char *type;
	char *text;
	char *tips;
	char *code;
	char *guid;
} LOF_DATA_VerificationType;

/**
 * User list store in local data file  (One-way linked list)
 */
typedef struct LOF_userlist {
	char no[24];						/* fetion no or mobile no  		*/
	char password[48];					/* password 			   		*/
	char userid[48];
	char sid[48];
	int laststate;						/* last state when logining		*/
	int islastuser;						/* is the last logined user		*/
	struct LOF_userlist *pre;
	struct LOF_userlist *next;				/* next User node				*/
} LOF_DATA_LocalUserListType;

/**
 * structure used to describe global proxy information
 */
typedef struct {
	int proxyEnabled;					/* whether http proxy is enable							  */
	char proxyHost[48];					/* proxy host name or ip address						  */
	int proxyPort;					/* port number of proxy server							  */
	char proxyUser[48];					/* username for proxy authentication					  */
	char proxyPass[48];					/* password for proxy authentication 					  */
} LOF_CONNECTION_ProxyType;

/* for close action */
//#define CLOSE_ALERT_ENABLE   0
//#define CLOSE_ALERT_DISABLE  1
/* for nitification action */
//#define MSG_ALERT_ENABLE   0
//#define MSG_ALERT_DISABLE  1
/**
 * Configuration information
 */
typedef struct {
	char globalPath[256];				/* global path,default $(HOME)/.openfetion                */
	char userPath[256];					/* user path , directory name by user`s sid in globalPath */
	char iconPath[256];					/* path stores user`s friend portraits in user`s path     */
	char sipcProxyIP[20];				/* sipc proxy server`s ip ,read from configuration.xml    */
	int sipcProxyPort;					/* sipc proxy server`s port , read from configuration.xml */
	char avatarServerName[48];		/* avatar server`s hostname ,read from configuration.xml*/
	char avatarServerPath[32];		/* avatar server`s path , such as /HD_POOL8             */
//	int iconSize;						/* portrait`s display size default 25px					  */
//	int closeAlert;						/* whether popup an alert when quiting					  */
//	int autoReply;						/* whether auto reply enabled							  */
//	int isMute;
//	char autoReplyMessage[180];			/* auto reply message content							  */
//	int msgAlert;
//	int autoPopup;						/* whether auto pupup chat dialog enabled				  */
//	int sendMode;						/* press enter to send message or ctrl + enter 			  */
//	int closeMode;						/* close button clicked to close window or iconize it	  */
//	int canIconify;
//	int allHighlight;
//	int autoAway;
//	int autoAwayTimeout;
//	int onlineNotify;
//	int closeSysMsg;
//	int closeFetionShow;
//	int useStatusIcon;

//	int window_width;
//	int window_height;
//	int window_pos_x;
//	int window_pos_y;

	char configServersVersion[16];		/* the version of some related servers such as sipc server	*/
	char configParametersVersion[16];
	char configHintsVersion[16];		/* the version of hints										*/

	LOF_DATA_LocalUserListType *ul;				/* user list stored in local data file					  */
	LOF_CONNECTION_ProxyType *proxy;						/* structure stores the global proxy information 		  */
} LOF_TOOL_ConfigType;

/**
 * User`s personal information and some related structs
 */
typedef struct {
	char sId[16];						/* fetion number 											*/
	char userId[16];					/* user id													*/
	char mobileno[16];					/* mobile phone number										*/
	char password[48];					/* raw password not hashed									*/
	char sipuri[48];					/* sipuri like 'sip:100@fetion.com.cn'						*/
	char publicIp[32];					/* public ip of current session								*/
	char lastLoginIp[32];				/* public ip of last login									*/
	char lastLoginTime[48];				/* last login time , got after sipc authentication			*/

	char personalVersion[16];			/* the version of personal information						*/
	char contactVersion[16];			/* the version of contact information						*/
	char customConfigVersion[16];		/* the version of custom config string,unused now			*/

	char nickname[48];					/* nickname of yourself										*/
	char impression[256];				/* mood phrase of yourself									*/
	char avatarCrc[16];				/* a number generated by crc algorithm						*/
	char country[6];					/* the country which your number belongs to					*/
	char province[6];					/* the province which your number belongs to				*/
	char city[6];						/* the city which your number belongs to 					*/
	int gender;							/* the gender of your self									*/
	char smsOnLineStatus[32];

	int smsDayLimit;
	int smsDayCount;
	int smsMonthLimit;
	int smsMonthCount;

	int pgGroupCallId;					/* callid for get group list request */
	int groupInfoCallId;					/* callid for get group info request */

	int status;							/* presence state											*/
	int loginType;   					/* using sid or mobileno									*/
	int loginStatus; 					/* login status code 										*/
	int carrierStatus;
	int boundToMobile;					/* whether this number is bound to a mobile number  */
	long loginTimes;
	int contactCount;
	int groupCount;
	char* ssic;						    /* cookie string read from reply message after ssi login 	*/
	char* customConfig;					/* custom config string used to set personal information	*/
	LOF_DATA_VerificationType* verification;			/* a struct used to generate picture code					*/
	LOF_DATA_BuddyContactType* contactList;				/* friend list of current user								*/
	LOF_DATA_BuddyGroupType* groupList;					/* buddylist list of current user							*/
	LOF_DATA_PGGroupType* pggroup;					/* group list */
	LOF_TOOL_ConfigType* config;						/* config information										*/
	LOF_SIP_FetionSipType* sip;						/* sip object used to handle sip event						*/
} LOF_DATA_LocalUserType;

/**
 * structure used to describe conversation information
 */
typedef struct {
	LOF_DATA_BuddyContactType    *currentContact;			 /* current friend who you a chating with					   */
	LOF_DATA_LocalUserType       *currentUser;			 /* current user,ourselves									   */
	LOF_SIP_FetionSipType  *currentSip;				 /* sip struct used to send message
										  * NULL if did not start a chat channel for this conversation */
} LOF_CONNECTION_ConversationType;




/**
 * structure used to describe message information
 */
typedef struct {
	char      *message;						 /* message body  		*/
	char      *sipuri;						 /* sender`s sip uri 		*/
	char      *pguri;
	int        callid;
	int        sysback;
	struct tm  sendtime;					 /* message sent time 		*/
} LOF_DATA_FetionMessageType;

/**
 * structure used to describe chat history
 */
typedef struct {
	char name[48];						 /* name of message sender	   */
	char userid[16];					 /* userid of message sender   */
	char sendtime[32];					 /* message sent time		   */
	char message[4096];					 /* message content			   */
	int  issend;						 /* message is sent of received*/
} LOF_DATA_HistoryType;

typedef struct {
	LOF_DATA_LocalUserType* user;
	sqlite3 *db;
} LOF_DATA_FetionHistoryType;

/*No Idea Start*/

#define LOF_FILE_RECEIVE 1
#define LOF_FILE_SEND 2

#define LOF_FILE_ACCEPTED 1
#define LOF_FILE_DECLINED 2

typedef struct {

	LOF_SIP_FetionSipType *sip;
	int shareMode;
	int shareState;
	char guid[64];
	char sessionid[64];
	char absolutePath[1024];
	char filename[64];
	char sipuri[64];
	char md5[64];
	long long filesize;
	char preferType[8];
	char innerIp[24];
	int innerUdpPort;
	int innerTcpPort;
	char outerIp[24];
	int outerUdpPort;
	int outerTcpPort;
} LOF_DATA_ShareType;

typedef struct LOF_unacked_list {
	int timeout;
	LOF_DATA_FetionMessageType *message;
	struct LOF_unacked_list *next;
	struct LOF_unacked_list *pre;
}LOF_DATA_UnackedListType;

/*No Idea End*/

typedef enum
{
	LOF_SIP_REGISTER = 1 ,
	LOF_SIP_SERVICE ,
 	LOF_SIP_SUBSCRIPTION ,
	LOF_SIP_NOTIFICATION ,
	LOF_SIP_INVITATION ,
	LOF_SIP_INCOMING ,
	LOF_SIP_OPTION ,
	LOF_SIP_MESSAGE ,
	LOF_SIP_SIPC_4_0 ,
	LOF_SIP_ACKNOWLEDGE ,
	LOF_SIP_UNKNOWN
} LOF_TOOL_SipType;

typedef enum
{
	LOF_NOTIFICATION_TYPE_PRESENCE ,
	LOF_NOTIFICATION_TYPE_CONTACT ,
	LOF_NOTIFICATION_TYPE_CONVERSATION ,
	LOF_NOTIFICATION_TYPE_REGISTRATION ,
	LOF_NOTIFICATION_TYPE_SYNCUSERINFO ,
	LOF_NOTIFICATION_TYPE_PGGROUP ,
	LOF_NOTIFICATION_TYPE_UNKNOWN
} LOF_TOOL_NotificationType;

typedef enum
{
	LOF_NOTIFICATION_EVENT_PRESENCECHANGED ,
	LOF_NOTIFICATION_EVENT_ADDBUDDYAPPLICATION ,
	LOF_NOTIFICATION_EVENT_USERLEFT ,
	LOF_NOTIFICATION_EVENT_DEREGISTRATION ,
	LOF_NOTIFICATION_EVENT_SYNCUSERINFO ,
	LOF_NOTIFICATION_EVENT_PGGETGROUPINFO ,
	LOF_NOTIFICATION_EVENT_UNKNOWN
} LOF_EVENT_NotificationEventType;

typedef enum
{
	LOF_SIP_EVENT_PRESENCE = 0,
	LOF_SIP_EVENT_SETPRESENCE ,
	LOF_SIP_EVENT_CONTACT ,
	LOF_SIP_EVENT_CONVERSATION ,
	LOF_SIP_EVENT_CATMESSAGE ,
	LOF_SIP_EVENT_SENDCATMESSAGE ,
	LOF_SIP_EVENT_STARTCHAT ,
	LOF_SIP_EVENT_INVITEBUDDY ,
	LOF_SIP_EVENT_GETCONTACTINFO ,
	LOF_SIP_EVENT_CREATEBUDDYLIST ,
	LOF_SIP_EVENT_DELETEBUDDYLIST ,
	LOF_SIP_EVENT_SETCONTACTINFO ,
	LOF_SIP_EVENT_SETUSERINFO ,
	LOF_SIP_EVENT_SETBUDDYLISTINFO ,
	LOF_SIP_EVENT_DELETEBUDDY ,
	LOF_SIP_EVENT_ADDBUDDY ,
	LOF_SIP_EVENT_KEEPALIVE ,
	LOF_SIP_EVENT_DIRECTSMS ,
	LOF_SIP_EVENT_SENDDIRECTCATSMS ,
	LOF_SIP_EVENT_HANDLECONTACTREQUEST ,
	LOF_SIP_EVENT_PGGETGROUPLIST ,
	LOF_SIP_EVENT_PGGETGROUPINFO ,
	LOF_SIP_EVENT_PGGETGROUPMEMBERS ,
	LOF_SIP_EVENT_PGSENDCATSMS ,
	LOF_SIP_EVENT_PGPRESENCE
} LOF_EVENT_SipEventType;

typedef enum
{
	LOF_INCOMING_NUDGE ,
	LOF_INCOMING_SHARE_CONTENT ,
	LOF_INCOMING_INPUT,
	LOF_INCOMING_UNKNOWN
} LOF_EVENT_IncomingType;

typedef enum
{
	LOF_INCOMING_ACTION_ACCEPT ,
	LOF_INCOMING_ACTION_CANCEL ,
	LOF_INCOMING_ACTION_UNKNOWN
} LOF_EVENT_IncomingActionType;

#endif /* LOF_TYPES_H_ */
