#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 5000

int main(int argc, char *argv[]) {
  if (argc < 3) {
    printf("Usage: %s input_file output_file\n", argv[0]);
    return 1;
  }

  char *input_file = argv[1];
  char *output_file = argv[2];
  int input_fd, output_fd;
  char buffer[BUFFER_SIZE];

  // Создаем именованный канал для передачи данных
  char *fifo_name = "/tmp/myfifo";
  mkfifo(fifo_name, 0666);

  // Открываем файлы для чтения и записи
  input_fd = open(input_file, O_RDONLY);
  output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);

  // Открываем именованный канал для записи
  int fifo_fd = open(fifo_name, O_WRONLY);

  // Читаем данные из файла и записываем в именованный канал
  ssize_t bytes_read;
  while ((bytes_read = read(input_fd, buffer, BUFFER_SIZE)) > 0) {
    write(fifo_fd, buffer, bytes_read);
  }

  // Закрываем файлы и именованный канал
  close(input_fd);
  close(fifo_fd);

  // Читаем данные из именованного канала
  fifo_fd = open(fifo_name, O_RDONLY);

  // Создаем буфер для чтения данных из именованного канала
  char result_buffer[BUFFER_SIZE];
  ssize_t result_bytes_read = read(fifo_fd, result_buffer, BUFFER_SIZE);

  // Записываем результат обработки данных в выходной файл
  write(output_fd, result_buffer, result_bytes_read);

  // Закрываем файлы и именованный канал
  close(fifo_fd);
  close(output_fd);
  unlink(fifo_name);

  return 0;
}
