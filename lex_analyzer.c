/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            lex_analyzer.c
 * Author:              Marcel Fiala
 *                      Luboš Vaníček
 *                      Matúš Turic
 *                      Ivan Straka
 *                      Attila Večerek
 * Encoding:            UTF-8
 *
 * Description:         Source file  of Lexical analyzer for team project
 *                      of IFJ course.
 *
*******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#include "token_id.h"
#include "lex_analyzer.h"
#include "syntax_analyzer.h"
#include "errors.h"

// array of keywords
const char *keywords[KEYWORD_NUMBER] =
{
    "else\0",   "function\0",   "if\0",     "return\0",
    "while\0",  "false\0",      "null\0",   "true\0", 
    "elseif\0"
};


int write_c(BUFFER_STRUCT buffer, char c)
{
    char* new_ptr = NULL;
    if (buffer->position >= buffer->max_length)
    {
        new_ptr = (char*) realloc(buffer->data,1+2*buffer->max_length*sizeof(char));

        if (new_ptr == NULL)
        {
            return IFJ_ERR_INTERNAL;
        }

        buffer->data = new_ptr;
        buffer->max_length *= 2;
    }
    buffer->data[buffer->position] = c;
    buffer->position++;
    buffer->data[buffer->position] = '\0';
    return 0;
}

int is_hexadecimal(char c)
{
    if (!(isdigit(c) || (c >= 'A' && c <= 'F') ||
        (c >= 'a' && c <= 'f')))
            return 0;
    return 1;
}
void buffer_next_token(BUFFER_STRUCT buffer)
{
    buffer->position++;
}

int lex_analyzer (FILE *input, TokenPtr token, BUFFER_STRUCT buffer)
{
    //zmenit buffer - pro efektivnost
    token->content = buffer->position;
    char c = fgetc(input); // current character

    if (c == EOF)
    {
        token->id = IFJ_T_EOF;
        return 0;
    }
/*
    printf("%zu %zu  ",buffer->position,buffer->max_length);
    for(size_t i=0;i<=buffer->max_length;i++)
        if (buffer->data[i] != '\0') printf("%c",buffer->data[i]);
        else printf("|");

    printf("--\n");
*/
    while(1)
    {
        switch(c)
        {
            case '.':
                token->id = IFJ_T_CONC;
                return 0;
                break;
            case ',':
                token->id = IFJ_T_SEP;
                return 0;
                break;
            case '+':
                token->id = IFJ_T_PLUS;
                return 0;
                break;
            case '-':
                token->id = IFJ_T_MIN;
                return 0;
                break;
            case '*':
                token->id = IFJ_T_MUL;
                return 0;
                break;
            /*case '%':
                token->id = IFJ_T_MOD;
                return 0;
                break;*/
            case '(':
                token->id = IFJ_T_LB;
                return 0;
                break;
            case ')':
                token->id = IFJ_T_RB;
                return 0;
                break;
            /*case '[':
                token->id = IFJ_T_LSB;
                return 0;
                break;
            case ']':
                token->id = IFJ_T_RSB;
                return 0;
                break;*/
            case '{':
                token->id = IFJ_T_LCB;
                return 0;
                break;
            case '}':
                token->id = IFJ_T_RCB;
                return 0;
                break;
            case ';':
                token->id = IFJ_T_SEMICOLON;
                return 0;
                break;
            case EOF:
                token->id = IFJ_T_EOF;
                return 0;
                break;
            /*
             * STATE STRING
             */
            case '"':
            {
                //save all correct character except, escape $ and end of str
                c = fgetc(input);
                while(c > 31 && c != '\\' && c != '"' && c != '$')
                {
                    if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                    {
                        return IFJ_ERR_INTERNAL;
                    }

                    c = fgetc(input);
                }
                //check, if next char is not uncorrect
                if (c <= 31)
                {
                    return IFJ_ERR_LEXICAL;
                }
                switch (c)
                {
                    //check corectness of variable
                    case '$':
                    {
                        return IFJ_ERR_LEXICAL;

                    } break; //end of variable

                    //escape sequence...c = '"' means starting " case again
                    case '\\':
                    {
                        c = fgetc(input);
                        if (c <= 31)
                        {
                            return IFJ_ERR_LEXICAL;
                        }
                        switch (c)
                        {
                            case '$':
                              {
                                if (write_c(buffer,'$') == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }

                                c = '"';
                              } break;
                            case 'n':
                              {
                                if (write_c(buffer,'\n') == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }

                                c = '"';
                              } break;
                              case 't':
                              {
                                if (write_c(buffer,'\t') == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }

                                c = '"';
                              } break;
                              case '"':
                              {
                                if (write_c(buffer,'\"') == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }

                                c = '"';
                              } break;
                            case 'x':
                            {
                                //read next two characters and convert it into long
                                char tmp_buffer[2];
                                tmp_buffer[1] = tmp_buffer[2] = '\0';
                                tmp_buffer[0] = fgetc(input);
                                if (tmp_buffer[0] <=31)
                                {
                                    return IFJ_ERR_LEXICAL;
                                }
                                if (! is_hexadecimal(tmp_buffer[0]))
                                {
                                    if ((write_c(buffer,'\\') != 0)
                                        ||(write_c(buffer, 'x') != 0))
                                    {
                                        return IFJ_ERR_INTERNAL;
                                    }
                                    ungetc(tmp_buffer[0],input);
                                    c = '"';
                                    break;
                                }
                                tmp_buffer[1] = fgetc(input);
                                if (tmp_buffer[1] <=31)
                                {
                                    return IFJ_ERR_LEXICAL;
                                }
                                if (! is_hexadecimal(tmp_buffer[1]))
                                {
                                    if ((write_c(buffer,'\\') != 0)
                                        ||(write_c(buffer, 'x') != 0)
                                        ||(write_c(buffer, tmp_buffer[0]) != 0))
                                    {
                                        return IFJ_ERR_INTERNAL;
                                    }
                                    ungetc(tmp_buffer[1],input);
                                    c = '"';
                                    break;
                                }
                                int number = strtol(tmp_buffer, NULL, 16);
                                if (write_c(buffer,number) == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }
                                c = '"';
                            } break;
                            case '\\':
                            {
                                if (write_c(buffer,'\\') == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }
                                c = '"';
                            } break;
                            case EOF:
                            {
                                return IFJ_ERR_LEXICAL;
                            } break;
                            default:
                            {
                                if (write_c(buffer,'\\') == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }
                                if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                {
                                    return IFJ_ERR_INTERNAL;
                                }
                                c = '"';
                            } break;
                        }//end of switch for escape sequence
                    } break;//end of escape sequence

                    //end of string
                    case '"':
                    {
                        token->id = IFJ_T_STRING;
                        buffer_next_token(buffer);
                        return 0;
                    } break;
                } //end of {\,$,"} switch
            } break; //end of string FSM
            /*
             * STATE OPERATORS (lower)
             */
            case '<':
                {
                    c = fgetc(input);

                    if (c == '=')
                    {
                        token->id = IFJ_T_LESS_EQUAL;
                        return 0;
                    }
                    else
                    {
                        token->id = IFJ_T_LESS;
                        ungetc(c, input);
                        return 0;
                    }
                } break;
            case '>':
                {
                    c = fgetc(input);

                    if (c == '=')
                    {
                        token->id = IFJ_T_GREATER_EQUAL;
                        return 0;
                    }
                    else
                    {
                        token->id = IFJ_T_GREATER;
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
                            token->id = IFJ_T_NOT_SUPER_EQUAL;
                            return 0;
                        }
                        else
                        {
                            return IFJ_ERR_LEXICAL;
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
                            token->id = IFJ_T_SUPER_EQUAL;
                            return 0;
                        }
                        else
                        {
                            return IFJ_ERR_LEXICAL;
                        }
                    }
                    else
                    {
                        token->id = IFJ_T_ASSIGN;
                        ungetc(c, input);
                        return 0;
                    }
                } break;
            /*
             * STATE_VARIABLE
             */
            case '$':
                {
                    if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                    {
                        return IFJ_ERR_INTERNAL;
                    }
                    c = fgetc(input);
                    if (isalpha(c) || c == '_')
                    {
                        if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                        {
                            return IFJ_ERR_INTERNAL;
                        }
                        c = fgetc(input);
                        while (isalpha(c) || isdigit(c) || c == '_')
                        {
                            if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                            {
                                return IFJ_ERR_INTERNAL;
                            }
                            c = fgetc(input);
                        }
                        ungetc(c, input);
                        buffer_next_token(buffer);
                        token->id = IFJ_T_VARIALBE;
                        return 0;
                    }
                    else
                    {
                        return IFJ_ERR_LEXICAL;
                    }
                } break;
            /*
             * STATE COMMENTS
             */
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
                                token->id = IFJ_T_EOF;
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
                            c = fgetc(input);
                            continue;
                        }
                        else
                        {
                            token->id = IFJ_T_EOF;
                            return IFJ_ERR_LEXICAL;
                        }
                    }
                    else // division
                    {
                        token->id = IFJ_T_DIV;
                        ungetc(c, input);
                        return 0;
                    }
                } break;
            /*
             * SPACE_IGNORE, STATE INT, DOUBLE, ID, KEYWORD
             */
            default:
                {
                    if (c < '!' || isspace(c))
                    {
                        c = fgetc(input);
                        continue; // skipping chars witch (carodejnica) ord. value <=32
                    }
                    /*
                     * INTEGER AND DOUBLE
                     */
                    else if (isdigit(c))
                    {
                        /** STATES INTEGER AND DOUBLE **/
                        int state = S_INT;  //because of multiple states heading into the same state
                        while (true)
                        {
                            switch(state)
                            {
                                case S_INT:     //first state
                                {
                                    while (isdigit(c))
                                    {
                                        if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                        {
                                            return IFJ_ERR_INTERNAL;
                                        }
                                        c = fgetc(input);
                                    }
                                    /* heading to double section */
                                    if (c == '.')
                                    {
                                        if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                        {
                                            return IFJ_ERR_INTERNAL;
                                        }
                                        c = fgetc(input);
                                        state = S_FDBL; //State First (Digit) Double
                                    }
                                    else if (c == 'e' || c == 'E')
                                    {
                                        if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                        {
                                            return IFJ_ERR_INTERNAL;
                                        }
                                        c = fgetc(input);
                                        state = S_E_EXP; //Beginning of exponent
                                    }
                                    /*****************************/
                                    else        //Stays Integer
                                    {
                                        token->id = IFJ_T_INT;
                                        buffer_next_token(buffer);
                                        ungetc(c,input);
                                        return 0;
                                    }
                                }
                                case S_FDBL:    //awaiting number, nothing else
                                {
                                    if (isdigit(c))
                                    {
                                        while (isdigit(c))
                                        {
                                            if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                            {
                                                return IFJ_ERR_INTERNAL;
                                            }
                                            c = fgetc(input);
                                        }
                                        if (c == 'e' || c == 'E')   //Beginning of exponent -> S_E_EXP
                                        {
                                            if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                            {
                                                return IFJ_ERR_INTERNAL;
                                            }
                                            c = fgetc(input);
                                            state = S_E_EXP;
                                        }
                                        else    //no exponent, return DOUBLE
                                        {
                                            token->id = IFJ_T_DOUBLE;
                                            ungetc(c,input);
                                            buffer_next_token(buffer);
                                            return 0;
                                        }
                                    }
                                    else
                                    {
                                        return IFJ_ERR_LEXICAL;
                                    }
                                }
                                case S_E_EXP:   //awaiting digit or signs '+'/'-'
                                {
                                    if (isdigit(c))     //Exponent's first digit -> S_EXP
                                    {
                                        if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                        {
                                            return IFJ_ERR_INTERNAL;
                                        }
                                        c = fgetc(input);
                                        state = S_EXP;
                                    }
                                    else if (c == '+' || c == '-')
                                    {
                                        if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                        {
                                            return IFJ_ERR_INTERNAL;
                                        }
                                        c = fgetc(input);
                                        if (isdigit(c)) //Exponent's first digit -> S_EXP
                                        {
                                            write_c(buffer,c);
                                            c = fgetc(input);
                                            state = S_EXP;
                                        }
                                        else            //no digit -> return LEX error
                                        {
                                            return IFJ_ERR_LEXICAL;
                                        }
                                    }
                                    else                //no digit, '+' or '-' -> return LEX error
                                    {
                                        return IFJ_ERR_LEXICAL;
                                    }
                                }
                                case S_EXP:     //awaiting digits and return Double
                                {
                                    while (isdigit(c))
                                    {
                                        if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                                        {
                                            return IFJ_ERR_INTERNAL;
                                        }
                                        c = fgetc(input);
                                    }
                                    token->id = IFJ_T_DOUBLE;
                                    ungetc(c,input);
                                    buffer_next_token(buffer);
                                    return 0;
                                }
                            }
                        }
                    }
                    /*
                     * STATE ID, KEYWORD
                     */
                    else if (isalpha(c) || (c == '_'))
                    {
                        // if different character - end cycle
                        while (isalpha(c) || isdigit(c) || (c == '_'))
                        {
                            if (write_c(buffer,c) == IFJ_ERR_INTERNAL)
                            {
                                return IFJ_ERR_INTERNAL;
                            }      // write char to buffer
                            c = fgetc(input);       // get new char from input
                        }
                        ungetc(c,input);

                        token->id = IFJ_T_ID;     // token = identificator
                        int i = 0;                  // index
                        while(i < KEYWORD_NUMBER)
                        {
                            // compare data in buffer with keywords
                            if(strcmp(&buffer->data[token->content],keywords[i]) == 0 )
                            {
                                // found match - token = keyword - end cycle
                                token->id = IFJ_T_KEYWORD;

                                break;
                            }
                            // increase index
                            i++;
                        }
                        buffer_next_token(buffer);
                        return 0;
                    }
                    else
                    {
                        return IFJ_ERR_LEXICAL;
                    }
                } break;
        } // end of "START" switch
    } // end of "START" cycle
}

/*** End of file lex_analyzer.c ***/
