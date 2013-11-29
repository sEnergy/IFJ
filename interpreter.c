/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            Interpreter.c
 * Author:              Lubo¹ Vaníèek
 * Encoding:            UTF-8
 *
 * Description:         Implementation of funtions for list of tokens.
 *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>

#include "errors.h"
#include "token_list.h"
#include "token_id.h"
#include "syntax_analyzer.h"
#include "ial.h"
#include "interpreter.h"
#include "functions.h"

changeable_tokenPtr changeable_token_Init (void)
{
    changeable_tokenPtr New_token = malloc(sizeof(struct changeable_token));
    if (New_token == NULL)
    {
        return NULL;
    }
    return New_token;
}

/*
 * function changes data in @change_token, reallocates spaces if needed
 */
int changeable_token_update(changeable_tokenPtr change_token, char * new_data)
{ 
    if(strlen(change_token->data) < strlen(new_data)) 
    { 
        free(change_token->data); 
        change_token->data = malloc (sizeof(char)* (strlen(new_data)+1)); 
        if(change_token->data == NULL) 
        { 
            return IFJ_ERR_INTERNAL; 
        } 
    }
    int i = 0;
    while (new_data[i] != '\0') 
    { 
        change_token->data[i] = new_data[i];
        ++i;
    } 
    change_token->data[i] = '\0';
    return 0;
}

int changeable_token_Insert (changeable_tokenPtr change_token, TokenPtr token, BUFFER_STRUCT buffer)
{
    change_token->id = token->id;
    if ((change_token->data = malloc(sizeof(char) * (strlen(&(buffer->data[token->content])) + 1))) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    int i = 0;
    while ((buffer->data[token->content + i]) != '\0') 
    { 
        change_token->data[i] = buffer->data[token->content + i];
        ++i;
    } 
    change_token->data[i] = '\0';
    
    return 0;
}

void changeable_token_Destroy (changeable_tokenPtr change_token)
{
    free(change_token->data);
    free(change_token);
}



/*
 * Chooses which function should be used, depends on token_id
 * This functions can only be in the root area
 */
int call_leaf_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable)
{
    /* error should be always changed */
    int error = IFJ_ERR_INTERNAL;
    
    switch (token->id)
    {
        case IFJ_T_INT:             //10
            error = number_function (token, change_token, buffer);
            break;
        case IFJ_T_DOUBLE:          //10.0
            error = number_function (token, change_token, buffer);
            break;
        case IFJ_T_STRING:          //"10ahoj"
            error = number_function (token, change_token, buffer);
            break;
        case IFJ_T_VARIALBE:        //$ahoj
            /* need hashtable and token for search */
            error = var_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_MUL:             // '*'
            error = basic_operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_DIV:             // '/'
            error = basic_operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_PLUS:            // '+'
            error = basic_operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_MIN:             // '-'
            error = basic_operator_function (token, change_token, buffer, hashtable);
            break;
            /*  under construction
        case IFJ_T_CONC:            // '.'
            error = operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_LESS:            // '<'
            error = operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_GREATER:         // '>'
            error = operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_LESS_EQUAL:      // '<='
            error = operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_GREATER_EQUAL:   // '>='
            error = operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_SUPER_EQUAL:     // '==='
            error = operator_function (token, change_token, buffer, hashtable);
            break;
        case IFJ_T_NOT_SUPER_EQUAL: // '!=='
            error = operator_function (token, change_token, buffer, hashtable);
            break;
            */
                
    /** TO DO
     *  when calling function, i shouldn't forget on creating new hashtable, when returning then free */
    }
    printf("error = %d token_id = %d token_data: %s \n", error, token->id, &(buffer->data[token->content]));    
    
    return error;
}


/*
 * Function that handles "*", "+", "-", "/"
 */
int basic_operator_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable)
{
    printf("operátor.\n");
    /* Allocating space for opperands */
    changeable_tokenPtr change_token_left;
    if ((change_token_left = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    changeable_tokenPtr change_token_right;
    if ((change_token_right = changeable_token_Init()) == NULL)
    {
        free(change_token_left);
        return IFJ_ERR_INTERNAL;
    }
    
    int error = call_leaf_function (token->LPtr, change_token_left, buffer, hashtable);
    if (!error)
    {
        error = call_leaf_function (token->RPtr, change_token_right, buffer, hashtable);
    }
    if (!error)
    {
        // start of "*", "+", "-", "/"
        if (token->id == IFJ_T_MUL || token->id == IFJ_T_PLUS || token->id == IFJ_T_MIN)
        {
            // good types
            if ((change_token_left->id == IFJ_T_INT || change_token_left->id == IFJ_T_DOUBLE) && (change_token_right->id == IFJ_T_INT || change_token_right->id == IFJ_T_DOUBLE))
            {
                /* @option int - true if result should be int, false if result should be double */
                int option_int = (change_token_left->id == IFJ_T_INT && change_token_right->id == IFJ_T_INT) && (!(token->id == IFJ_T_DIV));
                int operand_1_int;
                int operand_2_int;
                int result_int;
                double operand_1_double;
                double operand_2_double;
                double result_double;
                int length_result;
                char * result_string;
                
                // int and int
                if (option_int)
                {
                    operand_1_int = atoi(change_token_left->data);
                    operand_2_int = atoi(change_token_right->data);
                }
                // double and double, doble and int, int and double
                else
                {
                    operand_1_double = atof(change_token_left->data);
                    operand_2_double = atof(change_token_right->data);               
                }
                int length_left = strlen(change_token_left->data);
                int length_right = strlen(change_token_right->data);
                
                // do the operation
                if (token->id == IFJ_T_MUL)        // "*"
                {
                    if (option_int)
                    {
                        result_int = operand_1_int * operand_2_int;
                    }
                    else
                    {
                        result_double = operand_1_double * operand_2_double;                        
                    }
                    length_result = length_left > length_right ? (2 * length_left) : (2 * length_right);
                } 
                else if (token->id == IFJ_T_PLUS)  // "+"
                {
                    if (option_int)
                    {
                        result_int = operand_1_int + operand_2_int;
                    }
                    else
                    {
                        result_double = operand_1_double + operand_2_double;                        
                    }            
                    length_result = length_left > length_right ? (length_left + 1) : (length_right + 1);
                }
                else if (token->id == IFJ_T_MIN)  // "-"
                {
                    if (option_int)
                    {
                        result_int = operand_1_int - operand_2_int;
                    }
                    else
                    {
                        result_double = operand_1_double - operand_2_double;                        
                    }           
                    length_result = length_left > length_right ? (length_left) : (length_right);
                    
                }
                else if (token->id == IFJ_T_DIV)  // "/"
                {
                    // first check zero division
                    if (operand_2_double == 0)
                    {
                        changeable_token_Destroy (change_token_left);
                        changeable_token_Destroy (change_token_right);
                        return IFJ_ERR_ZERO_DIVISION;
                    }
                    else
                    {
                        result_double = operand_1_double / operand_2_double;                        
                    }           
                    length_result = length_left > length_right ? (length_left) : (length_right);
                    
                }
                result_string = malloc(sizeof(char) * (length_result + 3));
                // result is int
                if (option_int)
                {
                    sprintf(result_string, "%d", result_int);
                    change_token->id = IFJ_T_INT;
                }
                // result is double
                else
                {
                    sprintf(result_string, "%g", result_double);
                    change_token->id = IFJ_T_DOUBLE;
                }
                // update data
                error = changeable_token_update(change_token, result_string);
                free (result_string);
            }
            // bad types
            else
            {
                error = IFJ_ERR_TYPE_COMPATIBILITY;
            }
        } // end of "*", "+", "-", "/"
        changeable_token_Destroy (change_token_left);
        changeable_token_Destroy (change_token_right);
    }
    return error;
}

/* 
 * Function that handles numbers as terminals. 
 */
int number_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer)
{
    int error = changeable_token_Insert (change_token, token, buffer);
    return error;
}

/* 
 * Searches for the variable in hashtable. Retruns 0 if ok, otherwise returns nonzero value. 
 */
int var_function (TokenPtr token, changeable_tokenPtr change_token, BUFFER_STRUCT buffer, hashtable_item** hashtable)
{
    int error = 0;
    hashtable_item* item = search_hashtable (hashtable, &(buffer->data[token->content]));
    if (item == NULL)
    {
        return IFJ_ERR_UNDECLARED_VARIABLE;
    }
    /* if the data are found, get them from the hashtable */

    change_token->id = item->type;
    changeable_token_update(change_token, item->value);
    
    return error;
}

/*
 * function that handles '='
 */ 
int assignment (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer)
{
    int error = 0;
    changeable_tokenPtr change_token;
    if ((change_token = changeable_token_Init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    
    error = call_leaf_function (token->RPtr, change_token, buffer, hashtable);
    
    /* first check, if it even should try to insert, then it checks if the allocations in insert was fine */
    if ((!error) && (insert_item_hashtable (hashtable, &(buffer->data[token->content]), change_token->id, change_token->data)))
    {
        error = IFJ_ERR_INTERNAL;
    }
    free(change_token);
    return error;
}

/*
 * Chooses which function should be used, depends on token_id
 * This functions can only be in the leaf area
 */
int call_root_function (TokenPtr token, hashtable_item** hashtable, BUFFER_STRUCT buffer)
{
    /* error should be always changed */
    int error = IFJ_ERR_INTERNAL;
    
    switch (token->id)
    {
        case IFJ_T_ASSIGN:
            error = assignment (token, hashtable, buffer);
            break;
        case IFJ_T_EOF:
            error = 0;
            break;
    }
    return error;
}

/*
 * The core function that calls everything else
 */
int interpreter (BUFFER_STRUCT buffer, TokenPtr token) 
{
    int error = 0;
    hashtable_item** hashtable = hashtable_init();
    
    while(token != NULL)
    {
      
        if ((error = call_root_function (token, hashtable, buffer)) != 0)
        {
            printf("error = %d\n", error);
            hashtable_free(hashtable);
            return error;
        }
        token = token->next;
    }
    printf("error = %d\n", error);
    hashtable_free(hashtable);
    return error;
}
