/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            syntax_analyzer.c
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Source file of Syntax analyzer for team project
 *                      of IFJ course.
 *
*******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "token_id.h"
#include "syntax_analyzer.h"
#include "errors.h"
#include "token_list.h"

// initicalizes buffer before first use
int buffer_init(BUFFER_STRUCT buffer)
{
    buffer->data = (char*) malloc(IFJ_SYNTAX_DEF_BUFF_SIZE*sizeof(char));
    if (buffer->data == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    else
    {
        buffer->data[0] = '\0';
        buffer->max_length = IFJ_SYNTAX_DEF_BUFF_SIZE;
        buffer->position = 0;
    }
    return 0;
}

// reset buffer for another use
void buffer_clear (BUFFER_STRUCT buffer)
{
    buffer->position++;
}

// test the open tag - must be "<?php "
int check_open_tag (FILE* input)
{
    unsigned char open_tag_size = IFJ_OPEN_TAG_SIZE;
    int code;

    char *open_tag = (char*) malloc(open_tag_size*sizeof(char));

    if (open_tag == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    while (open_tag_size--) // intialization of array
    {
        open_tag[open_tag_size] = '\0';
    }

    // test itself
    if ((fscanf(input, "%5c", open_tag) != 1) || strcmp(open_tag, "<?php") != 0
            || !isspace(fgetc(input)))
    {
        code = IFJ_ERR_SYNTAX;
    }
    else
    {
        code = 0;
    }

    free(open_tag);
    return code;
}

/*
 * Main function of syntax analyzer - handles file, calls all other functions
 * and so on...
 */
int syntax_analyzer (char* input_filename)
{
    int code, token_id;
    FILE* input;
    BUFFER_STRUCT token_content = NULL; // structure for token data

    // open source file
    if ((input = fopen(input_filename, "r")) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    // testing the open tag "<?php "
    if ((code = check_open_tag (input)) != 0)
    {
        fclose(input);
        return code;
    }

    // initialization of structure for token data
    if ((token_content = malloc(sizeof(struct buffer_struct))) == NULL)
    {
        fclose(input);
        return IFJ_ERR_INTERNAL;
    }
    else
    {
        if (buffer_init(token_content) == IFJ_ERR_INTERNAL)
        {
            fclose(input);
            free(token_content);
            return IFJ_ERR_INTERNAL;
        }
    }

    // start syntax analyze itself
    code = check_syntax(input, &token_id, token_content);

    // free all allocated memory, close file
    fclose(input);
    free(token_content->data);
    free(token_content);

    return code;
}

/*
 * In infitine loop it reads from input source file and checks their order.
 * Only SYNTAX ERROR or EOF breaks cycle (and of course, INTERNAL ERROR).
 */
int check_syntax (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    while (1)
    {
        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
        else if (*token_id == IFJ_T_KEYWORD)
        {
            if (strcmp(token->data, "function\0") == 0) // function declaration
            {
                if((code = check_function_declaration (input, token_id, token)) != 0)
                {
                    return code;
                }
            }
            else if ((strcmp(token->data, "if\0") != 0)
                        && (strcmp(token->data, "while\0") != 0)
                        && (strcmp(token->data, "return\0") != 0))
            {
                return IFJ_ERR_SYNTAX;
            }
            else // return statement, if-else construction or while cycle
            {
                if((code = check_statement (input, token_id, token)) != 0)
                {
                    return code;
                }
            }
        }
        else if (*token_id == IFJ_T_EOF) // in"body", EOF is legal
        {
            return 0;
        }
        else if (*token_id == IFJ_T_VARIALBE) // assign statement
        {
            if((code = check_statement (input, token_id, token)) != 0)
            {
                return code;
            }
        }
        else
        {
            return IFJ_ERR_SYNTAX;
        }
    }
}

/*
 * Checks syntax of whole function declaration - identificator, parameter list
 * in braces and statement list in curly braces.
 */
int check_function_declaration (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    // first, there must be indentificator
    if ((code = lex_analyzer(input, token_id, token)) != 0)
    {
        return code;
    }
    else if (*token_id != IFJ_T_ID)
    {
        return IFJ_ERR_SYNTAX;
    }

    // test of parameter list
    if ((code = check_param_list (input, token_id, token)) != 0)
    {
        return code;
    }

    // after parameter list, there must be corrent statement list
    if ((code = check_stat_list (input, token_id, token)) != 0)
    {
        return code;
    }

    return 0;
}

/*
 * Checks parameter list for fuction definiton - statement list betwenn curly
 * braces.
 */
int check_param_list (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    // check, if there is openin brace '('
    if ((code = lex_analyzer(input, token_id, token)) != 0)
    {
        return code;
    }
    else if (*token_id != IFJ_T_LB)
    {
        return IFJ_ERR_SYNTAX;
    }

    if ((code = lex_analyzer(input, token_id, token)) != 0)
    {
        return code;
    }
    else if (*token_id == IFJ_T_VARIALBE) // first variable
    {
        while (1)
        {
            if ((code = lex_analyzer(input, token_id, token)) != 0)
            {
                return code;
            }
            else if (*token_id == IFJ_T_RB) // legal end of <PARAM-LIST>
            {
                break;
            }
            /*
             * <PARAM-LIST> not finished, must be separator
             * and variable here (then cycle repeats)
             */
            else if (*token_id == IFJ_T_SEP)
            {
                if ((code = lex_analyzer(input, token_id, token)) != 0)
                {
                    return code;
                }
                else if (*token_id == IFJ_T_VARIALBE)
                {
                    continue;
                }
                else
                {
                    return IFJ_ERR_SYNTAX;
                }
            }
            else
            {
                return IFJ_ERR_SYNTAX;
            }
        }
    }
    else if (*token_id != IFJ_T_RB)
    {
        return IFJ_ERR_SYNTAX;
    }

    return 0;
}

/*
 * Reads whole expression, saves all tokens into list and than calls function,
 * which test the syntax of the expression.
 */
int check_expression (FILE *input, int *token_id, BUFFER_STRUCT token,
                        int end_token, bool extra_read)
{
    int code;
    int rb_needed = 0;

    // if current token does ot belong to expression, we need to read one
    if (!extra_read)
    {
        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
    }

    TokenList t_list;
    TL_Init(&t_list);

    do // until end of file
    {
        if (*token_id == end_token && rb_needed == 0) // must be end token and 0
        {                                             // needed closing brackets
            /*
             * There is scope of cycle terminal condition. If expression is
             * empty, it is syntax error. Else, cycle just breaks and control
             * continues - to check expression witch precedence analysis.
             */
            if (!TL_IsEmpty(&t_list)) // not empty -> OK
            {
                break;
            }
            else // empty -> SYNTAX ERROR
            {
                TL_Dispose(&t_list);
                return IFJ_ERR_SYNTAX;
            }
        }
        // if valid token (literal or variable) ir read, save it to thel list
        else if (is_operator(*token_id) || is_terminal(*token_id)
            || *token_id == IFJ_T_RB || *token_id == IFJ_T_LB)
        {
            if (*token_id == IFJ_T_LB)
            {
                rb_needed++; // every '(' means there needs to be another ')'
            }
            else if (*token_id == IFJ_T_RB)
            {
                rb_needed--; // every ')' decrements number of needed ')'
            }

            code = TL_Insert(&t_list, *token_id, token);
        }
        else // everything else -> SYNTAX ERROR
        {
            code = IFJ_ERR_SYNTAX;
        }

        if (code != 0) // if error occurs, free list and return error
        {
            TL_Dispose(&t_list);
            return code;
        }
        // else read another token
        else if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }

    } while (*token_id != IFJ_T_EOF);

    /* FROM THIS PLACE, CALL EXPRESSION SYNTAX CHECK*/

    TL_Dispose(&t_list);
    return 0;
}

/*
 * This function is called ither with "if", "while" or "variable" token loaded,
 * or i may be start of "return" expression (in this case, i need to read "return"
 * token first)
 */
int check_statement (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    if (*token_id == IFJ_T_VARIALBE) // possible start of assign
    {
        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
        else if (*token_id == IFJ_T_ASSIGN) // there must be '=' after variable
        {
            if ((code = lex_analyzer(input, token_id, token)) != 0)
            {
                return code;
            }
            else if (*token_id == IFJ_T_ID)
            {
                // assign of return value
                return check_function_call (input, token_id, token);
            }
            else
            {
                // assign from expression
                return check_expression (input, token_id, token, IFJ_T_SEMICOLON, true);
            }
        }
        else
        {
            return IFJ_ERR_SYNTAX;
        }
    }
    else if (strcmp(token->data, "if\0") == 0)
    {
        return check_if_else (input, token_id, token);
    }
    else if (strcmp(token->data, "while\0") == 0)
    {
        return check_while (input, token_id, token);
    }
    else if (strcmp(token->data, "return\0") == 0)
    {
        // there must be expression ending with semicolon
        return check_expression (input, token_id, token, IFJ_T_SEMICOLON, false);
    }

    return IFJ_ERR_SYNTAX; // if control gets here, it is definitely error
}

/*
 * Check correct function call - this is called after id token is checked, so
 * it checks only if there is input parameter list in between braces followed
 * by semicolon.
 */
int check_function_call (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    if ((code = lex_analyzer(input, token_id, token)) != 0)
    {
        return code;
    }
    else if (*token_id == IFJ_T_LB) // <INPUT-PARAM-LIST> start: '('
    {
        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
        else if (is_terminal(*token_id)) // first variable
        {
            while (1)
            {
                if ((code = lex_analyzer(input, token_id, token)) != 0)
                {
                    return code;
                }
                else if (*token_id == IFJ_T_RB) // legal end of <INPUT-PARAM-LIST>
                {
                    break;
                }
                /*
                 * <INPUT-PARAM-LIST> not finished, must be separator
                 * and variable here (then cycle repeats)
                 */
                else if (*token_id == IFJ_T_SEP)
                {
                    if ((code = lex_analyzer(input, token_id, token)) != 0)
                    {
                        return code;
                    }
                    else if (is_terminal(*token_id))
                    {
                        continue;
                    }
                    else
                    {
                        return IFJ_ERR_SYNTAX;
                    }
                }
                else
                {
                    return IFJ_ERR_SYNTAX;
                }
            }
        }
        else if (*token_id != IFJ_T_RB)
        {
            return IFJ_ERR_SYNTAX;
        }

        // ater input param list, there must be semicolon
        if ((code = lex_analyzer (input, token_id, token)) != 0)
        {
            return code;
        }
        else if (*token_id != IFJ_T_SEMICOLON)
        {
            return IFJ_ERR_SYNTAX;
        }
    }

    return 0;
}

// Checks syntax of while construction - condition folowed by statement list
int check_while (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    if ((code = check_condition (input, token_id, token)) != 0)
    {
        return code;
    }

    if ((code = check_stat_list (input, token_id, token)) != 0)
    {
        return code;
    }

    return 0;
}

/*
 * This function is called after "if" keyword is found. It checks if there
 * is correct conditon, statement list, "else" keyword and another statement
 * list.
 */
int check_if_else (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    // condition
    if ((code = check_condition (input, token_id, token)) != 0)
    {
        return code;
    }

    // than first statement list for case conditon == true
    if ((code = check_stat_list (input, token_id, token)) != 0)
    {
        return code;
    }

    // after that, there must be "else" keyword
    if ((code = lex_analyzer (input, token_id, token)) != 0)
    {
        return code;
    }
    else if (strcmp(token->data, "else\0") != 0)
    {
        return IFJ_ERR_SYNTAX;
    }

    // and statement list for case conditon != true
    if ((code = check_stat_list (input, token_id, token)) != 0)
    {
        return code;
    }

    // if everything passed, the if-else control construction is fine
    return 0;
}

/*
 * Function checks the syntax of statement list (scope) between curly braces "{}".
 * This kind of statement list can be found after parameter list in function
 * definition, in cycles or in if-else control construction.
 */
int check_stat_list (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    // printf("\npre- token ID: %d", *token_id);

    if ((code = lex_analyzer(input, token_id, token)) != 0)
    {
        return code;
    }
    else if (*token_id == IFJ_T_LCB) // '{' - start of scope
    {
        //printf("overeni scope: ");

        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
        // this is illegal - the scope must not be empty ("{}")
        else if (*token_id == IFJ_T_RCB)
        {
            return IFJ_ERR_SYNTAX;
        }

        do // until the end of scope '}'
        {
            /*
             * Only variable, if-else, while or return is start of statement.
             * Everything else is means SYNTAX ERROR
             */
            if ((strcmp(token->data, "if\0") != 0)
                        && (strcmp(token->data, "while\0") != 0)
                        && (strcmp(token->data, "return\0") != 0)
                        && (*token_id != IFJ_T_VARIALBE))
            {
                return IFJ_ERR_SYNTAX;
            }
            else
            {
                if((code = check_statement (input, token_id, token)) != 0)
                {
                    return code;
                }
            }

            // read of next token
            if ((code = lex_analyzer(input, token_id, token)) != 0)
            {
                return code;
            }

        } while (*token_id != IFJ_T_RCB);
    }
    else // scope did not start with '}'
    {
        //printf("token ID: %d", *token_id);
        return IFJ_ERR_SYNTAX;
    }

    return 0;
}

/*
 * This function chcecks the syntax of condition in format: (<EXPRESSION>).
 * That means, there must be openint brace '(', expression (that may also contain)
 * some braces and it all end with closing brace ')'. Syntax of expression and
 * number of braces is controlled by check_expression().
 */
int check_condition (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    if ((code = lex_analyzer(input, token_id, token)) != 0)
    {
        return code;
    }
    else if (*token_id == IFJ_T_LB) // there must be '(' before expression
    {
        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
        else // expression
        {
           return check_expression (input, token_id, token, IFJ_T_RB, true);
        }
    }

    return IFJ_ERR_SYNTAX;
}

// if token is variable or literal, return true
bool is_terminal (int token_id)
{
    if (IFJ_T_VARIALBE <= token_id && token_id <= IFJ_T_STRING)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// if token id corresponds with operator that could be in expression, return true
bool is_operator (int token_id)
{
    if ((IFJ_T_ASSIGN <= token_id && token_id <= IFJ_T_NOT_SUPER_EQUAL))
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*** End of file syntax_analyzer.c ***/
