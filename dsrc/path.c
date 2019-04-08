#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bool.h"
#include "err_t.h"
#include "array.h"
#include "lenstring.h"

#include "path.h"

#if _WIN32
  #define DEFAULT_FILE_SEPARATOR '\\'
#else
  #define DEFAULT_FILE_SEPARATOR '/'
#endif

struct string malloc_dir_entry(struct string dir, struct string file)
{
  // remove leading '/' from file
  if (file.length > 0 && file.ptr[0] == '/') {
    file.ptr++;
    file.length--;
  }

  unsigned middleSlash = (file.length > 0 && dir.length > 0 && dir.ptr[dir.length - 1] != '/') ? 1 : 0;
  size_t length = dir.length + middleSlash + file.length;
  char *buf = malloc(length + 1);
  if (buf) {
    memcpy(buf, dir.ptr, dir.length);
    if (middleSlash)
      buf[dir.length] = DEFAULT_FILE_SEPARATOR;
    memcpy(buf + dir.length + middleSlash, file.ptr, file.length);
    buf[dir.length + middleSlash + file.length] = '\0';
  }
  return make_string(buf, length);
}
