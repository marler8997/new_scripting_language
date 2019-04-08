#!/usr/bin/env rund
//!importPath ../../mar/src
module __none;

import mar.flag;
import mar.sentinel : assumeSentinel;
import mar.mmap : createMemoryMap;

import nsl.log;
import nsl.scriptfile : ScriptFile;
import nsl.parse : parse;

/*
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
*/

void usage()
{
    import mar.io : stderr;
    stderr.write("Usage: script <file>
Options:
  TODO: add some options
");
}
int main(string[] args)
{
  args = args[1..$];
  {
    size_t newArgsLength = 0;
    scope (exit) args = args[0 .. newArgsLength];
    for (size_t i = 0; i < args.length; i++) {
      auto arg = args[i];
      if (arg[0] != '-') {
        args[newArgsLength++] = arg;
      } else if (arg == "-h" || arg == "--help") {
        usage();
        return 1;
      } else {
        logError("unknown command-line option '", arg, "'");
        return 1; // fail
      }
    }
  }
  if (args.length == 0)
  {
      usage();
      return 1; // fail
  }
  if (args.length != 1)
  {
      logError("too many command-line arguments (", args.length, "), expected only 1 script");
      return 1; // fail
  }


  auto file = ScriptFile.open(args[0].assumeSentinel);
  if (file.failed)
  {
      logError(file);
      return 1; // fail
  }
  parse(file.val);

  logError("not impl");
  return 1;
  /*
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
  */
}
