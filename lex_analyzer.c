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
#include <stdlib.h>

#include "lex_analyzer.h"
#include "token_id.h"
#include "errors.h"

int write_c(BUFFER_STRUCT buffer, char c)
{
    char* new_ptr = NULL;
    if (buffer->position == buffer->max_length)
    {
        new_ptr = (char*) realloc(buffer->data,2*buffer->max_length*sizeof(char));
        if (new_ptr == NULL)
            return -1;
        buffer->data = new_ptr;
        buffer->max_length *= 2;
    }
    buffer->data[buffer->position++] = c;
    buffer->data[buffer->position + 1] = '\0';
    return 0;
}
int is_hexadecimal(char* str)
{
	for (int i=0; str[i] != '\0'; i++)
	{
		if (!(isdigit(str[i]) || (str[i] >= 'A' && str[i] <= 'F') || 
		(str[i] >= 'a' && str[i] <= 'f')))
			return 0;
	}
	return 1;
}
int lex_analyzer (FILE *input, int *token_id, BUFFER_STRUCT buffer)
{
    unsigned int token_pos = 0; // position for writing in token
    (void)token_pos;

    char c; // current character
    c = fgetc(input);
    if (c == EOF)
    {
        *token_id = IFJ_T_EOF;
        return 0;
    }
    while(1)
    {

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
                while((c = fgetc(input)) > 31 && c != '\\' && c != '"' && c != '$')
                {
                    write_c(buffer,c);
                }
                if (c <= 31)
                    return IFJ_ERR_LEXICAL;
                switch (c)
                {
                    case '$':
                    {
                        write_c(buffer, c);
                        c = fgetc(input);
                        if (isalpha(c) || c == '_')
                        {
                            write_c(buffer,c);
                        }
                        else
                            return IFJ_ERR_LEXICAL;

                        while(isalnum(c = fgetc(input)) || c == '_')
                            {
                                write_c(buffer,c);
                            }
                        if (c == '"')
                        {
                            *token_id = IFJ_T_STRING;
                            return 0;
                        }
                        else if(isspace(c))
                        {
                            ungetc(c, input);
                            c = '"';
                        }
                        else
                            return IFJ_ERR_LEXICAL;
                    } break;
                    case '\\':
                    {
                        c = fgetc(input);
                        switch (c)
                        {
                            case 'n':
                            {
                                write_c(buffer,10);
                                c = '"';
                            } break;
                            case 't':
                            {
                                write_c(buffer,9);
                                c = '"';
                            } break;
                            case '"':
                            {
                                write_c(buffer,'"');
                                c = '"';
                            } break;
                            case 'x':
                            {
                                char tmp_buffer[2];
                                tmp_buffer[0] = fgetc(input);
                                if (tmp_buffer[0] == EOF)
                                    return IFJ_ERR_LEXICAL;
                                tmp_buffer[1] = fgetc(input);
                                if (tmp_buffer[1] == EOF)
                                    return IFJ_ERR_LEXICAL;
                                tmp_buffer[2]='\0';
                                if (is_hexadecimal(tmp_buffer))
                                {
                                    char* control_pointer = NULL;
                                    int number = strtol(tmp_buffer, &control_pointer, 16);

                                    if (*control_pointer != '\0')
                                        return IFJ_ERR_LEXICAL;
                                    write_c(buffer,number);
                                }
                                else
									return IFJ_ERR_LEXICAL;
                                c = '"';
                            } break;
                            case '\\':
                            {
                                write_c(buffer,'\\');
                                c = '"';
                            } break;
                            case EOF:
                            {
                                return IFJ_ERR_LEXICAL;
                            } break;
                            default:
                            {
                                write_c(buffer,'\\');
                                write_c(buffer,c);
                                c = '"';
                            } break;
                        }
                    } break;
                    case '"':
                    {
                        *token_id = IFJ_T_STRING;
                        return 0;
                    } break;
                }
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
                    write_c(buffer, c);
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
                    if (c < '!' || isspace(c))
                    {
                        c = fgetc(input);
                        continue; // skipping chars witch (carodejnica) ord. value <=32
                    }
                    else if (isdigit(c))
                    {
                        /** IMPLEMENT INTEGERS HERE **/
                        *token_id = IFJ_T_INT;
                        return 0;
                    }
                    else
                    {
                        /** IMPLEMENT IDENTIFICATORS HERE **/
                        *token_id = IFJ_T_ID;
                        return 0;
                    }
                } break;
        } // end of "START" switch
    } // end of "START" cycle
}

/*** End of file lex_analyzer.c ***/
