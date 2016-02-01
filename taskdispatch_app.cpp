#include "taskdispatch_app.h"
#include "web_msgreceiver.h"
#include "mysqloperate.h"


CWebMsgReceiver g_WebMsgReceiverThread;

CThreadLock g_TaskMapLock;
stru_MysqlConfig g_mysqladdr;




IMPLEMENT_APPLICATION(CTaskDispatch,"taskdispatch")




CTaskDispatch::CTaskDispatch()
{
	;
}

CTaskDispatch::~CTaskDispatch()
{
	;
}

void CTaskDispatch::GetCfgSubscribeInfo(
		CCfgPathPairList &paths, CCfgDynamicSubscriberList &dynamic_nodes)

{
	CApplication::GetCfgSubscribeInfo(paths,dynamic_nodes);
}
	
BOOL CTaskDispatch::Initialize()
{
	LoadRemoteAgents();
	CXMLElement* elem_mysql = GetConfig().GetElement((string(CFG_APP_ELEM_NAME) + "/mysql").c_str());
	if(elem_mysql)
	{
		CXMLElement* elem_mysqlconf;
		elem_mysqlconf = elem_mysql->GetElements().Find("mysql_ip_addr");
		if(elem_mysqlconf)
			strcpy(g_mysqladdr.mysql_db_ip,elem_mysqlconf->GetText() );
		
		elem_mysqlconf = elem_mysql->GetElements().Find("local0_or_remote1_db");
		if(elem_mysqlconf)
			g_mysqladdr.local0_or_remote1_db = atoi(elem_mysqlconf->GetText());
		
		elem_mysqlconf = elem_mysql->GetElements().Find("mysql_username");
		if(elem_mysqlconf)
			strcpy(g_mysqladdr.mysql_username,elem_mysqlconf->GetText() );
		
		elem_mysqlconf = elem_mysql->GetElements().Find("mysql_passwd");
		if(elem_mysqlconf)
			strcpy(g_mysqladdr.mysql_passwd,elem_mysqlconf->GetText() );
		
		elem_mysqlconf = elem_mysql->GetElements().Find("mysql_db_name");
		if(elem_mysqlconf)
			strcpy(g_mysqladdr.mysql_db_name,elem_mysqlconf->GetText() );
		
		elem_mysqlconf = elem_mysql->GetElements().Find("mysql_port");
		if(elem_mysqlconf)
			g_mysqladdr.mysql_port = atoi(elem_mysqlconf->GetText());
		//printf("mysql_db_ip:%s\nmysql_username:%s\nmysql_passwd:%s\nmysql_db_name:%s\nmysql_port:%u\n",g_mysqladdr.mysql_db_ip
		//,g_mysqladdr.mysql_username,g_mysqladdr.mysql_passwd,g_mysqladdr.mysql_db_name,g_mysqladdr.mysql_port);
	}
	
	CRemoteMsgAgent* pWebRecAgent = GetRemoteAgent(ELEM_AGENT_WEB_INTERFACE_SERVER);
	CRemoteMsgAgent* pTaskSendAgent = GetRemoteAgent(ELEM_AGENT_TASK_EXECUTE_CENTER);
	if( (NULL != pWebRecAgent) && (NULL != pTaskSendAgent) )
	{
		pWebRecAgent->SetTarget(&g_WebMsgReceiverThread);
		pTaskSendAgent->SetTarget(&g_WebMsgReceiverThread);
		
		g_WebMsgReceiverThread.Start();
		OSSleep(100);

		if( !pWebRecAgent->Start() )
		{
			TRACE_LOG("the web agent start is Failure !");
			return FALSE;
		}
		if( !pTaskSendAgent->Start() )
		{
			TRACE_LOG("the task agent start is failure !");
			return FALSE;
		}		
	}
	OSSleep(1000);
	TRACE_LOG("the Initialize is success !");
	return TRUE;
}
BOOL CTaskDispatch::Running()
{
	I8 buffer[1024];
	I8 bufWebCommand[256] = "RULENO,1449034216798,CREATE";
	char * pchar;
	memset(buffer,0,1024);
	TRawMsg* pRequestBlacklistMsg = (TRawMsg*)buffer;
	memcpy(pRequestBlacklistMsg->_param,bufWebCommand,strlen(bufWebCommand));
	pRequestBlacklistMsg->type = MSG_TYPE_WEB_TASK;
	pRequestBlacklistMsg->len = sizeof(TRawMsg) + strlen(bufWebCommand) + 1;
	g_WebMsgReceiverThread.SendMsg(pRequestBlacklistMsg);

	
	TRACE_LOG("over !  ");
	//string strtest("123");
	//TRACE_LOG("strtest:%s",strtest.c_str());
	return TRUE;
}
BOOL CTaskDispatch::Terminate()
{
	return TRUE;
}

CTaskDispatch* GetTaskDispatchApp()
{
	return (CTaskDispatch*)GetApplication();
}









































