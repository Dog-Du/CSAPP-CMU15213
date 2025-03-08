#include "sbuf.h"
#include "csapp.h"

typedef struct {
  int *buf;
  int n;
  int front;
  int rear;

  sem_t mutex;
  sem_t slots;
  sem_t items;
} sbuf;

void sbuf_init(sbuf_t *ss, int n) {
  *ss = Calloc(sizeof(sbuf), 1);
  sbuf *sp = (sbuf *)*ss;

  sp->buf = Calloc(n, sizeof(int));
  sp->n = n;
  sp->front = sp->rear = 0;
  Sem_init(&sp->mutex, 0, 1);
  Sem_init(&sp->slots, 0, n);
  Sem_init(&sp->items, 0, 0);
}

void sbuf_deinit(sbuf_t *ss) {
  sbuf *sp = (sbuf *)*ss;

  Free(sp->buf);
  Free(sp);
}

void sbuf_insert(sbuf_t *ss, int item) {
  sbuf *sp = (sbuf *)*ss;

  P(&sp->slots);
  P(&sp->mutex);
  sp->buf[(++sp->rear) % (sp->n)] = item;
  V(&sp->mutex);
  V(&sp->items);
}

int sbuf_remove(sbuf_t *ss) {
  sbuf *sp = (sbuf *)*ss;

  int item;
  P(&sp->items);
  P(&sp->mutex);
  item = sp->buf[(++sp->front) % (sp->n)];
  V(&sp->mutex);
  V(&sp->slots);
  return item;
}