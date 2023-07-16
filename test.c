#define ELANG_IMPLEMENTATION
#include "elang.h"

#include <stdio.h>
#include <stdlib.h>

// Error reporting mechanism. Must be implemented by the user
void platformErrorStart(void) {
  fputs("Error: ", stderr);
}

void platformErrorPush(char *data, int count) {
  fwrite(data, count, 1, stderr);
}

void platformErrorEnd(void) {
  fputc('\n', stderr);
  exit(1);
}

// Native function to be provided in the language
float print(float *args) {
  printf("%g\n", args[0]);
  return 0;
}

char *readFile(char *path, int *size) {
  FILE *file = fopen(path, "r");
  if (!file) {
    return NULL;
  }

  if (fseek(file, 0, SEEK_END) == -1) {
    fclose(file);
    return NULL;
  }

  *size = ftell(file);
  if (*size == -1) {
    fclose(file);
    return NULL;
  }
  rewind(file);

  char *data = malloc(*size);
  if (!data) {
    fclose(file);
    return NULL;
  }

  if (fread(data, 1, *size, file) != *size) {
    free(data);
    fclose(file);
    return NULL;
  }

  fclose(file);
  return data;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Error: File path not provided\n");
    fprintf(stderr, "Usage: %s <file>\n", *argv);
    return 1;
  }
  char *path = argv[1];

  int size;
  char *data = readFile(path, &size);
  if (!data) {
    fprintf(stderr, "Usage: could not read file '%s'\n", path);
    return 1;
  }

  // Register the native function
  elangRegisterNative("print", 1, print);

  // Compile and run
  elangCompile(data, size) && elangRun();
}
