#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#if _WIN32
  #include <windows.h>
  #define PATH_SEPARATOR ';'
#else
  #include <unistd.h> // for the 'access' function
  #define PATH_SEPARATOR ':'
#endif


#include "bool.h"
#include "err_t.h"
#include "array.h"
#include "lenstring.h"

#include "log.h"
#include "path.h"
#include "which.h"

bool contains_dir_separator(struct string str)
{
#if _WIN32
  for (size_t i = 0; i < str.length; i++) {
    char c = str.ptr[0];
    if (c == '\\' || c == '/')
      return true;
  }
  return false;
#else
  return memchr(str.ptr, '/', str.length) != NULL;
#endif
}

static bool exists(const char *file)
{
  printf("[DEBUG]   checking if '%s' exists...\n", file);
  //printf("[DEBUG]   it doesn't\n");
#if _WIN32
  return 0xFFFFFFFF != GetFileAttributes(file);
#else
  return 0 == access(file, F_OK);
#endif
}


err_t join_exists(struct string path, struct string postfix, struct string *out)
{
  printf("[DEBUG]   TODO: need to escape '~' in path '%.*s'\n", (unsigned)path.length, path.ptr);

  struct string combined = malloc_dir_entry(path, postfix);
  if (combined.ptr == NULL) {
    errorf("malloc failed for dir entry (e=%d)", errno);
    return err_fail;
  }
  if (exists(combined.ptr)) {
    *out = combined;
    return err_pass;
  }
  free((void*)combined.ptr);
  return err_pass;
}

// returns a null-terminated string where the program resides
// NOTE: prog does not need to be null-terminated
// Assumption: result.ptr is NULL
err_t which(struct string prog, struct string *result)
{
  if (result->ptr) {
    errorf("codebug: which was called with a non-NULL result string");
    return err_fail;
  }

  char *path = getenv("PATH");
  if (path == NULL)
    return err_pass;

  size_t start = 0;
  size_t next = 0;
  for (;;) {
    char c = path[next];
    if (c != PATH_SEPARATOR && c != '\0') {
      next++;
    } else {
      size_t length = next - start;
      if (length > 0) {
#ifdef _WIN32
        printf("[DEBUG] TODO: check each extension in PATHEXT\n");
#endif
        //printf("[DEBUG] which check dir '%.*s'\n", (unsigned)length, &path[start]);
        if (join_exists(make_string(&path[start], length), prog, result))
          return err_fail; // error already logged
        if (result->ptr)
          return err_pass; // success, with result
      }
      if (c == '\0')
        return err_pass; // success, no result
      next++;
      start = next;
    }
  }
}
