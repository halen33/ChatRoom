#pragma once
#define SERVER_IP "127.0.0.1"

// server port
#define SERVER_PORT 8888

//epoll size
#define EPOLL_SIZE 5000
//szbuf
#define _DEF_BUFFERSIZE  1024
//message buffer size
#define BUF_SIZE 0xFFFF
//消息
#define UM_ONLINEMSG   WM_USER + 1
#define UM_OFFLINEMSG  WM_USER + 2
#define UM_DATAINFOMSG  WM_USER + 3
#define UM_DATAINFOMSG_O   WM_USER + 4


//协议

#define _DEF_PROTOCOL_BASE  100

#define _DEF_PROTOCOL_ONLINE_RQ     _DEF_PROTOCOL_BASE  +1
#define _DEF_PROTOCOL_ONLINE_RS     _DEF_PROTOCOL_BASE  +2

#define _DEF_PROTOCOL_OFFLINE_RQ     _DEF_PROTOCOL_BASE  +3


#define _DEF_PROTOCOL_DATAINFO_RQ    _DEF_PROTOCOL_BASE  +4
#define _DEF_PROTOCOL_HEAART_RQ _DEF_PROTOCOL_BASE  +5

//协议包
typedef char PackdefType;
//上线请求 上线回复 下线请求

typedef struct HEART
{
	PackdefType m_nType;
 };

typedef struct STRU_ONLINE
{
	PackdefType m_nType;
	int clientfd;
	int rsid;

}STRU_OFFLINE;


struct STRU_DATAINFO
{
	PackdefType m_nType;
	int type;//0 私聊 1 群聊
	int fromid;
	int  toid;
	char        m_szBuf[_DEF_BUFFERSIZE];
	int         m_nNum;  //序号
};

