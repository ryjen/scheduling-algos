#include <stdio.h>
#include "parser.h"

int main() {

  // keep running, use ctrl-c to quit
  while(1) {

    // create a new parser
    Parser *parser = parser_new();

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
    parser_delete(parser);
  }

  return 0;
}


