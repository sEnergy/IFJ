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

/* *****************************************************************************
*		THIS FUNCTION WILL BE ERASED WHEN LUBOS COMMITS HIS CHANGES
*							FOR GCC TO SHUT UP
*******************************************************************************/

int changeable_token_update(changeable_tokenPtr token, char * new_data)
{ 
	if(strlen(token->data) < strlen(new_data))
	{
		free(token->data);
		token->data = malloc (sizeof(strlen(new_data))+1);
		if(token->data == NULL)
		{
			return IFJ_ERR_INTERNAL;	
		}

	}
	int i = 0;
	while(new_data [i] != '\0')
	{
		token->data[i] = new_data[i];
		i++;
	}
	token->data[i] = '\0';
	return 0;
}



/* *****************************************************************************
*									BOOLVAL
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
			return error_code;		}
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
		else error_code = IFJ_ERR_TYPE_COMPATIBILITY;

	}
	else error_code = IFJ_ERR_TYPE_COMPATIBILITY;
	return error_code;
}

/* *****************************************************************************
*									INTVAL
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
			if(isspace(tmp_data[i]));
			else
			{
				if(isdigit(tmp_data[i]))
				{
					tmp_number = (tmp_data[i] - '0') + 10 * tmp_number;
					if (tmp_data[i+1] == '\0' || !isdigit(tmp_data[i+1])) break;
				}
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
		else error_code = IFJ_ERR_TYPE_COMPATIBILITY;
	}
	else error_code = IFJ_ERR_TYPE_COMPATIBILITY;

	return error_code;
}
/* *****************************************************************************
*									DOUBLEVAL
*******************************************************************************/

int doubleval(changeable_tokenPtr token)
{
	int error_code = 0;

	// integer
	if( token->id == IFJ_T_INT)
	{
		double tmp_number = atof(token->data);
		char *tmp_data = token->data;
		sprintf(tmp_data,"%.1f",tmp_number);
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
		double e_num	= 0;
		int frac_div = 1;

		bool fraction = false;
		int e_sign = 1;
		bool count = true;

		while(len > i && count == true)
		{
			if(isspace(tmp_data[i]));
			else
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
						
						if (!isdigit(tmp_data[i+1]) && tmp_data[i+1] != '.') 
						{
							if(tmp_data[i+1] != 'e') count = false;
						}
				}
				else if (tmp_data[i] == 'e')
				{
					i++;
					if(isdigit(tmp_data[i]) || tmp_data[i] == '-' || tmp_data[i] == '+') 
					{
						if(tmp_data[i] == '-') 
						{
							e_sign = -1;
							i++;
							if(!isdigit(tmp_data[i])) return IFJ_ERR_RETYPE;
						}
						else if (tmp_data[i] == '+') 
						{
							e_sign = 1;
							i++;
							if(!isdigit(tmp_data[i])) return IFJ_ERR_RETYPE;
						}
						while(isdigit(tmp_data[i]))
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
					else return IFJ_ERR_RETYPE;
				}
				else if (tmp_data[i] == '.')
				{
					fraction = true;
				}
			}
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
		else error_code = IFJ_ERR_TYPE_COMPATIBILITY;
	}
	else error_code = IFJ_ERR_TYPE_COMPATIBILITY;
	return error_code;
}

/* *****************************************************************************
*									STRVAL
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
		else error_code = IFJ_ERR_TYPE_COMPATIBILITY;
	}
	else error_code = IFJ_ERR_TYPE_COMPATIBILITY;

	return error_code;
}

				/*** God and Chuck Norris approve this shit ***/
