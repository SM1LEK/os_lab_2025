#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

void *FirstThread(void *arg) {
  pthread_mutex_lock(&mut1);
  printf("(1) -> 1\n");
  fflush(NULL);
  sleep(1);
  printf("(1) ~ 2\n");
  fflush(NULL);
  pthread_mutex_lock(&mut2);
  pthread_mutex_unlock(&mut2);
  pthread_mutex_unlock(&mut1);
  return NULL;
}

void *SecondThread(void *arg) {
  pthread_mutex_lock(&mut2);
  printf("(2) -> 2\n");
  fflush(NULL);
  sleep(1);
  printf("(2) ~ 1\n");
  fflush(NULL);
  pthread_mutex_lock(&mut1);
  pthread_mutex_unlock(&mut1);
  pthread_mutex_unlock(&mut2);
  return NULL;
}

int main() {
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, FirstThread, NULL);
  pthread_create(&thread2, NULL, SecondThread, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  return 0;
}
