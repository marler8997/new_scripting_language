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
#include "ast.h"

const char *ast_node_type_strings[] = {
  "string",
  "call",
  "run",
};


static void indent(unsigned length)
{
  const char *ten_spaces = "          ";
  while (length >= 10) {
    printf("%s", ten_spaces);
    length -= 10;
  }
  if (length > 0) {
    printf("%.*s", length, ten_spaces);
  }
}

static void dump_nodes_helper(struct ast_node_vector *nodes, unsigned depth);

static void dump_node(struct ast_node *node, unsigned depth)
{
  switch (node->type) {
  case AST_NODE_STRING:
    indent(depth);
    // TODO: don't print escaped version
    printf("\"%.*s\"\n", (unsigned)node->string.escaped.length, node->string.escaped.ptr);
    break;
  case AST_NODE_CALL:
    indent(depth);
    printf("call %.*s(\n", (unsigned)node->call.function.length, node->call.function.ptr);
    dump_nodes_helper(&node->call.args, depth + 1);
    indent(depth);
    printf(")\n");
    break;
  default:
    printf("Error: print ast node '%s' (%d) not implemented\n",
           ast_node_type_strings[node->type], node->type);
    exit(1);
    break;
  }
}

static void dump_nodes_helper(struct ast_node_vector *nodes, unsigned depth)
{
  for (size_t i = 0; i < ast_node_vector_size(nodes); i++) {
    dump_node(ast_node_vector_get(nodes, i), depth);
  }
}

void dump_nodes(struct ast_node_vector *nodes)
{
  dump_nodes_helper(nodes, 0);
}
