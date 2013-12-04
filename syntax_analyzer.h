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
#include "token_list.h"
#include "interpreter.h"

#define IFJ_SYNTAX_DEF_BUFF_SIZE 20
#define IFJ_OPEN_TAG_SIZE 6


//generate AST using PSA table for defined rules
int PSA(TokenList* list, BUFFER_STRUCT big_string, Stack_t* garbages);

//Return left-closest term to active on input
TokenPtr Lclosest_term(TokenList* list);

//apply rule for PSA
int work(Stack_t* stack, TokenList* list);

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
int check_syntax (FILE *input, TokenPtr* token_oldPtr, function_hashtablePtr* HTable, BUFFER_STRUCT big_string, Stack_t* garbages);

/*
 * This function is called ither with "if", "while" or "variable" token loaded,
 * or i may be start of "return" expression (in this case, i need to read "return"
 * token first)
 */
int check_statement (FILE *input, TokenPtr* token_old, BUFFER_STRUCT big_string, Stack_t* garbages);

/*
 * Checks syntax of whole function declaration - identificator, parameter list
 * in braces and statement list in curly braces.
 */
int check_function_declaration (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string, Stack_t* garbages);

/*
 * Check correct function call - this is called after id token is checked, so
 * it checks only if there is input parameter list in between braces followed
 * by semicolon.
 */
int check_function_call (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string, Stack_t* garbages);

/*
 * Function checks the syntax of statement list (scope) between curly braces "{}".
 * This kind of statement list can be found after parameter list in function
 * definition, in cycles or in if-else control construction.
 */
int check_stat_list (FILE *input, TokenPtr* token_old, BUFFER_STRUCT big_string, Stack_t* garbages);

/*
 * Checks parameter list for fuction definiton - statement list betwenn curly
 * braces.
 */
int check_param_list (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string, Stack_t* garbages);

/*
 * This function chcecks the syntax of condition in format: (<EXPRESSION>).
 * That means, there must be openint brace '(', expression (that may also contain)
 * some braces and it all end with closing brace ')'. Syntax of expression and
 * number of braces is controlled by check_expression().
 */
int check_condition (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string, Stack_t* garbages);

/*
 * This function is called after "if" keyword is found. It checks if there
 * is correct conditon, statement list, "else" keyword and another statement
 * list.
 */
int check_if_else (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string, Stack_t* garbages);

// Checks syntax of while construction - condition folowed by statement list
//int check_while (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string);

/*
 * Reads whole expression, saves all tokens into list and than calls function,
 * which test the syntax of the expression.
 */
int check_expression (FILE *input, TokenPtr* token_oldPtr,
        unsigned int end_token, bool already_read,BUFFER_STRUCT big_string, Stack_t* garbages);

int check_while (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string, Stack_t* garbages);
// if token is variable or literal, return true
bool is_terminal (TokenPtr token, BUFFER_STRUCT big_string);

// if token id corresponds with operator that could be in expression, return true
bool is_operator (TokenPtr token);

#endif

/*** End of file syntax_analyzer.h ***/
