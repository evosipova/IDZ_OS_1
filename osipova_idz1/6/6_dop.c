#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <_ctype.h>
#include <fcntl.h>
#include <string.h>


#define BUFFER_SIZE 5000

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s <input_file> <output_file>\n", argv[0]);
    return 1;
  }

  // Создаем неименованный канал
  int channel[2];
  if (pipe(channel) == -1) {
    perror("pipe");
    return 1;
  }

  // Создаем первый процесс
  pid_t pid1 = fork();
  if (pid1 == -1) {
    perror("fork");
    return 1;
  }

  if (pid1 == 0) {
    // Первый процесс

    // Закрываем канал для чтения
    close(channel[0]);

    // Открываем файл для чтения
    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
      perror("fopen");
      exit(1);
    }

    // Открываем файл для записи
    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
      perror("fopen");
      exit(1);
    }

    // Читаем данные из файла и записываем в канал
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    size_t letters_count = 0;
    size_t digits_count = 0;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input_file)) > 0) {
      for (size_t i = 0; i < bytes_read; i++) {
        if (isalpha(buffer[i])) {
          letters_count++;
        } else if (isdigit(buffer[i])) {
          digits_count++;
        }
      }
    }

    // Записываем результаты в файл
    fprintf(output_file, "Digits: %zu\n", digits_count);
    fprintf(output_file, "Letters: %zu\n", letters_count);

    // Закрываем файлы
    fclose(input_file);
    fclose(output_file);

    // Закрываем канал для записи
    close(channel[1]);

    exit(0);
  }

  // Создаем второй процесс
  pid_t pid2 = fork();
  if (pid2 == -1) {
    perror("fork");
    return 1;
  }

  if (pid2 == 0) {

    // Закрываем канал для записи
    close(channel[1]);

    // Читаем данные из канала
    char buffer[BUFFER_SIZE];

    // Закрываем канал для чтения
    close(channel[0]);

    exit(0);
  }

  // Закрываем канал для чтения
  close(channel[0]);

  // Закрываем канал для записи
  close(channel[1]);

  // Ждем завершения обоих дочерних процессов
  int status;
  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);

  return 0;
}