/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            functions.c
 * Author:              Matúš Turic
 * Encoding:            UTF-8
 *
 * Description:         Source file of built-in functions for team project
 *                      of IFJ course.
 *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#include "syntax_analyzer.h"
#include "token_id.h"
#include "errors.h"
#include "functions.h"
#include "interpreter.h"
#include "ial.h"

/* *****************************************************************************
*                                   BOOLVAL
*******************************************************************************/


int boolval(changeable_tokenPtr token)
{
    int error_code = 0;

    // integer
    if(token->id == IFJ_T_INT)
    {
        if(atoi(token->data) == 0)
        {
            token->id = IFJ_T_KEYWORD;
            error_code = changeable_token_update(token, "false\0");
            return error_code;
        }
        else
        {
            token->id = IFJ_T_KEYWORD;
            error_code = changeable_token_update(token, "true\0");
            return error_code;
        }
    }
    // double
    else if( token->id == IFJ_T_DOUBLE)
    {
        if(atof(token->data) == 0.0)
        {
            token->id = IFJ_T_KEYWORD;
            error_code = changeable_token_update(token, "false\0");
            return error_code;
        }
        else
        {
            token->id = IFJ_T_KEYWORD;
            error_code = changeable_token_update(token, "true\0");
            return error_code;
        }
    }
    // string
    else if(token->id == IFJ_T_STRING)
    {
        if(!strcmp(token->data,""))
        {
            token->id = IFJ_T_KEYWORD;
            error_code = changeable_token_update(token, "false\0");
            return error_code;      }
        else
        {
            token->id = IFJ_T_KEYWORD;
            error_code = changeable_token_update(token, "true\0");
            return error_code;
        }
    }
    // NULL, true, false
    else if(token->id == IFJ_T_KEYWORD)
    {
        if(!strcmp(token->data,"null\0"))
        {
            error_code = changeable_token_update(token, "false\0");
            return error_code;
        }
        else if(!strcmp(token->data,"true\0")) return error_code;
        else if(!strcmp(token->data,"false\0")) return error_code;
        else error_code = IFJ_ERR_OTHER_RUNTIME;

    }
    else error_code = IFJ_ERR_OTHER_RUNTIME;
    return error_code;
}

/* *****************************************************************************
*                                   INTVAL
*******************************************************************************/

int intval(changeable_tokenPtr token)
{
    int error_code = 0;

    // integer
    if( token->id == IFJ_T_INT)
    {
        return error_code;
    }
    // double
    else if( token->id == IFJ_T_DOUBLE)
    {
        int tmp_number = atoi(token->data);
        char *tmp_data = token->data;
        sprintf(tmp_data,"%d",tmp_number);
        token->id = IFJ_T_INT;
        error_code = changeable_token_update(token, tmp_data);
        return error_code;
    }
    // string
    else if (token->id == IFJ_T_STRING)
    {
        token->id = IFJ_T_INT;
        char *tmp_data = token->data;
        int len = strlen(token->data);
        int i = 0;
        int tmp_number = 0;
        
        while(len > i)
        {
            if(isdigit(tmp_data[i]) || tmp_data[i] == '\0') 
            {
                break;
            }
            while(!isspace(tmp_data[i]) && (len > i))
            {
                i++;
            }
            while(isspace(tmp_data[i]) && (len > i))
            {
                i++;
            }
        }
        while(len > i)
        {
            if(isdigit(tmp_data[i]))
            {
                tmp_number = (tmp_data[i] - '0') + 10 * tmp_number;
                if (tmp_data[i+1] == '\0' || !isdigit(tmp_data[i+1])) break;
            }
            i++;
        }
        
        sprintf(tmp_data,"%d",tmp_number);
        token->id = IFJ_T_INT;
        error_code = changeable_token_update(token, tmp_data);
        return error_code;
    }
    // NULL, true, false
    else if(token->id == IFJ_T_KEYWORD)
    {
        if(!strcmp(token->data,"false\0"))
        {
            token->id = IFJ_T_INT;
            error_code = changeable_token_update(token, "0\0");
            return error_code;
        }
        else if(!strcmp(token->data,"true\0"))
        {
            token->id = IFJ_T_INT;
            error_code = changeable_token_update(token, "1\0");
            return error_code;
        }
        else if(!strcmp(token->data,"null\0"))
        {
            token->id = IFJ_T_INT;
            error_code = changeable_token_update(token, "0\0");
            return error_code;
        }
        else error_code = IFJ_ERR_OTHER_RUNTIME;
    }
    else error_code = IFJ_ERR_OTHER_RUNTIME;

    return error_code;
}
/* *****************************************************************************
*                                   DOUBLEVAL
*******************************************************************************/

int doubleval(changeable_tokenPtr token)
{
    int error_code = 0;

    // integer
    if( token->id == IFJ_T_INT)
    {
        double tmp_number = atof(token->data);
        char *tmp_data = token->data;
        sprintf(tmp_data,"%g",tmp_number);
        token->id = IFJ_T_DOUBLE;
        error_code = changeable_token_update(token, tmp_data);
        return error_code;
    }
    // double
    else if( token->id == IFJ_T_DOUBLE)
    {
        return error_code;
    }
    // string
    else if (token->id == IFJ_T_STRING)
    {
        char *tmp_data = token->data;
        int len = strlen(token->data);
        int i = 0;
        double int_num = 0;
        double frac_num = 0;
        double e_num = 0;
        int frac_div = 1;

        bool fraction = false;
        int e_sign = 1;
        bool count = true;
        bool was_here = false;

        while(len > i)
        {
            if(isdigit(tmp_data[i]) || tmp_data[i] == '\0') 
            {
                break;
            }           
            while(!isspace(tmp_data[i]) && (len > i))
            {
                i++;
            }
            while(isspace(tmp_data[i]) && (len > i))
            {
                i++;
            }
        }

        while(len > i && count == true)
        {
            if(isdigit(tmp_data[i]))
            {

                if(fraction)
                {
                    frac_num = (tmp_data[i] - '0') + 10.0 * frac_num;
                    frac_div *= 10;
                }
                else
                {
                    int_num = (tmp_data[i] - '0') + 10.0 * int_num;
                }           
                
                if (tmp_data[i+1] == 'e' || tmp_data[i+1] == 'E')
                {
                    i=i+2;
                    if(isdigit(tmp_data[i]) || tmp_data[i] == '-' || tmp_data[i] == '+') 
                    {
                        if(tmp_data[i] == '-') 
                        {
                            e_sign = -1;
                            i++;
                            if(!isdigit(tmp_data[i])) return 11;
                        }
                        else if (tmp_data[i] == '+') 
                        {
                            e_sign = 1;
                            i++;
                            if(!isdigit(tmp_data[i])) return 11;
                        }
                        while(isdigit(tmp_data[i]) && len > i)
                        {
                            e_num = (tmp_data[i] - '0') + 10.0 * e_num;
                            i++;
                            if (!isdigit(tmp_data[i+1])) count = false;
                        }
                        
                        if(e_sign == 1)
                        {
                            e_num = pow(10,e_num);
                        }
                        else
                        {
                            e_num = pow(10,(-1)*e_num);
                        }   
                    }
                    else return 11;
                }
                else if (tmp_data[i+1] == '.' && was_here == false)
                {
                    fraction = true;
                    was_here = true;
                    if (!isdigit(tmp_data[i+2])) return 11;
                }
            }
            if(isspace(tmp_data[i])) break;
            i++;
        }

        if(e_num == 0)
        {
            sprintf(tmp_data,"%g",(int_num + frac_num / frac_div));
            
        }
        else 
        {
            sprintf(tmp_data,"%g",((int_num + frac_num / frac_div)*e_num));
        }
        token->id = IFJ_T_DOUBLE;
        error_code = changeable_token_update(token, tmp_data);
        return error_code;
    }
    // NULL, true, false
    else if( token->id == IFJ_T_KEYWORD)
    {
        if(!strcmp(token->data,"false\0"))
        {
            token->id = IFJ_T_DOUBLE;
            error_code = changeable_token_update(token, "0.0\0");
            return error_code;
        }
        else if(!strcmp(token->data,"true\0"))
        {
            token->id = IFJ_T_DOUBLE;
            error_code = changeable_token_update(token, "1.0\0");
            return error_code;
        }
        else if(!strcmp(token->data,"null\0"))
        {
            token->id = IFJ_T_DOUBLE;
            error_code = changeable_token_update(token, "0.0\0");
            return error_code;
        }
        else error_code = IFJ_ERR_OTHER_RUNTIME;
    }
    else error_code = IFJ_ERR_OTHER_RUNTIME;
    return error_code;
}

/* *****************************************************************************
*                                   STRVAL
*******************************************************************************/

int strval(changeable_tokenPtr token)
{
    int error_code = 0;

    // integer
    if ( token->id == IFJ_T_INT )
    {
        token->id = IFJ_T_STRING;
        return error_code;
    }
    // double
    else if ( token->id == IFJ_T_DOUBLE )
    {
        token->id = IFJ_T_STRING;
        return error_code;
    }
    // string
    else if ( token->id == IFJ_T_STRING )
    {
        return error_code;
    }
    // NULL, false, true
    else if ( token->id == IFJ_T_KEYWORD )
    {
        if(!strcmp(token->data,"false\0"))
        {
            token->id = IFJ_T_STRING;
            error_code = changeable_token_update(token, "\0");
            return error_code;
        }
        else if(!strcmp(token->data,"true\0"))
        {
            token->id = IFJ_T_STRING;
            error_code = changeable_token_update(token, "1\0");
            return error_code;
        }
        else if(!strcmp(token->data,"null\0"))
        {
            token->id = IFJ_T_STRING;
            error_code = changeable_token_update(token, "\0");
            return error_code;
        }
        else error_code = IFJ_ERR_OTHER_RUNTIME;
    }
    else error_code = IFJ_ERR_OTHER_RUNTIME;

    return error_code;
}

/* *****************************************************************************
*                                   STRLEN
*******************************************************************************/

int str_len(changeable_tokenPtr token)
{
    int error_code = 0;
    token->id = IFJ_T_INT;
    char *tmp_data = token->data;
    char *str;

    for(str = tmp_data; *str != '\0'; ++str);

    sprintf(tmp_data,"%ld",(str - tmp_data));
    error_code = changeable_token_update(token, tmp_data);
    
    return error_code;
}

/* *****************************************************************************
*                                   GET_STRING
*******************************************************************************/

int get_string(changeable_tokenPtr token)
{
    int error_code = 0;
    char *tmp_data;
    char *realloc_buf;
    tmp_data = malloc(sizeof(char));
    if (tmp_data == NULL) return IFJ_ERR_INTERNAL;
    int i = 0;
    char znak;

    while(( znak = getchar()) != '\n')
    {
        if(znak == EOF) break;
        tmp_data[i] = znak;

        realloc_buf = realloc(tmp_data,(i+2)*sizeof(char));
        if(realloc_buf != NULL)
        {
            tmp_data = realloc_buf;
        }
        else
        {
            free(tmp_data);
            tmp_data = NULL;
            return IFJ_ERR_INTERNAL;
        }
        i++;
    }
    tmp_data[i] = '\0';

    token->data = tmp_data;
    token->id = IFJ_T_STRING;
    return error_code;
}

/* *****************************************************************************
*                                   PUT_STRING
*******************************************************************************/

int put_string(int* arg_number, changeable_tokenPtr token)
{
    int argc = 0;
    while(token->data != NULL)
    {
        if (token->id == IFJ_T_INT) 
        {
        printf("%s",token->data);
        argc++;
        }
        else if (token->id == IFJ_T_DOUBLE) 
        {
        printf("%s",token->data);
        argc++;
        }
        else if (token->id == IFJ_T_STRING) 
        {
        printf("%s",token->data);
        argc++;
        }
        else if (token->id == IFJ_T_KEYWORD)    
        {
        printf("%s",token->data);
        argc++;
        }
        if(token->next_params == NULL) break;
        token = token->next_params;
    }
    *arg_number = argc;
    return 0;   
}

/* *****************************************************************************
*                                   SORT_STRING
*******************************************************************************/

int sort_string(changeable_tokenPtr token)
{
    int error_code = 0;
    if(token->id == IFJ_T_STRING)
    {
        int length = strlen(token->data);    
        char *tmp_data = token->data;
        char *out = shell_sort(tmp_data, length);
        out = shell_sort(tmp_data, length);
        error_code = changeable_token_update(token, out);
    }
    else error_code = IFJ_ERR_OTHER_RUNTIME;
    return error_code;
}

/* *****************************************************************************
*                                   GET_SUBSTRING
*******************************************************************************/
int get_substring (changeable_tokenPtr token)
{
    char* string = token->data;
    int start, end, code, str_len = strlen(string);
    start = strtol(token->next_params->data, NULL, 10);
    end = strtol(token->next_params->next_params->data, NULL, 10);

    if (start < 0 || end < 0 || start > end || start >= str_len || end >= str_len)
    {
        code = IFJ_ERR_OTHER_RUNTIME;
    }
    else if (start == end)
    {
        code = changeable_token_update (token, "");

    }
    else
    {
        int substr_len = end-start;
        int i_dest = 0;
        char* tmp = malloc(sizeof(char)*(substr_len+1));
        for (int i_src = start; i_src < end; ++i_src)
        {
            tmp[i_dest] = string[i_src];
            ++i_dest;
        }
    tmp[i_dest] = '\0';
    code = changeable_token_update (token, tmp);
    free(tmp);
    tmp = NULL;
    }
    return code;
}

/* *****************************************************************************
*                                   FIND_STRING
*******************************************************************************/
int find_string (changeable_tokenPtr token)
{
    int code = 0, found;

    char* source = token->data;
    char* pattern = token->next_params->data;

    if (strlen(pattern) == 0) // empty pattern is always found on position zero
    {
        code = changeable_token_update (token, "0");
    }
    else
    {
        code = search_substring(source, pattern, &found);
        if (code != 0) return code;
        char *tmp = malloc(sizeof(char)*10);
        if (tmp == NULL) return IFJ_ERR_INTERNAL;
        sprintf(tmp, "%i", found);
        code = changeable_token_update (token, tmp);

        free(tmp);
        tmp = NULL;
    }
    return code;

}



/*** End of file functions.c ***/
