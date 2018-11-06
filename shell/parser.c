#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "parser.h"
#include "command.h"
#include "io.h"

// a command parser
struct parser {
  // the list of commands (separated by pipe)
  Command *commands;
  // the user input
  char *input;
  // the saved stdin file descriptor
  int in;
  // the saved stdout file descriptor
  int out;
};

// allocates a new parser into memory
Parser *parser_new() {
  Parser *value = (Parser*) malloc(sizeof(Parser));
  if (value == NULL) {
    abort();
  }
  value->commands = NULL;
  value->input = NULL;
  value->in = -1;
  value->out = -1;
  return value;
}

// free a parser from memory
void parser_clear(Parser *parser) {
  command_delete_list(parser->commands);
  
  if (parser->input) {
    free(parser->input);
  }
  
  if (parser->in != -1) {
    close(parser->in);
  }
  
  if (parser->out != -1) {
    close(parser->out);
  }
}

void parser_delete(Parser *parser) {
  parser_clear(parser);

  free(parser);
}

// copies standard in/out to parser
static int parser_save_io(Parser *parser) {
  if (parser == NULL) {
    return -1;
  }

  parser->in = dup(STDIN_FILENO);

  if (parser->in == -1) {
    perror("dup");
    return -1;
  }

  parser->out = dup(STDOUT_FILENO);
  
  if (parser->out == -1) {
    perror("dup");
    return -1;
  }

  return 0;
}

// restores previously saved std in/out from parser
static int parser_restore_io(Parser *parser) {
  if (parser == NULL) {
    return -1;
  }

  if (io_redirect_in(parser->in) == -1) {
    return -1;
  }

  if (io_redirect_out(parser->out) == -1) {
    return -1;
  }

  return 0;
}

// gets input from stdin without a line ending
static size_t parser_get_input(Parser *parser) {
  size_t n = 0;

  // get the next input
  ssize_t size = getline(&parser->input, &n, stdin);

  // error check, could be EOF
  if (size == -1) {
    perror("getline");
    return -1;
  }

  // remove the line ending
  parser->input[size-1] = '\0';

  return 0;
}

// sets the parameter argument by either
// stripping a symbol from the argument or
// strtok'ing the next argument
//
// returns 0 on success, -1 on error (no arg)
static int parser_strip_symbol(char **arg) {
  char *next = NULL;

  if (arg == NULL) {
    return -1;
  }

  next = *arg;

  if (strlen(next) > 1) {
    *arg = next + 1;
    return 0;
  }

  next = strtok(NULL, " ");

  if (next == NULL) {
    return -1;
  }

  *arg = next;

  return 0;
}

// parses stdin to a list of commands separated by pipes
int parser_read(Parser *parser) {
  Command *value = NULL;
  char *arg = NULL;
  int n = 0;

  if (parser == NULL) {
    return -1;
  }

  // get the next line
  if (parser_get_input(parser)) {
    return -1;
  }

  // no input? short circuit
  if (parser->input == NULL || *parser->input == '\0') {
    puts("No input");
    return -1;
  }

  if (parser_map_commands(parser)) {
    puts("Error in input");
    return -1;
  }
  return 0;
}

int parser_finish_word(Parser *parser, int n, int redir_in, int redir_out, char *start, char *end) {

  if (parser == NULL) {
    return -1;
  }

  const size_t len = end - start;
  char word[len + 1] = {0};

  strncpy(word, len, start);

  if (redir_out) {
      command_set_file_output(value, word, redir_out == PARSER_APPEND_OUTPUT);
      return 0;
  }

  if (redir_in) {
    command_set_file_input(value, word, redir_in == PARSER_APPEND_INPUT);
    return 0;
  }

  command_set_arg(value, word);
  return 0;
}

int parser_map_commands(Parser *parser) {

  char *start = NULL;
  int redir_in = 0;
  int redir_out = 0;
  int n = 0;

  if (parser == NULL) {
    return -1;
  }

  // create a new command list for the parser
  parser->commands = value = command_new();

  for (char *it = parser->input; it; ++it) {
    switch(*it) {
      case '\'':
      case '"':
        if (start == NULL) {
          // start new word
          start = it + 1;
          continue;
        }

        if (start == it) {
          // add word to commands
          parser_finish_word(value, n++, redir_in, redir_out, start, it);
          continue;
        } 
        break;
      case '>':
        if (start == NULL) {
          redir_out = (redir_out == PARSER_REDIRECT_OUTPUT) ? 
            PARSER_APPEND_OUTPUT : PARSER_REDIRECT_OUTPUT;
        }
        break;

      case '<':
        // redirect or append input
        if (start == NULL) {
          redir_in = (redir_in & PARSER_REDIRECT_INPUT) ?
            PARSER_APPEND_INPUT : PARSER_REDIRECT_INPUT;
        }
        break;

      case '|':
        // pipe output
        // finalize current command arg list
        command_set_arg(value, n, NULL);
        n = 0;
        // create new command in list 
        value = command_next(value);
        break;

      default:
        if (isspace(*it)) {
          if (start == NULL) {
            start = it + 1;
            continue;
          }
          
          if (isspace(start)) {
            // add word to commands
            parser_finish_word(value, n++, redir_in, redir_out, start, it);

            // skip remaining spaces
            while(isspace(*it)) {
              ++it;
            }
            continue;
          }
        }
        break;
    }
  }

  return 0;
}

// interprets the parser
int parser_interpret(Parser *parser) {

  // save the standard io
  if (parser_save_io(parser)) {
    return -1;
  }

  // interpret the command list
  if (command_interpret(parser->commands)) {
    return -1;
  }

  // restore the standard io
  if (parser_restore_io(parser)) {
    return -1;
  }

  return 0;
}

