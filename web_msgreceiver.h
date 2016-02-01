#ifndef _WEB_MSGRECEIVER_H_
#define _WEB_MSGRECEIVER_H_

#include "framework.h"
#include "mysqloperate.h"
#include "msg_areasms_task_def.h"

class CWebMsgReceiver : public CMsgThread
{
public:
	CWebMsgReceiver();
	~CWebMsgReceiver();

private:	
	
	map<U32,U8>::iterator m_itWebTask;

	TaskPackageMsg *m_pTaskPackageMsg;
	CMysqlOperate m_MysqlOperate;
	IMsg *m_pTaskExecute;
	IMsg *m_pWebInterface;

	stringss m_result;
	
	virtual BOOL Prepare(long *pnResult);
	BOOL OnMsg(TRawMsg * msg,IMsg * remote_src);

	BOOL CreateWebTaskHandle(U64 TaskID);
	BOOL CloseWebTaskHandle(U64 TaskID);
	BOOL ExecuteAllWebTasks();
	BOOL WriteStatus(TaskStatusMsg* msg);
	BOOL GetBlackList();
};








































#endif//_WEB_MSGRECEIVER_H_
