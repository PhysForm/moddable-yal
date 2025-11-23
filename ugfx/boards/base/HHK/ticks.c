#include "gfx.h"
#include <sys/time.h>

gTicks gfxSystemTicks() {
  struct timeval t;
  gettimeofday(&t, nullptr);
  return t.tv_usec;
}

gTicks gfxMillisecondsToTicks(gDelay ms) { return ms * 1000; }
