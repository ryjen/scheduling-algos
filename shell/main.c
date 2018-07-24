#include <stdio.h>
#include "parser.h"

int main() {

  Parser *parser = parser_new();

  // keep running, use ctrl-c to quit
  while(1) {

    // print prompt
    printf("> ");

    // read command from standard input
    if (parser_read(parser)) {
      return 1;
    }

    // interpret the commands and run
    if (parser_interpret(parser)) {
      return 1;
    }

    // cleanup
    parser_clear(parser);
  }

  return 0;
}


