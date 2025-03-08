#ifndef __SBUF__
#define __SBUF__

#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include "csapp.h"

typedef void *sbuf_t;

void sbuf_init(sbuf_t *sp, int n);
void sbuf_deinit(sbuf_t *sp);
void sbuf_insert(sbuf_t *sp, int item);
int sbuf_remove(sbuf_t *sp);

#endif  // __SBUF__