include ../app_make_inc_1


MYSQL_LIB	=	-L "/usr/lib64/mysql"

TARGET		=	areasms_taskdispatch	
OBJECTS		= 	taskdispatch_app.o	\
				web_msgreceiver.o	\
				mysqloperate.o		
				
LINKFLAGS +=	$(MYSQL_LIB) -lmysqlclient $(MYSQL_LIB) -lmysqlclient_r -lz -lm

include ../app_make_inc_2
