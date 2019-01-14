/*
 * Created by Anton Zhigaylo <antoooon@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the MIT License
 */

#ifndef _SLOG_H_
#define _SLOG_H_

//------------------------------------------------------------------------------
#define MSG_ERROR    0
#define MSG_WARNING  1
#define MSG_INFO     2
#define MSG_DBG      3

#define TRACE_SINK_CONSOLE 0
#define TRACE_SINK_DLT     1

#define TRACE_SINK TRACE_SINK_DLT
//------------------------------------------------------------------------------
/**
 * set debug level
 * @param debug level
 */
void setDbgLevel(int lvl);

/**
 * set debug level
 * @param debug level
 */
void setDbgSink(int sink);

/**
 * get debug level
 */
int getDbgSink();

/**
 * init DLT trace component
 */
void initDlt();

/**
 * deinit DLT trace component
 */
void deinitDlt();

/**
 * print error
 * @param *fmt format string
 */
void printError(const char *fmt, ...);

/**
 * print warning
 * @param *fmt format string
 */
void printWarning(const char *fmt, ...);

/**
 * print info
 * @param *fmt format string
 */
void printInfo(const char *fmt, ...);

/**
 * print info
 * @param *fmt format string
 */
void printDebug(const char * const fmt, ...);

#endif // _SLOG_H_
