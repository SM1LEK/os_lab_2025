#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

volatile sig_atomic_t timeout_expired = 0;

void HandleAlarm(int signal_number) {
  timeout_expired = 1;
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  int timeout = 0;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"timeout", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            // your code here
            if (seed <= 0) {
              printf("+seed\n");
              return 1;
            }
            // error handling
            break;
          case 1:
            array_size = atoi(optarg);
            // your code here
            if (array_size <= 0) {
              printf("+array_size\n");
              return 1;
            }
            // error handling
            break;
          case 2:
            pnum = atoi(optarg);
            // your code here
            if (pnum <= 0) {
              printf("+pnum\n");
              return 1;
            }
            // error handling
            break;
          case 3:
            timeout = atoi(optarg);
            if (timeout <= 0) {
              printf("Неверный таймаут\n");
              return 1;
            }
            break;
          case 4:
            with_files = true;
            break;

          default:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" [--timeout \"num\"] \n",
           argv[0]);
    return 1;
  }

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;
  pid_t child_pids[pnum];

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int (*pipes)[2] = NULL;
  if (!with_files) {
    pipes = malloc(sizeof(int[2]) * pnum);
    for (int i = 0; i < pnum; i++) {
      if (pipe(pipes[i]) < 0) {
        printf("Не удалось создать канал\n");
        return 1;
      }
    }
  }

  for (int i = 0; i < pnum; i++) {
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid > 0) {
        child_pids[i] = child_pid;
      }
      if (child_pid == 0) {
        // child process

        // parallel somehow
        unsigned int chunk = array_size / pnum;
        unsigned int begin = i * chunk;
        unsigned int end = begin + chunk;
        if (i == pnum - 1) {
          end = array_size;
        }

        struct MinMax local = GetMinMax(array, begin, end);

        if (with_files) {
          // use files here
          char filename[64];
          sprintf(filename, "min_max_%d.txt", i);
          FILE *file = fopen(filename, "w");
          if (file == NULL) {
            printf("Не удалось открыть файл %s\n", filename);
            return 1;
          }
          fprintf(file, "%d %d\n", local.min, local.max);
          fclose(file);
        } else {
          // use pipe here
          close(pipes[i][0]);
          write(pipes[i][1], &local, sizeof(struct MinMax));
          close(pipes[i][1]);
        }
        return 0;
      }

    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  if (timeout > 0) {
    signal(SIGALRM, HandleAlarm);
    alarm(timeout);

    while (active_child_processes > 0 && !timeout_expired) {
      pid_t finished_pid = waitpid(-1, NULL, WNOHANG);
      if (finished_pid > 0) {
        active_child_processes -= 1;
        for (int i = 0; i < pnum; i++) {
          if (child_pids[i] == finished_pid) {
            child_pids[i] = 0;
          }
        }
      }
      if (finished_pid < 0) {
        active_child_processes = 0;
      }
    }

    alarm(0);

    if (timeout_expired) {
      for (int i = 0; i < pnum; i++) {
        if (child_pids[i] > 0) {
          kill(child_pids[i], SIGKILL);
        }
      }
      while (active_child_processes > 0) {
        wait(NULL);
        active_child_processes -= 1;
      }
      free(array);
      printf("Время выполнения истекло\n");
      return 1;
    }
  } else {
    while (active_child_processes > 0) {
      // your code here
      wait(NULL);
      active_child_processes -= 1;
    }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    int min = INT_MAX;
    int max = INT_MIN;

    if (with_files) {
      // read from files
      char filename[64];
      sprintf(filename, "min_max_%d.txt", i);
      FILE *file = fopen(filename, "r");
      if (file == NULL) {
        printf("Не удалось открыть файл %s\n", filename);
        return 1;
      }
      fscanf(file, "%d %d", &min, &max);
      fclose(file);
      remove(filename);
    } else {
      // read from pipes
      struct MinMax local;
      read(pipes[i][0], &local, sizeof(struct MinMax));
      close(pipes[i][0]);
      min = local.min;
      max = local.max;
    }

    if (min < min_max.min) min_max.min = min;
    if (max > min_max.max) min_max.max = max;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
