#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "bool.h"
#include "array.h"
#include "lenstring.h"

bool str_ends_with(struct string left, struct string right)
{
  return (left.length >= right.length) &&
    (0 == memcmp(left.ptr + (left.length - right.length), right.ptr, right.length));
}
bool str_starts_with(struct string left, struct string right)
{
  return (left.length >= right.length) &&
    (0 == memcmp(left.ptr, right.ptr, right.length));
}

/*
size_t str_diff_index(struct string left, struct string right)
{
  size_t limit = (left.length <= right.length) ? left.length : right.length;
  size_t i = 0;
  for (i = 0; ; i++) {
    if (i >= limit || left.ptr[i] != right.ptr[i])
      return i;
  }
}
*/

/*
THIS FUNCTION DOESN'T WORK ON ARM
struct mutable_string concat_impl(struct string first, ...)
{
  va_list args;
  size_t length = first.length;
  va_start(args, first);
  for (;;) {
    struct string next = va_arg(args, struct string);
    if (!next.ptr) break;
    length += next.length;
  }
  va_end(args);
  char *buf = malloc(length);
  memcpy(buf, first.ptr, first.length);
  size_t offset = first.length;
  va_start(args, first);
  for (;;) {
    struct string next = va_arg(args, struct string);
    if (!next.ptr) break;
    memcpy(buf + offset, next.ptr, next.length);
    offset += next.length;
  }
  va_end(args);
  return make_mutable_string(buf, length);
}
*/
