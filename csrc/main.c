#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bool.h"
#include "err_t.h"
#include "array.h"
#include "lenstring.h"
#include "vector.h"

#include "log.h"
#include "file.h"
#include "ast.h"
#include "scriptfile.h"
#include "parse.h"
#include "interpreter.h"

void usage()
{
  printf("Usage: script <file>\n");
  printf("Options:\n");
}
int main(int argc, char *argv[])
{
  argv++;argc--;
  {
    int original_argc = argc;
    argc = 0;
    for (int i = 0; i < original_argc; i++) {
      char *arg = argv[i];
      if (arg[0] != '-') {
        argv[argc++] = arg;
      } else if (0 == strcmp(arg, "-h") || 0 == strcmp(arg, "--help")) {
        usage();
        return 1;
      } else {
        errorf("unknown command-line option '%s'", arg);
        return 1; // fail
      }
    }
  }
  if (argc == 0) {
    usage();
    return 1; // fail
  }
  if (argc != 1) {
    errorf("too many command-line arguments (%d), expected only 1 script", argc);
    return 1; // fail
  }
  struct script_file file = {0};
  file.filename = argv[0];
  file.text = as_string(read_file(file.filename));
  if (file.text.ptr == NULL) {
    // error already logged
    return 1; // fail
  }

  if (parse_script(&file)) {
    // error already logged
    return 1; // fail
  }
  return interpret_script(&file);
}
