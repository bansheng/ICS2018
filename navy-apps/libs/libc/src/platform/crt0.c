#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

__attribute__((section(".text.unlikely"))) void _start(int argc, char *argv[], char *envp[]) {
  char *env[] = {NULL};
  environ = env;
  exit(main(argc, argv, env));
  assert(0);
}
