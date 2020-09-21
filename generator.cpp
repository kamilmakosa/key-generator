#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "racing.cpp"
#include "postprocessing.cpp"

#define FIRSTBIT 0x8000000000000000

void set_parity_bits(unsigned long &key) {
  bool parity_bit = false;
  for(int i=0; i<64; i++) {
    if (i%8 == 7) {
      if (parity_bit && ((FIRSTBIT>>i) & key) == 0) {
        key += (FIRSTBIT>>i);
      } else if (!parity_bit && ((FIRSTBIT>>i) & key) > 0) {
        key -= (FIRSTBIT>>i);
      }
      parity_bit = false;
    } else {
      if (((FIRSTBIT>>i) & key) > 0) {
        parity_bit = !parity_bit;
      }
    }
  }
}

int main(int argc, char ** argv) {
  char * filename = NULL;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--levels") == 0) {
      filename = argv[i+1];
    }
  }

  if (filename == NULL) {
    perror("Missing parameter 'levels'\n");
    return 0;
  }

  FILE * file;
  file = fopen(filename, "r");
  if (file == NULL) {
    perror("No open file\n");
    return 0;
  }

  int levels[256];
  char str[10];
  int i = 0;
  int number;
  while (fgets(str, 10, file) != NULL) {
    sscanf(str, "%d", &number);
    levels[i++] = number;
  }

  int times[8];
  int numbers[8];
  unsigned long random_number = 0;
  for(int i=0; i<8; i++) {
    random_number <<= 8;
    times[i] = race_time();
    numbers[i] = postprocessing(levels, times[i]);
    random_number ^= numbers[i];
    sleep(1);
  }
  set_parity_bits(random_number);
  printf("%016lX", random_number);
}
