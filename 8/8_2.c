#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
  // Создаем именованный канал для передачи данных
  char *fifo_name = "/tmp/myfifo";
  mkfifo(fifo_name, 0666);

  // Открываем именованный канал для чтения
  int fifo_fd = open(fifo_name, O_RDONLY);

  // Создаем буфер для чтения данных из именованного канала
  char buffer[BUFFER_SIZE];
  ssize_t bytes_read = read(fifo_fd, buffer, BUFFER_SIZE);

  // Обрабатываем данные
  int letters = 0;
  int digits = 0;
  for (int i = 0; i < bytes_read; i++) {
    if (isalpha(buffer[i])) {
      letters++;
    } else if (isdigit(buffer[i])) {
      digits++;
    }
  }

  // Закрываем именованный канал для чтения
  close(fifo_fd);

  // Открываем именованный канал для записи
  fifo_fd = open(fifo_name, O_WRONLY);

  // Создаем буфер для записи результата обработки данных в именованный канал
  char result_buffer[BUFFER_SIZE];
  int result_bytes_written = sprintf(result_buffer, "Digits: %d\nLetters: %d", letters, digits);

  // Записываем результат обработки данных в именованный канал
  write(fifo_fd, result_buffer, result_bytes_written);

  // Закрываем именованный канал для записи
  close(fifo_fd);
  unlink(fifo_name);

  return 0;
}
