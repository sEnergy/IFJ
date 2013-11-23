/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            syntax_analyzer.h
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Header file  of Syntax analyzer for team project
 *                      of IFJ course.
 *
*******************************************************************************/

#ifndef IFJ_SYNTAX_INCLUDED
#define IFJ_SYNTAX_INCLUDED

#include "lex_analyzer.h"

#define IFJ_SYNTAX_DEF_BUFF_SIZE 20
#define IFJ_OPEN_TAG_SIZE 6

/* Enumeration of non-terminals */
enum non_terminals
{
    IFJ_NTERM_BODY = 0,
    IFJ_NTERM_PARAM_LIST,
    IFJ_NTERM_PARAM_END,
    IFJ_NTERM_STAT_LIST,
    IFJ_NTERM_STAT,
    IFJ_NTERM_ELSE,
    IFJ_NTERM_INPUT_PARAM_LIST,
    IFJ_NTERM_INPUT_PARAM_END,
    IFJ_NTERM_TERM,
    IFJ_NTERM_EXPRESSION,
    IFJ_NTERM_OPERATOR
};

int buffer_init(BUFFER_STRUCT buffer);
void buffer_clear (BUFFER_STRUCT buffer);
int check_open_tag (FILE* input);
int syntax_analyzer (char* input_filename);

#endif

/*** End of file syntax_analyzer.h ***/
