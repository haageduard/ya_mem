#include <stddef.h>
#include <stdint.h>

#include "alloc.h"
#include "log.h"

#ifndef YA_MEM_ALLOC_POOL_SIZE
#error YA_MEM_ALLOC_POOL_SIZE is not defined
#endif

#ifndef YA_MEM_ALLOC_CHUNK_SIZE
#error YA_MEM_ALLOC_CHUNK_SIZE is not defined
#endif

#define YA_MEM_ALLOC_CHUNK_COUNT (YA_MEM_ALLOC_POOL_SIZE / YA_MEM_ALLOC_CHUNK_SIZE)
#define YA_MEM_ALLOC_BM_SIZE (YA_MEM_ALLOC_CHUNK_COUNT / 8)

static uint8_t mem_pool[YA_MEM_ALLOC_POOL_SIZE] = {
    0,
};

static uint8_t mem_pool_bm[YA_MEM_ALLOC_BM_SIZE] = {
    0,
};

static const size_t mem_alloc_pool_size = YA_MEM_ALLOC_POOL_SIZE;
static const size_t mem_alloc_chunk_size = YA_MEM_ALLOC_CHUNK_SIZE;
static const size_t mem_alloc_chunk_count = YA_MEM_ALLOC_CHUNK_COUNT;
static const size_t mem_alloc_bm_size = YA_MEM_ALLOC_BM_SIZE;

static void *get_next_chunk(ya_mem_pool_alloc_t *mp)
{
  void *ptr = NULL;
  uint8_t n = 0;
  for (size_t i = 0; i < mem_alloc_bm_size; i++)
  {
    n = mem_pool_bm[i];
    if (n ^ 0xff)
    {
      for (uint8_t j = 0; j < 8; j++)
      {
        if (!(n & 0x80))
        {
          mem_pool_bm[i] |= (1 << (7 - j));
          ptr = mp->first + (i * 8 + j) * mem_alloc_chunk_size;
          return ptr;
        }
        n = n << 1;
      }
    }
  }
  return NULL;
}

static inline void mutex_lock(ya_mem_sm_t *sm)
{
#if defined(YA_MEM_ALLOC_USE_RTOS)
  while (xSemaphoreTake(sm, portMAX_DELAY) != pdTRUE)
  {
  };
#else
  while (sm->lock == 1)
  {
  };
#endif
}

static inline void mutex_unlock(ya_mem_sm_t *sm)
{
#if defined(YA_MEM_ALLOC_USE_RTOS)
  xSemaphoreGive(sm);
#else
  sm->lock = 0;
#endif
}

ya_mem_err_t ya_mem_pool_init(ya_mem_pool_alloc_t *mp)
{
  if (mp == NULL)
  {
    return YA_MEM_ERR_FAIL;
  }
  if (mem_alloc_pool_size == 0)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_init: zero pool size\n");
#endif
    return YA_MEM_ERR_FAIL;
  }
#if defined(YA_MEM_ALLOC_USE_RTOS)
  mp->sm = xSemaphoreCreateMutex();
  if (mp->sm == NULL)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_init: mutex init error\n");
#endif
    return YA_MEM_ERR_FAIL;
  }
#else
  mp->sm.lock = 0;
#endif
  mp->first = (void *)&mem_pool[0];
  mp->last = (void *)&mem_pool[(mem_alloc_chunk_count - 1) * mem_alloc_chunk_size];
  return YA_MEM_ERR_OK;
}

ya_mem_err_t ya_mem_pool_alloc(ya_mem_pool_alloc_t *mp, void **ptr)
{
  void *p = NULL;
  if (mp == NULL)
  {
    return YA_MEM_ERR_FAIL;
  }
  if (mp->first == NULL)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_alloc: first is NULL (library not initialized?)\n");
#endif
    return YA_MEM_ERR_INIT;
  }
  if (mp->last == NULL)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_alloc: last is NULL (library not initialized?)\n");
#endif
    return YA_MEM_ERR_INIT;
  }
  mutex_lock(&mp->sm);
  p = get_next_chunk(mp);
  if (p == NULL)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_alloc: p is NULL (out of memory)\n");
#endif
    mutex_unlock(&mp->sm);
    return YA_MEM_ERR_FAIL;
  }
  *ptr = p;
  mutex_unlock(&mp->sm);
  return YA_MEM_ERR_OK;
}

ya_mem_err_t ya_mem_pool_free(ya_mem_pool_alloc_t *mp, void *ptr)
{
  if (mp == NULL)
  {
    return YA_MEM_ERR_FAIL;
  }
  if (mp->first == NULL)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_free: first is NULL (library not initialized?)\n");
#endif
    return YA_MEM_ERR_INIT;
  }
  if (mp->last == NULL)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_free: last is NULL (library not initialized?)\n");
#endif
    return YA_MEM_ERR_INIT;
  }
  if (ptr == NULL)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_free: ptr is NULL\n");
#endif
    return YA_MEM_ERR_FAIL;
  }
  // check ptr is valid block
  if (!(ptr >= mp->first && ptr <= mp->last + mem_alloc_chunk_size))
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_free: ptr is not valid pointer\n");
#endif
    return YA_MEM_ERR_FAIL;
  }
  // check ptr is correct aligned block adress
  if ((uintptr_t)ptr % mem_alloc_chunk_size > 0)
  {
#ifdef YA_MEM_DEBUG
    YA_MEM_LOG("ya_mem_pool_free: ptr is not aligned pointer\n");
#endif
    return YA_MEM_ERR_FAIL;
  }
  mutex_lock(&mp->sm);
  const size_t bl_n = (ptr - mp->first) / mem_alloc_chunk_size;
  const size_t bm_n = bl_n / 8;
  const size_t bm_bn = bl_n % 8;
  mem_pool_bm[bm_n] &= ~(1 << (7 - bm_bn));
  mutex_unlock(&mp->sm);
  return YA_MEM_ERR_OK;
}

const size_t ya_mem_pool_chunk_count(void)
{
  return mem_alloc_chunk_count;
}

const size_t ya_mem_pool_chunk_size(void)
{
  return mem_alloc_chunk_size;
}

const size_t ya_mem_pool_bm_size(void)
{
  return mem_alloc_bm_size;
}