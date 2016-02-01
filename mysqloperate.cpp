#include "mysqloperate.h"


extern stru_MysqlConfig g_mysqladdr;

CMysqlOperate::CMysqlOperate()
{
	m_iTimes = 0;
	m_pmysql = 0;
	memset(&m_localtime,0,sizeof(TUTime));
}

CMysqlOperate::~CMysqlOperate()
{
	closeMysqlConnect();
	mysql_library_end();	
	if( NULL != m_mysqlconf.query)
	{	
		delete[] m_mysqlconf.query;
	}
	TRACE_LOG( "~CCheckDB().");
}

void CMysqlOperate::SetMysqlConf(stru_MysqlConfig *mysqlconf)
{
	if( NULL != m_mysqlconf.query)
	{	
		delete[] m_mysqlconf.query;
	}
	memset(&m_mysqlconf,0,sizeof(stru_MysqlConfig));
	m_mysqlconf.local0_or_remote1_db = g_mysqladdr.local0_or_remote1_db;
	strcpy(m_mysqlconf.mysql_db_ip,g_mysqladdr.mysql_db_ip);
	strcpy(m_mysqlconf.mysql_db_name,g_mysqladdr.mysql_db_name);
	strcpy(m_mysqlconf.mysql_passwd,g_mysqladdr.mysql_passwd);
	strcpy(m_mysqlconf.mysql_username,g_mysqladdr.mysql_username);
	m_mysqlconf.mysql_port = g_mysqladdr.mysql_port;
	
}

void CMysqlOperate::SetQueryHandle(char *query)
{
	if( NULL != m_mysqlconf.query)
	{	
		delete[] m_mysqlconf.query;
	}
	m_mysqlconf.query = (char *)new char[strlen(query)+1];
	memset(m_mysqlconf.query,0,strlen(query)+1);
	memcpy(m_mysqlconf.query,query,strlen(query));
}


BOOL CMysqlOperate::init_db()
{
	if ( NULL != m_pmysql )
	{
		//TRACE_LOG( "[_Debug]: NULL != m_pmysql,first execute closeMysqlConnect()." );
		closeMysqlConnect();
	}
	

	if ( FALSE == mysql_database_init() ) 
	{   
		TRACE_LOG( "mysql_library_init() [failure]");
		return FALSE;
	}


	if ( NULL == (m_pmysql = mysql_init( NULL )) )
	{	//failure
		TRACE_LOG( "mysql_init() failure.");
		return FALSE;
	}	
	///connect Remote Mysql Db
	//TRACE_LOG("[Debug]: the open mysql ip:%s",mysqlconf->mysql_db_ip);
	if(NULL == mysql_real_connect(m_pmysql, 
								m_mysqlconf.mysql_db_ip, 
								m_mysqlconf.mysql_username,
								m_mysqlconf.mysql_passwd, 
								m_mysqlconf.mysql_db_name,
								m_mysqlconf.mysql_port,
								NULL, 0) )
	{//FAILURE
		TRACE_LOG( "database connect : %s", mysql_error(m_pmysql) );
		return FALSE;
	}
	else
	{//SUCCESS
		//TRACE_LOG( "[checkdb]: database connect: [success]." );
		return TRUE;
	}
}

/* 得到当前微秒级的时间 */
/*
I64 CMysqlManage::TUGetTime(void)
{
	struct timezone tz;
	timeval tv;
	gettimeofday(&tv, &tz);
	return ((I64)tv.tv_sec - tz.tz_minuteswest * 60) * 1000000 + tv.tv_usec;
}
*/

void CMysqlOperate::closeMysqlConnect(void) 
{
	if ( NULL != m_pmysql ) 
	{
		mysql_close( m_pmysql);
		m_pmysql = NULL;
		//TRACE_LOG( "mysql_close()." );
	}	
}

BOOL CMysqlOperate::mysql_database_init(void)
{
	static char *server_args[] = {
		"this_program", 
		"--datadir=.", 
		"--key_buffer_size=32M"
	};

	static char *server_groups[] = {
		"embedded",
		"server",
		"this_program_SERVER",
		(char *)0
	};
	
	if (1 == mysql_library_init(sizeof(server_args) / sizeof(char *), server_args, server_groups))
	{ // failure
		return FALSE;
	}
	return TRUE;
}


BOOL CMysqlOperate::GetMysqlData(stringss *result)
{
	if (NULL == m_mysqlconf.mysql_db_name)
	{
		TRACE_LOG("[Error]: NULL == m_mysqlconf");
		return FALSE;
	}
	if( FALSE == init_db() )
	{
		TRACE_LOG("[Error]: the m_mysqlconf can't connect !");
		return FALSE;
	}
	
	MYSQL_RES		*res = NULL;
	MYSQL_ROW		row;
	U32				num_fields;
	U32				num_column;
	U32				num_rows;
	strings			StrsCom;
	string 			strNULL;
	
	//I64				iCurtime = 0;
	//iCurtime = TUGetTime();
	//TUGetTime(m_localtime);
	stringss::iterator itStringss = result->begin();
	while(itStringss != result->end())
	{
		itStringss->clear();
		itStringss++;
	}
	result->clear();
	TRACE_LOG("[_Debug]: the query word:%s",m_mysqlconf.query);
	if ( 0 !=  mysql_real_query(m_pmysql, m_mysqlconf.query, strlen(m_mysqlconf.query) + 1 ) ) 
	{	//failure
		TRACE_LOG("[error]: mysql_real_query: %s, %d",  mysql_error(m_pmysql), mysql_errno(m_pmysql) );
		return FALSE;
	}	

	if ( NULL == ( res = mysql_store_result(m_pmysql)) )
	{
		if (0 == mysql_errno(m_pmysql) )
		{	// no data, success // not process
			TRACE_LOG( "[error]: dropManyIndexAndTable [OK] no data : [%s]", m_mysqlconf.query );
			return TRUE;  
		}
		else
		{	//failure
			TRACE_LOG( "[Error]: %s",  mysql_error(m_pmysql) );
			return FALSE;
		}
	}		

	num_fields = mysql_num_fields(res); // table column's number
	if ( 1 > num_fields )
		{// failure
		TRACE_LOG( "[Error]: logical error!" );
		return FALSE;
		}
	TRACE_LOG("[_Debug]: the result sizes:%d",num_fields);
	num_rows = 0;
	while ( NULL !=  (row = mysql_fetch_row(res)) )//get the next line from the result buffer memory,
	{
		num_column = 0;
		result->push_back(StrsCom);
		//printf("the mysql result[%u]:",num_rows);
		while(num_fields > num_column)//get the row value from the line buffer memory
		{
			//printf("%s ",row[num_column]);
			if(NULL == row[num_column]){
				result->at(num_rows).push_back(strNULL);
			}else	
				result->at(num_rows).push_back(row[num_column]);
			num_column++;
		}
		//printf("\n");
		num_rows++;
	}
	if (NULL != res)
	{
		mysql_free_result(res); // Free Memory
		res = NULL;
	}
	TRACE_LOG("[_Debug]: Mysql query finish!");
	stringss::iterator itstringss;
	strings::iterator itstrings;
	U32 wTestNum = 0;
	itstringss = result->begin();
	printf("@ [2015-12-17 10:54:34.563] [_Debug]: ");
	while(itstringss != result->end())
	{
		itstrings = itstringss->begin();
		while(itstringss->end() != itstrings)
		{
			printf("[%u]:%s ",wTestNum++,itstrings->c_str());
			itstrings++;
		}
		printf("\n");wTestNum = 0;
		itstringss++;
	}
	printf("\n");
	return TRUE;	
}

BOOL CMysqlOperate::InsertIntoMysqlData()
{
	if (NULL == m_mysqlconf.mysql_db_name)
	{
		TRACE_LOG("NULL == m_mysqlconf");
		return FALSE;
	}
	if( FALSE == init_db() )
	{
		TRACE_LOG("the m_mysqlconf can't connect !");
		return FALSE;
	}
	
	MYSQL_RES		*res = NULL;
	MYSQL_ROW		row;
	U32				num_fields;
	U32				num_column;
	U32				num_rows;
	
	//I64				iCurtime = 0;
	//iCurtime = TUGetTime();
	//TUGetTime(m_localtime);
	TRACE_LOG("[_Debug]: the query word:%s",m_mysqlconf.query);
	if ( 0 !=  mysql_real_query(m_pmysql, m_mysqlconf.query, strlen(m_mysqlconf.query) + 1 ) ) 
	{	//failure
		TRACE_LOG("[error]: mysql_real_query: %s, %d",  mysql_error(m_pmysql), mysql_errno(m_pmysql) );
		return FALSE;
	}	
	/*

	if ( NULL == ( res = mysql_store_result(m_pmysql)) )
	{
		if (0 == mysql_errno(m_pmysql) )
		{	// no data, success // not process
			TRACE_LOG( "[error]: dropManyIndexAndTable [OK] no data : [%s]", m_mysqlconf.query );
			return TRUE;  
		}
		else
		{	//failure
			TRACE_LOG( "%s",  mysql_error(m_pmysql) );
			return FALSE;
		}
	}		

	num_fields = mysql_num_fields(res); // table column's number
	if ( 1 > num_fields )
		{// failure
		TRACE_LOG( "logical error!" );
		return FALSE;
		}
	TRACE_LOG("the result sizes:%d",num_fields);
	num_column = 0;
	while ( NULL !=  (row = mysql_fetch_row(res)) )//get the next line from the result buffer memory,
	{
		num_rows = 0;
		printf("the mysql result[%u]:",num_rows);
		while(num_fields > num_rows)//get the row value from the line buffer memory
		{
			//printf("%s ",row[num_rows]);
			m_mysqlconf.result[num_column].append(row[num_rows]);
			m_mysqlconf.result[num_column].append(" ");
			num_rows++;
		}
		//printf("\n");
		num_column++;
	}
	if (NULL != res)
	{
		mysql_free_result(res); // Free Memory
		res = NULL;
	}
	*/
	return TRUE;	
}




