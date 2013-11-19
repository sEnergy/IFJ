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
#define KEYWORD_NUMBER 8

typedef struct buffer_struct
{
    unsigned long long int max_length;
    unsigned long long int position;
    char* data;
}* BUFFER_STRUCT;

/* Comment this function */
int lex_analyzer (FILE *input, int *token_id, BUFFER_STRUCT buffer);

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
