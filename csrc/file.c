#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bool.h"
#include "array.h"
#include "lenstring.h"
#include "log.h"

// Reads the given file into a string. logs errors to stderr.
// Note that it will make sure the returned content is NULL-TERMINATED.
// Returns: NULL on error
struct mutable_string read_file(const char *filename)
{
  struct mutable_string content = {0};
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    errorf("failed to open '%s': %s (errno=%d)", filename, strerror(errno), errno);
    goto error_exit;
  }

  if (0 != fseek(file, 0, SEEK_END)) {
    errorfErrno("fseek(file='%s', SEEK_END) failed", filename);
    goto error_exit;
  }
  long size = ftell(file);
  if (size == -1) {
    errorfErrno("ftell(file='%s') failed", filename);
    goto error_exit;
  }
  if (0 != fseek(file, 0, SEEK_SET)) {
    errorfErrno("fseek(file='%s', SEEK_SET) failed", filename);
    goto error_exit;
  }
  content.ptr = malloc(size + 1);
  if (!content.ptr) {
    errorfErrno("malloc(%lu) for file '%s' failed", size + 1, filename);
    goto error_exit;
  }
  {
    size_t result = fread(content.ptr, 1, size, file);
    if (result != size) {
      errorf("fread on file '%s' with size %lu returned %llu (errno=%d '%s')",
             filename, size, (unsigned long long)result, errno, strerror(errno));
      goto error_exit;
    }
  }
  content.ptr[size] = '\0';
  goto exit;
 error_exit:
  if (content.ptr) {
    free(content.ptr);
    content = make_mutable_string(NULL, 0);
  }
 exit:
  if (file)
    fclose(file);
  return content;
}
