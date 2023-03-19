#include <_ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Usage: %s input_file output_file\n", argv[0]);
    return 1;
  }

  int fd[2]; // Массив для хранения файловых дескрипторов канала.
  if (pipe(fd) == -1) { // Создаем неименованный канал.
    perror("pipe");
    return 1;
  }

  //  процесс 1 записывает данные из файла в канал.
  int input_fd = open(argv[1], O_RDONLY);
  char buffer[BUFFER_SIZE];
  ssize_t n;
  while ((n = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
    write(fd[1], buffer, n);
  }
  close(input_fd);

  pid_t pid1 = fork();
  if (pid1 == -1) {
    perror("fork");
    return 1;
  } else if (pid1 == 0) {


    wait(NULL);
    // Записываем результат в выходной файл.
    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
    char result_buffer[BUFFER_SIZE];
    snprintf(result_buffer, BUFFER_SIZE, "Digits: %d\nLetters: %d\n",
             digit_count, letter_count);
    write(output_fd, result_buffer, strlen(result_buffer));
    close(output_fd);
    return 0;
  }
  pid_t pid2 = fork();
  if (pid2 == -1) {
    perror("fork");
    return 1;
  } else if (pid2 == 0) {
    // Дочерний процесс 2 считывает данные из канала и
    // подсчитывает количество цифр и букв.
    close(fd[1]); // Закрываем запись.
    char buffer[BUFFER_SIZE];
    ssize_t n;
    int digit_count = 0, letter_count = 0;
    while ((n = read(fd[0], buffer, BUFFER_SIZE)) > 0) {
      for (int i = 0; i < n; i++) {
        if (isdigit(buffer[i])) {
          digit_count++;
        } else if (isalpha(buffer[i])) {
          letter_count++;
        }
      }
    }
    close(fd[0]); // Закрываем чтение.
    return 0;
  }

  // Родительский процесс ждет завершения всех дочерних процессов.
  close(fd[0]);
  close(fd[1]);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  return 0;
}
