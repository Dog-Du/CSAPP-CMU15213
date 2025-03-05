#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "../csapp.h"

void sigchld_handler(int sig) {
  printf("sigchild_handler\n");
  while(waitpid(-1, NULL, WUNTRACED | WNOHANG)) {
    sleep(1);
  }
}

int main() {
  Signal(SIGCHLD, sigchld_handler);

  pid_t pid = fork();

  if ((pid = fork()) == 0) {
    while (1) {
      ;
    }
    exit(0);
  }

  kill(pid, SIGINT);
  sleep(1);
  printf("end\n");
  return 0;
}