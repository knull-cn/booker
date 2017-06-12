
#ifndef _K_LOG_H__    
#define _K_LOG_H__

#ifdef _KDEBUG_
#include <cstdio>
#include <cstring>
#include <cerrno>
extern int errno;
#endif//_KDEBUG_


namespace KBASE
{

//#define _QDEBUG_
#ifdef _KDEBUG_
//#include <cstdlib>

#define fplog stdout
//SYSTEM ERROR;
#define LOGABORT(fmt,...)  do{fprintf(fplog,"  ABORT |");fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");abort();}while(0)
#define LOGSYSERR(fmt,...) do{fprintf(fplog," SYSERR |");fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");abort();}while(0)
#define LOGICERR(fmt,...)  do{fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0)
//#define LOGICERRNO LOGICERR
#define LOGICERRNO(fmt,...) \
do{\
    fprintf(fplog,fmt,##__VA_ARGS__);\
    fprintf(fplog,"(%d):%s\n",errno,strerror(errno));\
}while(false)

//business error;
#define LOGERROR(fmt,...) do{fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0)
#define LOGWARN(fmt,...) do{ fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0)
#define LOGDEBUG(fmt,...) do{fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0)
//business info;
#define LOGINFO(fmt,...) do{ fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0)
//LIBTEST-info;
#define LLIBERROR  LOGERROR
#define LLIBDEBUG(fmt,...) do{fprintf(fplog,fmt,##__VA_ARGS__);fprintf(fplog,"\n");}while(0)

#else
#error "not define _KDEBUG_"
#endif//_KDEBUG_

}

#endif//_K_LOG_H__
