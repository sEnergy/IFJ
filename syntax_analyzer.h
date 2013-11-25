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

#include <stdbool.h>
#include "lex_analyzer.h"

#define IFJ_SYNTAX_DEF_BUFF_SIZE 20
#define IFJ_OPEN_TAG_SIZE 6

// initicalizes buffer before first use
int buffer_init(BUFFER_STRUCT buffer);

// reset buffer for another use
void buffer_clear (BUFFER_STRUCT buffer);

// test the open tag - must be "<?php "
int check_open_tag (FILE* input);

/*
 * Main function of syntax analyzer - handles file, calls all other functions
 * and so on...
 */
int syntax_analyzer (char* input_filename);

/*
 * In infitine loop it reads from input source file and checks their order.
 * Only SYNTAX ERROR or EOF breaks cycle (and of course, INTERNAL ERROR).
 */
int check_syntax (FILE *input, int *token_id, BUFFER_STRUCT token_data);

/*
 * This function is called ither with "if", "while" or "variable" token loaded,
 * or i may be start of "return" expression (in this case, i need to read "return"
 * token first)
 */
int check_statement (FILE *input, int *token_id, BUFFER_STRUCT token);

/*
 * Checks syntax of whole function declaration - identificator, parameter list
 * in braces and statement list in curly braces.
 */
int check_function_declaration (FILE *input, int *token_id, BUFFER_STRUCT token);

/*
 * Check correct function call - this is called after id token is checked, so
 * it checks only if there is input parameter list in between braces followed
 * by semicolon.
 */
int check_function_call (FILE *input, int *token_id, BUFFER_STRUCT token);

/*
 * Function checks the syntax of statement list (scope) between curly braces "{}".
 * This kind of statement list can be found after parameter list in function
 * definition, in cycles or in if-else control construction.
 */
int check_stat_list (FILE *input, int *token_id, BUFFER_STRUCT token);

/*
 * Checks parameter list for fuction definiton - statement list betwenn curly
 * braces.
 */
int check_param_list (FILE *input, int *token_id, BUFFER_STRUCT token);

/*
 * This function chcecks the syntax of condition in format: (<EXPRESSION>).
 * That means, there must be openint brace '(', expression (that may also contain)
 * some braces and it all end with closing brace ')'. Syntax of expression and
 * number of braces is controlled by check_expression().
 */
int check_condition (FILE *input, int *token_id, BUFFER_STRUCT token);

/*
 * This function is called after "if" keyword is found. It checks if there
 * is correct conditon, statement list, "else" keyword and another statement
 * list.
 */
int check_if_else (FILE *input, int *token_id, BUFFER_STRUCT token);

// Checks syntax of while construction - condition folowed by statement list
int check_while (FILE *input, int *token_id, BUFFER_STRUCT token);

/*
 * Reads whole expression, saves all tokens into list and than calls function,
 * which test the syntax of the expression.
 */
int check_expression (FILE *input, int *token_id, BUFFER_STRUCT token,
    int end_token, bool extra_read);

// if token is variable or literal, return true
bool is_terminal (int token_id);

// if token id corresponds with operator that could be in expression, return true
bool is_operator (int token_id);

#endif

/*** End of file syntax_analyzer.h ***/
