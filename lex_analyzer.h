/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            lex_analyzer.h
 * Author:              Marcel Fiala
 *                      Attila Večerek
 * Encoding:            UTF-8
 *
 * Description:         Header file  of Lexical analyzer for team project
 *                      of IFJ course.
 *
*******************************************************************************/
#ifndef IFJ_LEX_INCLUDED
#define IFJ_LEX_INCLUDED

// defined number of keywords
#define KEYWORD_NUMBER 9
#include "token_list.h"

typedef struct buffer_struct
{
    size_t max_length;
    size_t position;
    char* data;
}* BUFFER_STRUCT;

int lex_analyzer (FILE *input, TokenPtr token, BUFFER_STRUCT buffer);
int write_c(BUFFER_STRUCT buffer, char c);
void buffer_next_token(BUFFER_STRUCT buffer);

/* Enumeration of states in INT and DOUBLE section */
enum state
{
    S_INT, // Integer
    S_FDBL, // First number of double
    S_E_EXP, // 'e' or 'E' part of exponent
    S_EXP, // Exponent's number(s)
};

#endif

/*** End of file lex_analyzer.h ***/
