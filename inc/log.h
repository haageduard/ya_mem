#ifndef __YA_MEM_LOG_H__
#define __YA_MEM_LOG_H__

#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define YA_MEM_LOG ya_mem_log

  void ya_mem_log(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif