#include <stdio.h>
#include <stdarg.h>

#include "log.h"

void ya_mem_log(const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
}
