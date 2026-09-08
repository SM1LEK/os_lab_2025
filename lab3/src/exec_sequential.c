#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  pid_t child_pid = fork();

  if (child_pid == 0) {
    printf("Потомок %d\n", getpid());
    execl("./sequential_min_max", "sequential_min_max", argv[1], argv[2],
          (char *)NULL);
    return 1;
  }

  wait(NULL);
  printf("Готово\n");

  return 0;
}
