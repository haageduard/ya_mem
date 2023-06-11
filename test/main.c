#include <stdlib.h>
#include <stdio.h>

/* ya_mem */
#include "alloc.h"
#include "log.h"

ya_mem_pool_alloc_t mp = {
    0,
};
size_t chunk_count;
void **chunks;

typedef struct
{
  char *title;
  int (*fn)(void);
} test_meta_t;

#define TEST_ERR_OK (1)
#define TEST_ERR_FAIL (2)

int test_init(void)
{
  ya_mem_err_t err;
  err = ya_mem_pool_init(&mp);
  if (err != YA_MEM_ERR_OK)
  {
    YA_MEM_LOG("ya_mem_pool_init error\n");
    return TEST_ERR_FAIL;
  }
  return TEST_ERR_OK;
}

int test_alloc(void)
{
  ya_mem_err_t err;
  void *ptr;
  for (size_t i = 0; i < chunk_count; i++)
  {
    if (err = ya_mem_pool_alloc(&mp, &ptr) != YA_MEM_ERR_OK)
    {
      YA_MEM_LOG("ya_mem_pool_alloc error\n");
      return TEST_ERR_FAIL;
    }
    chunks[i] = ptr;
  }
  return TEST_ERR_OK;
}

int test_free(void)
{
  ya_mem_err_t err;
  void *ptr;
  for (size_t i = 0; i < chunk_count; i++)
  {
    ptr = chunks[i];
    if (err = ya_mem_pool_free(&mp, ptr) != YA_MEM_ERR_OK)
    {
      YA_MEM_LOG("ya_mem_pool_free error\n");
      return TEST_ERR_FAIL;
    }
  }
  return TEST_ERR_OK;
}

int test_alloc_without_init(void)
{
  ya_mem_err_t err;
  void *ptr;
  err = ya_mem_pool_alloc(&mp, &ptr);
  if (err == YA_MEM_ERR_INIT)
  {
    return TEST_ERR_OK;
  }
  return TEST_ERR_FAIL;
}

int test_free_without_init(void)
{
  ya_mem_err_t err;
  err = ya_mem_pool_free(&mp, NULL);
  if (err == YA_MEM_ERR_INIT)
  {
    return TEST_ERR_OK;
  }
  return TEST_ERR_FAIL;
}

int test_free_invalid_pointer(void)
{
  ya_mem_err_t err;
  void *ptr;
  uint8_t n = 1;
  ptr = (void *)&n;
  err = ya_mem_pool_free(&mp, ptr);
  if (err == YA_MEM_ERR_FAIL)
  {
    return TEST_ERR_OK;
  }
  return TEST_ERR_FAIL;
}

int test_not_aligned_pointer(void)
{
  ya_mem_err_t err;
  void *ptr = chunks[0];
  ptr++;
  err = ya_mem_pool_free(&mp, ptr);
  if (err == YA_MEM_ERR_FAIL)
  {
    return TEST_ERR_OK;
  }
  return TEST_ERR_FAIL;
}

int test_alloc_free(void)
{
  ya_mem_err_t err;
  void *ptr1, *ptr2, *ptr3, *ptr4;
  err = ya_mem_pool_alloc(&mp, &ptr1);
  if (err != YA_MEM_ERR_OK)
  {
    return TEST_ERR_FAIL;
  }
  err = ya_mem_pool_alloc(&mp, &ptr2);
  if (err != YA_MEM_ERR_OK)
  {
    return TEST_ERR_FAIL;
  }
  err = ya_mem_pool_free(&mp, ptr1);
  if (err != YA_MEM_ERR_OK)
  {
    return TEST_ERR_FAIL;
  }
  err = ya_mem_pool_alloc(&mp, &ptr3);
  if (err != YA_MEM_ERR_OK)
  {
    return TEST_ERR_FAIL;
  }
  if (ptr1 != ptr3)
  {
    return TEST_ERR_FAIL;
  }
  err = ya_mem_pool_alloc(&mp, &ptr4);
  if (err != YA_MEM_ERR_OK)
  {
    return TEST_ERR_FAIL;
  }
  if (ptr4 != ptr2 + ya_mem_pool_chunk_size())
  {
    return TEST_ERR_FAIL;
  }
  return TEST_ERR_OK;
}

int main(int argc, void *argv)
{
  test_meta_t test[] = {
      {
          "test_alloc_without_init",
          test_alloc_without_init,
      },
      {
          "test_free_without_init",
          test_free_without_init,
      },
      {
          "init",
          test_init,
      },
      {
          "alloc",
          test_alloc,
      },
      {
          "free",
          test_free,
      },
      {
          "free_invalid_pointer",
          test_free_invalid_pointer,
      },
      {
          "not_aligned_pointer",
          test_not_aligned_pointer,
      },
      {
          "alloc_free",
          test_alloc_free,
      },
  };
  int ret = 0;
  const size_t test_count = sizeof(test) / sizeof(test_meta_t);
  chunk_count = ya_mem_pool_chunk_count();
  if (chunk_count == 0)
  {
    YA_MEM_LOG("chunks is zero (check YA_MEM_ALLOC_POOL_SIZE and YA_MEM_ALLOC_CHUNK_SIZE)\n");
    return TEST_ERR_FAIL;
  }
  YA_MEM_LOG("chunk count = %ld\n", chunk_count);
  chunks = malloc(chunk_count);
  if (chunks == NULL)
  {
    YA_MEM_LOG("chunks alloc error\n");
    return TEST_ERR_FAIL;
  }
  for (size_t i = 0; i < test_count; i++)
  {
    ret = test[i].fn();
    if (ret == TEST_ERR_OK)
    {
      YA_MEM_LOG("test %ld (%s) passed\n", (i + 1), test[i].title);
    }
    else
    {
      YA_MEM_LOG("test %ld (%s) failed\n", (i + 1), test[i].title);
      return TEST_ERR_FAIL;
    }
  }
  free(chunks);
  chunks = NULL;
  return TEST_ERR_OK;
}
