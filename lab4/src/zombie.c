#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  pid_t child_pid = fork();
  if (child_pid == 0) {
    printf("PID = %d, PPID = %d\n", getpid(), getppid());
    fflush(NULL);
    return 0;
  }

  printf("PID = %d\n", getpid());
  printf("%d завершён\n", child_pid);
  fflush(NULL);
  sleep(10);
  printf("Родитель получает результат завершения %d\n", child_pid);
  wait(NULL);
  printf("Процесс удалён\n");
  return 0;
}
