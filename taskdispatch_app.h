#ifndef __AREASMS_TASKDISPATCH_APP_H__
#define __AREASMS_TASKDISPATCH_APP_H__

#include <framework.h>
#include "msg_areasms_task_def.h"


#define ELEM_AGENT_WEB_INTERFACE_SERVER 		"webinterf_server"
#define ELEM_AGENT_TASK_EXECUTE_CENTER			"taskdispatch"








class CTaskDispatch : public CApplication
{
public:
	CTaskDispatch();
	~CTaskDispatch();
protected:
	
	virtual void GetCfgSubscribeInfo(
				CCfgPathPairList &paths, CCfgDynamicSubscriberList &dynamic_nodes);
	
	virtual BOOL Initialize();
	virtual BOOL Running();
	virtual BOOL Terminate();		
	
};













































#endif//__AREASMS_TASKDISPATCH_APP_H__
