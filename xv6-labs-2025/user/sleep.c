#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(2, "currArgNum: %d --- expectedArgNum: %d\n", argc - 1, 1);
    exit(1);
  }
  int t;
  if ((t = atoi(argv[1])) < 0) {
    fprintf(2, "%s is not a valid number \n",argv[1]);
    exit(1);
  }
  pause(t);
  exit(0);
}
