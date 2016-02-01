#ifndef _MYSQLOPERATE_H_
#define _MYSQLOPERATE_H_


#include <framework.h>
#include "msg_areasms_task_def.h"

#include <cassert>

const I32 CONFARRAYLEN = 64;

typedef struct _stru_mysql_configuretion {
	U16   	local0_or_remote1_db; 
	I8		mysql_db_ip[CONFARRAYLEN];
	I8		mysql_username[CONFARRAYLEN];
	I8		mysql_passwd[CONFARRAYLEN];
	I8		mysql_db_name[CONFARRAYLEN];
	U16    	mysql_port;
	I8		*query;
	strings result;
}stru_MysqlConfig;


typedef vector<strings> stringss;


extern "C"
{
#include "../inc/mysql.h"
#include <sys/time.h>
#include <errno.h>
}


/************************************************************************
检查数据库数据是否正常                           

************************************************************************/

class CMysqlOperate 
{
public:
	CMysqlOperate();
	~CMysqlOperate();


public:
				
		
		/* 得到当前微秒级的时间 */
		//I64 TUGetTime(void);
		void 	SetMysqlConf(stru_MysqlConfig *mysqlconf);
		void 	SetQueryHandle(char *query);
		BOOL	GetMysqlData(stringss *result);
		BOOL 	InsertIntoMysqlData();
		void	closeMysqlConnect(void);
		
private:
	
		/* init Poiter: m_pmysql, make connect to Db */
		BOOL	init_db();		
		BOOL	mysql_database_init(void);

		U8 		m_iTimes;

		
		
		MYSQL*				m_pmysql;
		TUTime				m_localtime;
		stru_MysqlConfig 	m_mysqlconf;
	
};







#endif//_MYSQLOPERATE_H_






