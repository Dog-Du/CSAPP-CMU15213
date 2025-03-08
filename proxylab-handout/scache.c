#include "scache.h"
#include <pthread.h>
#include "csapp.h"
#include "defs.h"

#define CACHE_SIZE (((MAX_CACHE_SIZE) / (MAX_OBJECT_SIZE)) + 1)

typedef struct {
  char uri[MAXLINE >> 6];
  char object[MAX_OBJECT_SIZE];
  int object_size;
  int uri_size;

  int count;  // count for lru.
} cache_page;

typedef struct {
  cache_page cache[CACHE_SIZE];
  pthread_rwlock_t mutex;
} scache;

void scache_init(scache_t *ss) {
  *ss = Calloc(1, sizeof(scache));
  scache *cache = (scache *)*ss;
  pthread_rwlock_init(&cache->mutex, NULL);
  return;
}

void scache_deinit(scache_t *ss) {
  scache *cache = (scache *)*ss;
  pthread_rwlock_destroy(&cache->mutex);
  Free(cache);
}

int scache_found(scache_t *ss, char *uri, int usize, char *response,
                 int *rsize) {
  scache *cache = (scache *)*ss;
  pthread_rwlock_wrlock(&cache->mutex);
  int i;
  int j;

  for (i = 0; i < CACHE_SIZE; ++i) {
    if (cache->cache[i].uri_size == usize &&
        strncmp(cache->cache[i].uri, uri, usize) == 0) {
      break;
    }
  }

  if (i == CACHE_SIZE) {
    fprintf(stderr, "not find : %s\n", uri);
    pthread_rwlock_unlock(&cache->mutex);
    return 0;
  }

  for (j = 0; j < CACHE_SIZE; ++j) {
    if (cache->cache[j].uri_size > 0 &&
        cache->cache[j].count < cache->cache[i].count) {
      ++cache->cache[j].count;
    }
  }
  cache->cache[i].count = 0;  // 这里应该遵守 lru 的规则。

  memcpy(response, cache->cache[i].object, cache->cache[i].object_size);
  *rsize = cache->cache[i].object_size;
  pthread_rwlock_unlock(&cache->mutex);
  return 1;
}

void scache_insert(scache_t *ss, char *uri, int usize, char *response,
                   int rsize) {
  scache *cache = (scache *)*ss;
  pthread_rwlock_wrlock(&cache->mutex);

  int i;
  int j;
  for (i = 0; i < CACHE_SIZE; ++i) {
    if (cache->cache[i].uri_size == 0) {
      break;
    }
  }

  if (i < CACHE_SIZE) {
    for (j = 0; j < CACHE_SIZE; ++j) {
      if (cache->cache[j].uri_size > 0) {
        cache->cache[j].count++;
      }
    }

    cache->cache[i].count = 0;
    memcpy(cache->cache[i].uri, uri, usize);
    memcpy(cache->cache[i].object, response, rsize);
    cache->cache[i].uri_size = usize;
    cache->cache[i].object_size = rsize;
    cache->cache[i].object[rsize] = '\0';
    cache->cache[i].uri[usize] = '\0';
    pthread_rwlock_unlock(&cache->mutex);
    return;
  }

  j = 0;
  for (i = 0; i < CACHE_SIZE; ++i) {
    ++cache->cache[i].count;

    if (cache->cache[i].count > cache->cache[j].count) {
      j = i;
    }
  }

  cache->cache[i].count = 0;
  memcpy(cache->cache[i].uri, uri, usize);
  memcpy(cache->cache[i].object, response, rsize);
  cache->cache[i].uri_size = usize;
  cache->cache[i].object_size = rsize;
  cache->cache[i].object[rsize] = '\0';
  cache->cache[i].uri[usize] = '\0';
  pthread_rwlock_unlock(&cache->mutex);
  return;
}