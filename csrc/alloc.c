#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include "log.h"

void *malloc_or_exit_func(size_t size, const char *sourcefile, unsigned line)
{
  void *result = malloc(size);
  if (!result) {
    errorf("malloc(%llu) failed (e=%d, source=%s line=%d)",
           (unsigned long long)size, errno, sourcefile, line);
    exit(1);
  }
  return result;
}
