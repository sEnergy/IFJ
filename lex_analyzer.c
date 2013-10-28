/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            lex_analyzer.c
 * Author:              Marcel Fiala
 * 						Luboš Vaníček
 * 						Matúš Turic
 * 						Bambi
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
#include <string.h>


#include "lex_analyzer.h"
#include "token_id.h"
#include "errors.h"

// defined number of keywords
#define keyword_number 8

// array of keywords
const char *keywords[keyword_number] =
{
    "else\0",   "function\0",   "if\0",     "return\0", 
    "while\0",  "false\0",      "null\0",   "true\0"
};




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
				//save all correct character except, escape $ and end of str
				c = fgetc(input);
                while(c > 31 && c != '\\' && c != '"' && c != '$')
                {
                    write_c(buffer,c);
                    c = fgetc(input)
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
						//save $ and check if next char is alpha
                        write_c(buffer, c);
                        c = fgetc(input);
                        if (isalpha(c) || c == '_')
                        {
                            write_c(buffer,c);
                        }
                        else
                        {
                            return IFJ_ERR_LEXICAL;
						}
						//save all alpha=numeric characters
						c = fgetc(input);
                        while(isalnum(c) || c == '_')
                        {
                            write_c(buffer,c);
                            c = fgetc(input);
                        }
                        /*end of string - ' " '
                         * end of variable - white character
                         * or lexical error
                         */
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
                    } break; //end of variable
                    
                    //escape sequence...c = '"' means starting " case again
                    case '\\':
                    {
                        c = fgetc(input);
                        switch (c)
                        {
                            case 'n':
                            {
                                write_c(buffer,'\n');
                                c = '"';
                            } break;
                            case 't':
                            {
                                write_c(buffer,'\t');
                                c = '"';
                            } break;
                            case '"':
                            {
                                write_c(buffer,'"');
                                c = '"';
                            } break;
                            case 'x':
                            {
								//read next two characters and convert it into long
                                char tmp_buffer[2];
                                read = fscanf(input,"%2s",tmp_buffer))
                                tmp_buffer[2]='\0';
                                if (read != 2)
                                {
									return IF_ERR_LEXICAL;
								}
                                if (is_hexadecimal(tmp_buffer))
                                {
                                    char* control_pointer = NULL;
                                    int number = strtol(tmp_buffer, &control_pointer, 16);
                                    if (*control_pointer != '\0')
                                    {    
										return IFJ_ERR_LEXICAL;
                                    }
                                    write_c(buffer,number);
                                }
                                else
								{
									return IFJ_ERR_LEXICAL;
                                }
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
                        }//end of switch for escape sequence
                    } break;//end of escape sequence
                    
                    //end of string
                    case '"':
                    {
                        *token_id = IFJ_T_STRING;
                        return 0;
                    } break;
                } //end of {\,$,"} switch 
            } break; //end of string FSM
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
			/*
             * STATE_VARIABLE
             */
            case '$':
				{
					c = fgetc(input);
					if (isalpha(c) || c == '_')
					{
						write_c(buffer,c);
						c = fgetc(input);
						while (isalpha(c) || isdigit(c) || c == '_')
						{
							write_c(buffer,c);
							c = fgetc(input);
						}
						ungetc(c, input);
						*token_id = IFJ_T_VARIALBE;
						return 0;
					}
					else
					{
						return IFJ_ERR_LEXICAL;
					}
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
                        while (isalpha(c) || isdigit(c) || (c == '_'))
                        {
                            write_c(buffer,c);
                            c = fgetc(input);
                            // if different character - end cycle
                        }   
                        
                        (*token_id) = IFJ_T_ID;
                        int i = 0;  // index  
                        while(i < keyword_number)
                        {
                            // compare data in buffer with keywords
                            if(strcmp(buffer->data,keywords[i]) == 0 )
                            {
                                // found match - end cycle
                                (*token_id) = IFJ_T_KEYWORD;
                                break;
                            }
                            i++;
                        }  
                        return 0;
                    }
                } break;
        } // end of "START" switch
    } // end of "START" cycle
}

/*** End of file lex_analyzer.c ***/
