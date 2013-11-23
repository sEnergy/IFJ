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

int buffer_init(BUFFER_STRUCT buffer);
int check_open_tag (FILE* input);
int syntax_analyzer (char* input_filename);

#endif

/*** End of file syntax_analyzer.h ***/
