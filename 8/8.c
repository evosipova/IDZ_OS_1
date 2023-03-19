#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUF 5000

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s input_file output_file\n", argv[0]);
    exit(1);
  }

  char *input_file = argv[1];
  char *output_file = argv[2];
  char *fifo_name = "/tmp/my_fifo";

  mkfifo(fifo_name, 0666);

  int pid = fork();

  if (pid == 0) {
    int fd = open(fifo_name, O_WRONLY);
    int input_fd = open(input_file, O_RDONLY);
    char buf[MAX_BUF];
    int n;

    while ((n = read(input_fd, buf, MAX_BUF)) > 0) {
      write(fd, buf, n);
    }

    close(fd);
    close(input_fd);
    exit(0);
  } else if (pid > 0) {
    int fd = open(fifo_name, O_RDONLY);
    char buf[MAX_BUF];
    int n;
    int num_digits = 0;
    int num_letters = 0;

    while ((n = read(fd, buf, MAX_BUF)) > 0) {
      for (int i = 0; i < n; i++) {
        if (isdigit(buf[i])) {
          num_digits++;
        } else if (isalpha(buf[i])) {
          num_letters++;
        }
      }
    }

    close(fd);

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    dprintf(output_fd, "Digits: %d\n", num_digits);
    dprintf(output_fd, "Letters: %d\n", num_letters);
    close(output_fd);

    wait(NULL);

    exit(0);
  } else {
    perror("fork");
    exit(1);
  }
}
