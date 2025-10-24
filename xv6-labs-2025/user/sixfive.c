#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void process_file(char *filename) {
  int fd = open(filename, O_RDONLY);
  if(fd < 0) {
      fprintf(2, "Error: cannot open %s\n", filename);
      return;
  }
  
  char buff[2];
  int n, val = 0;
  
  while((n = read(fd, buff, 1)) > 0) {
      buff[1] = '\0';
      if(strchr(" -\r\t\n./,", buff[0]) != 0) {
          if(val != 0 && (val % 6 == 0 || val % 5 == 0)) {
              fprintf(1, "%d\n", val);
          }
          val = 0;
      } else {
          val = (val*10) + atoi(buff);
      }
  }
  
  if(val != 0 && (val % 6 == 0 || val % 5 == 0)) {
      fprintf(1, "%d\n", val);
  }
  
  close(fd);
}





int main(int argc, char *argv[]) {
  if(argc < 2) {
      fprintf(2, "Usage: sixfive <file1> [file2] ...\n");
      exit(1);
  }
  
  for(int i = 1; i < argc; i++) {
      process_file(argv[i]);
  }
  
  exit(0);
}
