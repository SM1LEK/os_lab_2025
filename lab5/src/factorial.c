#include <getopt.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct FactArgs {
  int begin;
  int end;
  int mod;
};

long long total = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void *ThreadFact(void *arg) {
  struct FactArgs args = *(struct FactArgs *)arg;
  long long part = 1;
  for (int i = args.begin; i <= args.end; i++) {
    part = (part * i) % args.mod;
  }
  pthread_mutex_lock(&mut);
  total = (total * part) % args.mod;
  pthread_mutex_unlock(&mut);
  return NULL;
}

int main(int argc, char **argv) {
  int k = -1;
  int pnum = -1;
  int mod = -1;

  while (true) {
    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "k:", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 'k':
        k = atoi(optarg);
        break;
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            break;
          case 1:
            pnum = atoi(optarg);
            break;
          case 2:
            mod = atoi(optarg);
            break;
          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case '?':
        break;
      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (k <= 0) {
    printf("+k\n");
    return 1;
  }
  if (pnum <= 0) {
    printf("+pnum\n");
    return 1;
  }
  if (mod <= 0) {
    printf("+mod\n");
    return 1;
  }

  if (pnum > k) {
    pnum = k;
  }

  pthread_t threads[pnum];
  struct FactArgs args[pnum];

  int chunk = k / pnum;

  for (int i = 0; i < pnum; i++) {
    args[i].begin = i * chunk + 1;
    args[i].end = (i + 1) * chunk;
    if (i == pnum - 1) {
      args[i].end = k;
    }
    args[i].mod = mod;

    if (pthread_create(&threads[i], NULL, ThreadFact, (void *)&args[i]) != 0) {
      printf("Ошибка создания потока\n");
      return 1;
    }
  }

  for (int i = 0; i < pnum; i++) {
    pthread_join(threads[i], NULL);
  }

  printf("%d! mod %d = %lld\n", k, mod, total);

  return 0;
}
