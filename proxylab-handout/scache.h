#ifndef __SCACHE__
#define __SCACHE__

typedef void *scache_t;

void scache_init(scache_t *scache);
void scache_deinit(scache_t *scache);
int scache_found(scache_t *ss, char *uri, int usize, char *response,
                 int *rsize);

void scache_insert(scache_t *ss, char *uri, int usize, char *response,
                   int rsize);

#endif  // __SCACHE__