#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "bool.h"
#include "err_t.h"
#include "array.h"
#include "lenstring.h"
#include "vector.h"
#include "stringvector.h"

#include "log.h"
#include "alloc.h"
#include "which.h"
#include "ast.h"
#include "scriptfile.h"

struct interpreter
{
  struct script_file *file;
};

static err_t print(struct ast_node *node)
{
  switch (node->type) {
  case AST_NODE_STRING:
    printf("%.*s", (unsigned)node->string.escaped.length, node->string.escaped.ptr);
    return err_pass;
  default:
    errorf("print node type '%s' (%d) not implemented", ast_node_type_strings[node->type], node->type);
    return err_fail;
  }
}

static err_t run_call(struct interpreter *interpreter, struct ast_node *node)
{
  switch(node->call.function.length) {
  case 5:
    if (string_equals(node->call.function, strlit("print"))) {
      for (size_t i = 0; i < ast_node_vector_size(&node->call.args); i++) {
        struct ast_node *arg = ast_node_vector_get(&node->call.args, i);
        if (print(arg))
          return err_fail;
      }
      printf("\n");
      return err_pass;
    }
    break;
  }
  errorf("unknown function '%.*s'", (unsigned)node->call.function.length, node->call.function.ptr);
  return err_fail;
}

static err_t as_run_arg(struct ast_node *node, struct string *out_string)
{
  if (node->type == AST_NODE_STRING) {
    *out_string = node->string.escaped;
    return err_pass;
  }
  errorf("as_run_arg: not sure how to convert node type '%s' to a string yet",
         ast_node_type_strings[node->type]);
  return err_fail;
}


static err_t run_args_to_strings(struct interpreter *interpreter,
                                 struct ast_node_vector *arg_nodes,
                                 struct string_vector *arg_strings)
{
  for (size_t i = 0; i < arg_nodes->size; i++) {
    struct ast_node *node = ast_node_vector_get(arg_nodes, i);
    switch(node->type) {
      /*
    case AST_NODE_STRING:
      {
        const char *copy = malloc_str_to_cstr(node->string.escaped);
        // NOTE: this doesn't work because it needs to be a string pointer!
        if (string_vector_add(arg_strings, make_string(copy, node->string.escaped.length))) {
          errorf("failed to allocate memory for string vector (e=%d)", errno);
          // TODO: cleanup strings that were allocated
          return err_fail;
        }
      }
      */
    default:
      errorf("run_args_to_strings does not yet handle ast node type '%s' (%d)",
             ast_node_type_strings[node->type], node->type);
      // TODO: cleanup strings that were allocated
      return err_fail;
    }

  }
  errorf("run_args_to_strings not implemented");
  return err_fail;
}


static err_t run_run2(struct interpreter *interpreter, struct ast_node *node,
                      struct string program_string, const char *program_file)
{
  char **argv = malloc_or_exit(sizeof(char*) * node->run.args.size);
  //size_t total_length = 0;
  for (size_t i = 0; i < node->run.args.size; i++) {

  }

  errorf("interpret run directive for '%s' not implemented", program_file);
  free(argv);
  return err_fail;
}
static err_t run_run(struct interpreter *interpreter, struct ast_node *node)
{
  if (node->run.args.size == 0) {
    errorf("the 'run' directive must have one or more arguments");
    return err_fail;
  }

  // interpret all the arguments as strings
  struct string_vector arg_strings;
  if (string_vector_alloc(&arg_strings, node->run.args.size)) {
    errorf("failed to allocate memory for argument strings (e=%d)", errno);
    return err_fail;
  }
  if (run_args_to_strings(interpreter, &node->run.args, &arg_strings))
    return err_fail; // error already logged
  
  struct ast_node *program_node = ast_node_vector_get(&node->run.args, 0);
  struct string program_string;
  if (as_run_arg(program_node, &program_string))
    return err_fail;
  if (program_string.length == 0) {
    errorf("the first argument of the 'run' directive cannot be an empty string");
    return err_fail;
  }

  // This is a malloc'd NULL-terminated string
  struct string program_file_string;
  if (contains_dir_separator(program_string)) {
    program_file_string = make_string(malloc_str_to_cstr(program_string), program_string.length);
    if (!program_file_string.ptr) {
      errorf("malloc for program_file string failed (e=%d)", errno);
      return err_fail;
    }
  } else {
    program_file_string.ptr = NULL;
    if (which(program_string, &program_file_string))
      return err_fail; // error already logged
    if (program_file_string.ptr == NULL) {
      errorf("cannot find program '%.*s'", (unsigned)program_string.length, program_string.ptr);
      return err_fail;
    }
  }

  err_t result = run_run2(interpreter, node, program_string, program_file_string.ptr);
  free((void*)program_file_string.ptr);
  return result;
}

static err_t run_interpreter(struct interpreter *interpreter)
{
  for (size_t node_index = 0; node_index < ast_node_vector_size(&interpreter->file->nodes); node_index++) {
    struct ast_node *node = ast_node_vector_get(&interpreter->file->nodes, node_index);
    switch (node->type) {
    case AST_NODE_CALL:
      if (run_call(interpreter, node))
        return err_fail;
      break;
    case AST_NODE_RUN:
      if (run_run(interpreter, node))
        return err_fail;
      break;
    default:
      errorf("interpret_script: not implemented for node type '%s' (%d)",
             ast_node_type_strings[node->type], node->type);
      return err_fail;
    }
  }
  return err_pass;
}

err_t interpret_script(struct script_file *file)
{
  struct interpreter interpreter = {file};
  return run_interpreter(&interpreter);
}
