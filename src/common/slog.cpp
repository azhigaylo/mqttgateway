//------------------------------------------------------------------------------
#include <stdarg.h>
#include <inttypes.h>
#include <syslog.h>
#include <cstdio>
#include <string.h>
#include <iostream>

#include "common/slog.h"
#include "dlt/dlt.h"

//-------------------------------------------------------------------------------
using namespace std;
//-------------------------------------------------------------------------------

static volatile int32_t optv = MSG_ERROR;
static volatile int32_t dbg_sink = TRACE_SINK_CONSOLE;

DLT_DECLARE_CONTEXT(dlt_context);

//---------------------------------CONSOLE---------------------------------------

void printErrorConsole(const char *msg)
{
    printf("%s \n", msg);
}

void printWarningConsole(const char *msg)
{
   if (optv > 0)
   {
      printf("%s \n", msg);
   }
}

void printInfoConsole(const char *msg)
{
   if (optv > 1)
   {
      printf("%s \n", msg);
   }
}

void printDebugConsole(const char *msg)
{
   if (optv > 2)
   {
      printf("%s \n", msg);
   }
}

//---------------------------------DLT-------------------------------------------

void printErrorDlt(const char *msg)
{
   DLT_LOG(dlt_context, DLT_LOG_ERROR, DLT_STRING(msg));
}

void printWarningDlt(const char *msg)
{
   if (optv > 0)
   {
      DLT_LOG(dlt_context, DLT_LOG_WARN, DLT_STRING(msg));
   }
}

void printInfoDlt(const char *msg)
{
   if (optv > 1)
   {
      DLT_LOG(dlt_context, DLT_LOG_INFO, DLT_STRING(msg));
   }
}

void printDebugDlt(const char *msg)
{
   if (optv > 2)
   {
      DLT_LOG(dlt_context, DLT_LOG_DEBUG, DLT_STRING(msg));
   }
}

//------------------------------------------------------------------------------

void setDbgLevel(int lvl)
{
   optv = lvl;
}

void setDbgSink(int sink)
{
   dbg_sink = sink;
}

int getDbgSink()
{
   return dbg_sink;
}

void initDlt()
{
   DLT_REGISTER_APP("HB","HomeBrain trace");
   DLT_REGISTER_CONTEXT(dlt_context,"HB_CTX","Main context");

   usleep(10000);
}

void deinitDlt()
{
    DLT_UNREGISTER_CONTEXT(dlt_context);

    DLT_UNREGISTER_APP();
}

void printError(const char * const fmt,...)
{
   // get string from va list
   va_list vargs;
   char data[5000];

   static pthread_mutex_t cs_mutex =  PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock( &cs_mutex );

   va_start( vargs, fmt );
   vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
   va_end( vargs );

   // send to sink
   switch (dbg_sink)
   {
      case TRACE_SINK_DLT :
      {
         printErrorDlt( data );
         break;
      }
      case TRACE_SINK_CONSOLE :
      default:
      {
         printErrorConsole( data );
         break;
      }
   }
   pthread_mutex_unlock( &cs_mutex );
}

void printWarning(const char * const fmt, ...)
{
   // get string from va list
   va_list vargs;
   char data[5000];

   static pthread_mutex_t cs_mutex =  PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock( &cs_mutex );

   va_start( vargs, fmt );
   vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
   va_end( vargs );

   // send to sink
   switch (dbg_sink)
   {
      case TRACE_SINK_DLT :
      {
         printWarningDlt( data );
         break;
      }
      case TRACE_SINK_CONSOLE :
      default:
      {
         printWarningConsole( data );
         break;
      }
   }
   pthread_mutex_unlock( &cs_mutex );
}

void printInfo(const char * const fmt, ...)
{
   // get string from va list
   va_list vargs;
   char data[5000];

   static pthread_mutex_t cs_mutex =  PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock( &cs_mutex );

   va_start( vargs, fmt );
   vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
   va_end( vargs );
   // send to sink
   switch (dbg_sink)
   {
      case TRACE_SINK_DLT :
      {
         printInfoDlt( data );
         break;
      }
      case TRACE_SINK_CONSOLE :
      default:
      {
         printInfoConsole( data );
         break;
      }
   }
   pthread_mutex_unlock( &cs_mutex );
}

void printDebug(const char * const fmt, ...)
{
   // get string from va list
   va_list vargs;
   char data[5000];

   static pthread_mutex_t cs_mutex =  PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_lock( &cs_mutex );

   va_start( vargs, fmt );
   vsnprintf(data,((sizeof(data)) - 1), fmt, vargs);
   va_end( vargs );
   // send to sink
   switch (dbg_sink)
   {
      case TRACE_SINK_DLT :
      {
         printDebugDlt( data );
         break;
      }
      case TRACE_SINK_CONSOLE :
      default:
      {
         printDebugConsole( data );
         break;
      }
   }
   pthread_mutex_unlock( &cs_mutex );
}

