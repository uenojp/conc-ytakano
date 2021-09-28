#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void *inc(void *count) {
  int *c = (int *)count;

  // lockとunlockをコメントアウトすると競合する
  if (0)
    if (pthread_mutex_lock(&mut) != 0) {
      perror("pthread_mutex_lock");
      exit(1);
    }
  for (int i = 0; i < 100000; i++) (*c)++;
  if (0)
    if (pthread_mutex_unlock(&mut) != 0) {
      perror("pthread_mutex_unlock");
      exit(1);
    }

  return NULL;
}

int main(int argc, char **argv) {
  int count = 0;

  printf("%d\n", count);
  pthread_t th1, th2;
  if (pthread_create(&th1, NULL, inc, &count) != 0) {
    perror("pthread_create");
    exit(1);
  }
  if (pthread_create(&th2, NULL, inc, &count) != 0) {
    perror("pthread_create");
    exit(1);
  }

  if (pthread_join(th1, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  if (pthread_join(th2, NULL) != 0) {
    perror("pthread_join");
    exit(1);
  }

  printf("%d\n", count);
  return 0;
}