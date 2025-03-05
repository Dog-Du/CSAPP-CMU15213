#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "../csapp.h"

int s;
int n;
sem_t mutex;

void *test_thread(void *x) {
  char buf[MAXLINE];

  if (x == NULL) {
    system("echo \"\" > mytest.result");
    for (int i = s; i <= n; ++i) {
      if (i >= 11 && i <= 13) {
        P(&mutex);
      }
      if (i < 10) {
        memset(buf, 0, MAXLINE);
        sprintf(buf, "make test0%d >> mytest.result", i);
        system(buf);
      } else {
        memset(buf, 0, MAXLINE);
        sprintf(buf, "make test%d >> mytest.result", i);
        system(buf);
      }

      if(i >= 11 && i <= 13) {
        V(&mutex);
      }
    }
  } else {
    system("echo \"\" > rtest.result");
    for (int i = s; i <= n; ++i) {
      if (i >= 11 && i <= 13) {
        P(&mutex);
      }

      if (i < 10) {
        memset(buf, 0, MAXLINE);
        sprintf(buf, "make rtest0%d >> rtest.result", i);
        system(buf);
      } else {
        memset(buf, 0, MAXLINE);
        sprintf(buf, "make rtest%d >> rtest.result", i);
        system(buf);
      }

      if (i >= 11 && i <= 13) {
        V(&mutex);
      }
    }
  }
  return NULL;
}

int main(int argc, char *argv[], char *envp[]) {
  if (argc == 2) {
    s = 1;
    n = atoi(argv[1]);
  } else if (argc == 3) {
    s = atoi(argv[1]);
    n = atoi(argv[2]);
  }

  s = s > 1 ? s : 1;
  n = n > 16 ? 16 : n;

  sem_init(&mutex, 0, 1);

  pthread_t p1, p2;
  pthread_create(&p1, NULL, test_thread, (void *)0);
  pthread_create(&p2, NULL, test_thread, (void *)1);

  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  return 0;
}