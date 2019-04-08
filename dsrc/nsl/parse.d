module nsl.parse;

import nsl.scriptfile : ScriptFile;

/*
size_t estimateMaxAstNodeCount(size_t text_length)
{
  // maybe max 1 ast_node per 50 characters?
  return (unsigned)(text_length / 50);
}
*/

void parse(ScriptFile file)
{
    //assert(!file.astNodes, "codebug: parse was called on a script file that was already parsed");
    Parser parser = Parser(&file);
    parser.parse();
}

private struct Parser
{
    ScriptFile* file;
    immutable(char)* next;
    uint lineNumber;

    size_t fileOffsetAt(const(char) *ptr) const
    {
        return cast(void*)ptr - cast(void*)file.mmap.ptr;
    }
    size_t currentFileOffset() const { return fileOffsetAt(next); }

/+
    static struct ast_node *alloc_node(struct parser *parser)
    {
      // for now we just use malloc for each node, but this is probably wasteful
      struct ast_node *node = (struct ast_node*)malloc_or_exit(sizeof(struct ast_node));
      return node;
    }

    static err_t parse_expressions(struct parser *parser, enum token_type end_token, struct ast_node_vector *nodes)
    {
      for (;;) {
        struct token token;
        if (read_and_consume_token(parser, &token))
          return err_fail;
        if (token.type == end_token)
          return err_pass;

        switch (token.type) {
        case TOKEN_TYPE_STRING:
          {
            struct ast_node *node = alloc_node(parser);
            init_ast_node(node, AST_NODE_STRING);
            node->string.escaped = token.string.escaped;
            if (ast_node_vector_add(nodes, node)) {
              errorf("TODO: write good error message");
              return err_fail;
            }
          }
          break;
        case TOKEN_TYPE_EOF:
          return unexpected_token(parser, end_token, &token);
        default:
          errorf("parse_expressions: token '%s' not implemented", token_type_strings[token.type]);
          return err_fail;
          break;
        }
      }
    }

    static err_t parse_call(struct parser *parser, struct ast_node *node)
    {
      struct token id_token;
      if (expect_and_return_token(parser, TOKEN_TYPE_ID, &id_token))
        return err_fail;

      //printf("[DEBUG] calling function '%.*s'\n", (unsigned)id_token.source.length, id_token.source.ptr);
      if (expect_and_ignore_token(parser, TOKEN_TYPE_LEFT_PAREN))
        return err_fail;

      struct ast_node_vector arg_nodes = {0};
      if (parse_expressions(parser, TOKEN_TYPE_RIGHT_PAREN, &arg_nodes))
        return err_fail;

      init_ast_node(node, AST_NODE_CALL);
      node->call.function = id_token.source;
      node->call.args = arg_nodes;
      return err_pass;
    }

    void parse(struct ast_node *node)
    {
      if (expect_and_ignore_token(parser, TOKEN_TYPE_LEFT_PAREN))
        return err_fail;
      struct ast_node_vector arg_nodes = {0};
      if (parse_expressions(parser, TOKEN_TYPE_RIGHT_PAREN, &arg_nodes))
        return err_fail;
      if (arg_nodes.size == 0) {
        errorf("%s(line %d) the 'run' directive must have at least one argument",
               parser->file->filename, parser->line_number);
        return err_fail;
      }

      for (;;) {
        if (read_token(parser))
          return err_fail;

        // TODO: check if it is one of the keywords that can appear in a run command
        if (parser->token.type != TOKEN_TYPE_STRING)
          break;
        errorf("not impl");
        return err_fail;
      }

      init_ast_node(node, AST_NODE_RUN);
      node->run.args = arg_nodes;
      return err_pass;
    }
    +/
    static void parse()
    {
        assert(0, "parse not impl");
    /+
      for (;;) {
        /*
        printf("--------------------------------------------------------------------------------\n");
        printf("Current Nodes(%llu):\n", (unsigned long long)ast_node_vector_size(&parser->file->nodes));
        dump_nodes(&parser->file->nodes);
        printf("parsing next node...\n");
        */

        struct token token;
        if (read_and_consume_token(parser, &token)) {
          // error already logged
          return err_fail;
        }

        struct ast_node *new_node = NULL;
        switch(token.type) {
        case TOKEN_TYPE_ID:
          {
            //printf("[DEBUG] got id token '%.*s'\n", (unsigned)token.source.length, token.source.ptr);
            enum keyword keyword;
            if (!try_as_keyword(token.source, &keyword)) {
              errorf("%s(%d) expected a keyword but got '%.*s'",
                     parser->file->filename, parser->line_number,
                     (unsigned)token.source.length, token.source.ptr);
              return err_fail;
            }
            switch(keyword) {
            case KEYWORD_CALL:
              new_node = alloc_node(parser);
              if (parse_call(parser, new_node))
                return err_fail; // error already logged
              break;
            case KEYWORD_RUN:
              new_node = alloc_node(parser);
              if (parse_run(parser, new_node))
                return err_fail; // error already logged
              break;
            default:
              errorf("code bug: unhandled keyword enum value %d", keyword);
              return err_fail;
            }
          }
          break;
        case TOKEN_TYPE_EOF:
          return err_pass;
        default:
          errorf("code bug: unhandled token type enum value %d", token.type);
          return err_fail;
        }
        if (new_node) {
          if (ast_node_vector_add(&parser->file->nodes, new_node)) {
            errorf("TODO: write good error message");
            return err_fail;
          }
        }
      }
      +/
    }
}
