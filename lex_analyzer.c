/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            lex_analyzer.c
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Source file  of Lexical analyzer for team project
 *                      of IFJ course.
 *
*******************************************************************************/

#include <stdio.h>  // pak můžu smazat
#include <stdbool.h>
#include <ctype.h>

#include "token_id.h"
#include "errors.h"

int lex_analyzer (FILE *input, int *token_id, char* token_data)
{
    unsigned int token_pos = 0; // position for writing in token
    (void)token_pos;
    token_data[0] = ' '; // default token data is empty array of chars
    token_data[1] = '\0';

    char c; // current character

    while(1)
    {
        c = fgetc(input);

        switch(c)
        {
            case '.':
                *token_id = IFJ_T_CONC;
                return 0;
                break;
            case '+':
                *token_id = IFJ_T_PLUS;
                return 0;
                break;
            case '-':
                *token_id = IFJ_T_MIN;
                return 0;
                break;
            case '*':
                *token_id = IFJ_T_MUL;
                return 0;
                break;
            case '%':
                *token_id = IFJ_T_MOD;
                return 0;
                break;
            case '(':
                *token_id = IFJ_T_LB;
                return 0;
                break;
            case ')':
                *token_id = IFJ_T_RB;
                return 0;
                break;
            case '[':
                *token_id = IFJ_T_LSB;
                return 0;
                break;
            case ']':
                *token_id = IFJ_T_RSB;
                return 0;
                break;
            case '{':
                *token_id = IFJ_T_LCB;
                return 0;
                break;
            case '}':
                *token_id = IFJ_T_RCB;
                return 0;
                break;
            case ';':
                *token_id = IFJ_T_SEMICOLON;
                return 0;
                break;
            case EOF:
                *token_id = IFJ_T_EOF;
                return 0;
                break;
            case '"':
                {
                    /** IMPLEMENT STRINGS HERE **/
                    *token_id = IFJ_T_STRING;
                    token_data[0] = c;
                    return 0;
                } break;
            case '<':
                {
                    c = fgetc(input);

                    if (c == '=')
                    {
                        *token_id = IFJ_T_LESS_EQUAL;
                        return 0;
                    }
                    else
                    {
                        *token_id = IFJ_T_LESS;
                        ungetc(c, input);
                        return 0;
                    }
                } break;
            case '>':
                {
                    c = fgetc(input);

                    if (c == '=')
                    {
                        *token_id = IFJ_T_GREATER_EQUAL;
                        return 0;
                    }
                    else
                    {
                        *token_id = IFJ_T_GREATER;
                        ungetc(c, input);
                        return 0;
                    }
                } break;
            case '!':
                {
                    c = fgetc(input);

                    if (c == '=')
                    {
                        c = fgetc(input);

                        if (c == '=')
                        {
                            *token_id = IFJ_T_NOT_SUPER_EQUAL;
                            return 0;
                        }
                        else
                        {
                            *token_id = IFJ_T_NOT_EQUAL;
                            ungetc(c, input);
                            return 0;
                        }
                    }
                    else
                    {
                        return IFJ_ERR_LEXICAL;
                    }
                } break;
            case '=':
                {
                    c = fgetc(input);

                    if (c == '=')
                    {
                        c = fgetc(input);

                        if (c == '=')
                        {
                            *token_id = IFJ_T_SUPER_EQUAL;
                            return 0;
                        }
                        else
                        {
                            *token_id = IFJ_T_EQUAL;
                            ungetc(c, input);
                            return 0;
                        }
                    }
                    else
                    {
                        *token_id = IFJ_T_ASSIGN;
                        ungetc(c, input);
                        return 0;
                    }
                } break;
            case '$':
                {
                    /** IMPLEMENT VARIABLES HERE **/
                    *token_id = IFJ_T_VARIALBE;
                    token_data[0] = c;
                    return 0;
                } break;
            case '/':
                {
                    c = fgetc(input);

                    if (c == '/') // line comment
                    {
                        while (1) // only EOF or newline end the cycle
                        {
                            c = fgetc(input);

                            if (c == EOF)
                            {
                                *token_id = IFJ_T_EOF;
                                return 0;
                            }
                            else if (c == '\n') // end of coment
                            {
                                break;
                            }
                        }

                        /*
                         * If control ever gets here, the last character was
                         * EOL. That means end of comment - continue makes
                         * main cycle read another character.
                         */
                        continue;
                    }
                    else if (c == '*') // block comment
                    {
                        unsigned char prev = 'a'; // just asuring last != '*'
                        bool end = false;

                        while ( (c = fgetc(input)) != EOF)
                        {
                            if (c == '/' && prev == '*')
                            {
                                end = true; // end of comment FOUND
                                break;
                            }
                            else
                            {
                                // saving curent char for subsequent testing
                                prev = c;
                            }
                        }

                        /*
                         * If control ever gets here, there is either EOF or
                         * end of comment. If there is end of comment - continue
                         * makes main cycle read another character. Else return
                         * EOF token.
                         */
                        if (end)
                        {
                            continue;
                        }
                        else
                        {
                            *token_id = IFJ_T_EOF;
                            return 0;
                        }
                    }
                    else // division
                    {
                        *token_id = IFJ_T_DIV;
                        ungetc(c, input);
                        return 0;
                    }
                } break;
            default:
                {
                    if (c < '!' || isblank(c))
                    {
                        continue; // skipping chars witch ord. value <=32
                    }
                    else if (isdigit(c))
                    {
                        /** IMPLEMENT INTEGERS HERE **/
                        *token_id = IFJ_T_INT;
                        token_data[0] = c;
                        return 0;
                    }
                    else
                    {
                        /** IMPLEMENT IDENTIFICATORS HERE **/
                        *token_id = IFJ_T_ID;
                        token_data[0] = c;
                        return 0;
                    }
                } break;
        } // end of "START" switch
    } // end of "START" cycle
}

/*** End of file lex_analyzer.c ***/
