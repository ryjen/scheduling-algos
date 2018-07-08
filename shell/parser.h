#ifndef RYJEN_PARSER_H
#define RYJEN_PARSER_H

// the parser type
typedef struct parser Parser;

// constructor
Parser *parser_new();

// destructor
void parser_delete(Parser *);

// reads input from standard input
int parser_read(Parser *);

// interprets commands and executes
int parser_interpret(Parser *);

#endif

