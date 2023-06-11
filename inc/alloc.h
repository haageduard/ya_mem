#ifndef __YA_MEM_ALLOC_H__
#define __YA_MEM_ALLOC_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

  // #define YA_MEM_ALLOC_USE_RTOS
  // #define YA_MEM_DEBUG
  // #define YA_MEM_ALLOC_POOL_SIZE (16384)
  // #define YA_MEM_ALLOC_CHUNK_SIZE (16)

#if defined(YA_MEM_ALLOC_USE_RTOS)
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/semphr.h"
#endif

#define YA_MEM_ERR_OK (0)
#define YA_MEM_ERR_FAIL (1)
#define YA_MEM_ERR_INIT (2)

#if defined(YA_MEM_ALLOC_USE_RTOS)
  typedef SemaphoreHandle_t ya_mem_sm_t;
#else
typedef struct
{
  uint8_t lock;
} ya_mem_sm_t;
#endif

  typedef struct
  {
    void *first;
    void *last;
    ya_mem_sm_t sm;
  } ya_mem_pool_alloc_t;

  typedef uint8_t ya_mem_err_t;

  ya_mem_err_t ya_mem_pool_init(ya_mem_pool_alloc_t *mp);
  ya_mem_err_t ya_mem_pool_alloc(ya_mem_pool_alloc_t *mp, void **ptr);
  ya_mem_err_t ya_mem_pool_free(ya_mem_pool_alloc_t *mp, void *ptr);
  const size_t ya_mem_pool_chunk_count(void);
  const size_t ya_mem_pool_chunk_size(void);

#ifdef __cplusplus
}
#endif

#endif