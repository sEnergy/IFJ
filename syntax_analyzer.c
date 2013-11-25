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
#include <stdio.h>  // pak můžu smazat
#include <stdlib.h>
#include <ctype.h>

#include "token_id.h"
#include "syntax_analyzer.h"
#include "errors.h"
#include "token_list.h"

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

void buffer_clear (BUFFER_STRUCT buffer)
{
    buffer->data[0] = '\0';
    buffer->position = 0;
}

int check_open_tag (FILE* input)
{
    unsigned char open_tag_size = IFJ_OPEN_TAG_SIZE;

    char *open_tag = (char*) malloc(open_tag_size*sizeof(char));

    if (open_tag == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    while (open_tag_size--) open_tag[open_tag_size] = '\0';

    if ((fscanf(input, "%5c", open_tag) != 1) || strcmp(open_tag, "<?php") != 0
            || !isspace(fgetc(input)))
    {
        free(open_tag);
        return IFJ_ERR_SYNTAX;
    }
    else
    {
        free(open_tag);
        return 0;
    }

}

int syntax_analyzer (char* input_filename)
{
    int code = 0;

    // source file to intepret
    FILE* input = fopen(input_filename, "r");
    if (input == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    // testing the open tag "<?php "
    if ((code = check_open_tag (input)) != 0)
    {
        fclose(input);
        return code;
    }

    // buffer fot token content
    BUFFER_STRUCT token_content = NULL;

    if ((token_content = (BUFFER_STRUCT) malloc(sizeof(struct buffer_struct))) == NULL)
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

    int token_id;

    if ((code = check_syntax(input, &token_id, token_content)) != 0)
    {
        fclose(input);
        free(token_content->data);
        free(token_content);
        return code;
    }

    fclose(input);
    free(token_content->data);
    free(token_content);
    return code;
}

int check_syntax (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    while (1) // <BODY>
    {
        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
        else if (*token_id == IFJ_T_KEYWORD)
        {
            if (strcmp(token->data, "function\0") == 0) // function declaration
            {
                if ((code = lex_analyzer(input, token_id, token)) != 0)
                {
                    return code;
                }
                else if (*token_id == IFJ_T_ID) // identificator
                {
                    if ((code = lex_analyzer(input, token_id, token)) != 0)
                    {
                        return code;
                    }
                    else if (*token_id == IFJ_T_LB) // <PARAM-LIST> start: '('
                    {
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

                        /*
                         * Ater <PARAM-LIST>, there must be statement scope. ///////
                         */
                        if ((code = check_stat_list (input, token_id, token)) != 0)
                        {
                            return code;
                        }
                    }
                }
                else
                {
                    return IFJ_ERR_SYNTAX;
                }
            } // end: function declaration
            else if ((strcmp(token->data, "if\0") != 0)
                        && (strcmp(token->data, "while\0") != 0)
                        && (strcmp(token->data, "return\0") != 0))
            {
                return IFJ_ERR_SYNTAX;
            }
            else
            {
                // token is either keyword "if" or "while" -> check rest of statement
                if((code = check_statement (input, token_id, token)) != 0)
                {
                    return code;
                }
            }
        }
        else if (*token_id == IFJ_T_EOF) // in <BODY>, EOF is legal
        {
            return 0;
        }
        else if (*token_id == IFJ_T_VARIALBE)
        { // start of expression
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
 * This function is called ither with "if", "while" or "variable" token loaded,
 * or i may be start of "return" expression (in this case, i need to read "return"
 * token first)
 */
int check_statement (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    if (*token_id == IFJ_T_VARIALBE)
    {
        //printf("bude to 11 nebo 14 - variable\n");

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
              //  printf("\t-nasel jsem identifikator\n");

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

                        /*
                         * Ater <PARAM-LIST>, there must be statement scope. ///////
                         */
                        if ((code = lex_analyzer (input, token_id, token)) != 0)
                        {
                            return code;
                        }

                        if (*token_id != IFJ_T_SEMICOLON)
                        {
                            return 76;
                        }
                    }

                return 0;
            }
            else // possibility of expression
            {
                // printf("variable = EXPRESSION;");
                TokenList t_list;
                TL_Init(&t_list);

                do
                {
                    if (*token_id == IFJ_T_SEMICOLON)
                    {
                        if (t_list.first != NULL)
                        {
                            break;
                        }
                        else
                        {
                            TL_Dispose(&t_list);
                            return 7;
                        }
                    }
                    else if (is_operator(*token_id) || is_terminal(*token_id)
                        || *token_id == IFJ_T_RB || *token_id == IFJ_T_LB)
                    {
                        code = TL_Insert(&t_list, *token_id, token);
                    }
                    else
                    {
                        TL_Dispose(&t_list);
                        return 15;
                    }

                    if (code != 0)
                    {return code; TL_Dispose(&t_list);}


                    if ((code = lex_analyzer(input, token_id, token)) != 0)
                    {
                        return code;
                    }

                } while (*token_id != IFJ_T_EOF);

                TL_Dispose(&t_list);
                //printf("--- OK\n");
            } // zpracování expression
        } // za =
        else
        {
            return IFJ_ERR_SYNTAX;
        }

        return 0;
    }
    else if (strcmp(token->data, "if\0") == 0)
    {
        //printf("---IF---\n");

        if ((code = check_condition (input, token_id, token)) != 0)
        {
            return code;
        }

        if ((code = check_stat_list (input, token_id, token)) != 0)
        {
            return code;
        }

        if ((code = lex_analyzer (input, token_id, token)) != 0)
        {
            return code;
        }

        if (strcmp(token->data, "else\0") != 0)
        {
            return IFJ_ERR_SYNTAX;
        }

        if ((code = check_stat_list (input, token_id, token)) != 0)
        {
            return code;
        }

        return 0;
    }
    else if (strcmp(token->data, "while\0") == 0)
    {
        //printf("---WHILE---");

        if ((code = check_condition (input, token_id, token)) != 0)
        {
            return code;
        }

        //printf("podminka ok,");

        if ((code = check_stat_list (input, token_id, token)) != 0)
        {
            return code;
        }

        return 0;

    }
    else if (strcmp(token->data, "return\0") == 0)
    {
        //printf("return EXPRESSION;");

                TokenList t_list;
                TL_Init(&t_list);

                do
                {
                    if ((code = lex_analyzer(input, token_id, token)) != 0)
                    {
                        return code;
                    }
                    else
                    {
                        if (*token_id == IFJ_T_SEMICOLON)
                        {
                            if (t_list.first != NULL)
                            {
                                break;
                            }
                            else
                            {
                                TL_Dispose(&t_list);
                            return 2;
                            }
                        }
                        else if (is_operator(*token_id) || is_terminal(*token_id)
                            || *token_id == IFJ_T_RB || *token_id == IFJ_T_LB)
                        {
                            code = TL_Insert(&t_list, *token_id, token);
                        }
                        else
                        {
                            TL_Dispose(&t_list);
                            return 2;
                        }

                        if (code != 0)
                        {return code; TL_Dispose(&t_list);}
                    }
                } while (*token_id != IFJ_T_EOF);

                TL_Dispose(&t_list);
               // printf("--- OK\n");
        return 0;
    }

    //printf("\n\ntoken ID: %d", *token_id);
    return 69;
}

int check_condition (FILE *input, int *token_id, BUFFER_STRUCT token)
{
    int code;

    if ((code = lex_analyzer(input, token_id, token)) != 0)
    {
        return code;
    }
    else if (*token_id == IFJ_T_LB) // there must be '(' after variable
    {
        if ((code = lex_analyzer(input, token_id, token)) != 0)
        {
            return code;
        }
        else // possibility of expression
        {
           // printf("variable = EXPRESSION;");
            TokenList t_list;
            TL_Init(&t_list);

            do
            {
                if (*token_id == IFJ_T_RB)
                {
                    if (t_list.first != NULL)
                    {
                        break;
                    }
                    else
                    {
                        TL_Dispose(&t_list);
                        return 7;
                    }
                }
                else if (is_operator(*token_id) || is_terminal(*token_id)
                    || *token_id == IFJ_T_RB || *token_id == IFJ_T_LB)
                {
                    code = TL_Insert(&t_list, *token_id, token);
                }
                else
                {
                    TL_Dispose(&t_list);
                    return 15;
                }

                if (code != 0)
                {return code; TL_Dispose(&t_list);}


                if ((code = lex_analyzer(input, token_id, token)) != 0)
                {
                    return code;
                }

            } while (*token_id != IFJ_T_EOF);

            TL_Dispose(&t_list);
            //printf("--- OK\n");
        } // zpracování if
    } // za =
    else
    {
        return IFJ_ERR_SYNTAX;
    }

    return 0;
}

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

        if (*token_id == IFJ_T_RCB) // '}' - end of scope
        { // in this case illegal - scope cannot be empty
            return IFJ_ERR_SYNTAX;
        }

        do
        {
            if ((strcmp(token->data, "if\0") != 0)
                        && (strcmp(token->data, "while\0") != 0)
                        && (strcmp(token->data, "return\0") != 0)
                        && (*token_id != IFJ_T_VARIALBE))
            {
                return IFJ_ERR_SYNTAX;
            }
            else // variable/if-else/while/return statement
            {
                if((code = check_statement (input, token_id, token)) != 0)
                {
                    return code;
                }
            }

            if ((code = lex_analyzer(input, token_id, token)) != 0)
            {
                return code;
            }

        } while (*token_id != IFJ_T_RCB);
    }
    else
    {
        //printf("token ID: %d", *token_id);
        return IFJ_ERR_SYNTAX;
    }

    return 0;
}

bool is_terminal (int token_id)
{
    if (token_id == IFJ_T_VARIALBE || token_id == IFJ_T_INT
        || token_id == IFJ_T_DOUBLE || token_id == IFJ_T_STRING)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool is_operator (int token_id)
{
    if ( (IFJ_T_NOT_SUPER_EQUAL >= token_id && token_id >= IFJ_T_ASSIGN)
        || (IFJ_T_MOD >= token_id && token_id >= IFJ_T_PLUS)
        || token_id == IFJ_T_CONC )
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*** End of file syntax_analyzer.c ***/
