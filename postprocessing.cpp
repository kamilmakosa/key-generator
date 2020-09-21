#include <stdio.h>

int postprocessing(int levels[], int number) {
  int cc = 0;
  unsigned long timestamp;

  for(int i=0; i<256; i++) {
    if(levels[i] >= number) {
      while(levels[i+cc+1] < 0 && i+cc+1 < 256) {
        cc++;
      }
      if(cc) {
        timestamp = (unsigned long) time(NULL);
        cc = (int)(timestamp % (cc+1));
      }
      return i+cc;
    }
  }
  return 255;
}
