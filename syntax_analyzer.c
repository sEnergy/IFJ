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

#include "lex_analyzer.h"
#include "syntax_analyzer.h"
#include "token_id.h"
#include "errors.h"
#include "token_list.h"
#include "interpreter.h"

//Return pointer to initialized token on heap. Null if internal error.
TokenPtr new_token(void)
{
    TokenPtr token = NULL;
    if ((token =(TokenPtr) malloc(sizeof(struct Token))) == NULL)
    {
        return NULL;
    }
    token->id = 50;
    token->content = 0;
    token->LPtr = NULL;
    token->RPtr = NULL;
    token->next = NULL;
    token->condition = NULL;
    return token;
}
    
//jebnut ma ide z tohto

/*
 * Function apply defined rules on input and check errors.
 */ 
int work(Stack_t* stack, TokenList* list)
{
    List_itemPtr proceeded = list->active->LPtr;
    int i = 0;
// There are rules only for 3 tokens.
    while(proceeded != NULL && proceeded->content != S_top(stack))
    {
        i++;
        proceeded = proceeded->LPtr;
        if (i>3)
        {
            return IFJ_ERR_SYNTAX;
        }
    }
    proceeded = list->active->LPtr; 
    if (i!=3)
    {
        return IFJ_ERR_SYNTAX;
    }
//E->(E)
    if (proceeded->content->id == IFJ_T_RB)
    {  
        List_itemPtr expression = proceeded->LPtr;
        List_itemPtr l_lb = expression->LPtr;
        if (expression->is_expression &&
            l_lb->content->id == IFJ_T_LB)
        {
            expression->LPtr = l_lb->LPtr;
            expression->RPtr = proceeded->RPtr;
            proceeded->RPtr->LPtr = expression;
            l_lb->LPtr->RPtr = expression;
            free(l_lb);
            l_lb = NULL;
            free(proceeded);
            proceeded = NULL;
            S_pop(stack);
        }
        else
        {
            return IFJ_ERR_SYNTAX;
        }
    }
//E->E operator E
    else if(proceeded->is_expression)
    {    
        List_itemPtr operator = proceeded->LPtr;
        List_itemPtr l_op = operator->LPtr;
        if (!operator->is_expression &&
            l_op->is_expression)
        {
            operator->content->LPtr = l_op->content;
            operator->content->RPtr = proceeded->content;
            operator->LPtr = l_op->LPtr;
            operator->RPtr = proceeded->RPtr;
            proceeded->RPtr->LPtr = operator;
            l_op->LPtr->RPtr = operator;
            free(l_op);
            l_op = NULL;
            free(proceeded);
            proceeded = NULL;
            S_pop(stack);
            operator->is_expression = TRUE;
        }
        else
        {
            return IFJ_ERR_SYNTAX;
        }
    }
    else
    {
        return IFJ_ERR_LEXICAL;
    }
    return 0;
}
/*
 * Function return left-closest from active TERMINAL on input
 */         
TokenPtr Lclosest_term(TokenList* list)
{
    List_itemPtr proceeded = list->active->LPtr;
    while(proceeded->is_expression == TRUE)
    {
        proceeded = proceeded->LPtr;
    }
    return proceeded->content;
}
/* 
 * Function perform precedence syntax analyze and create abstract syntax 
 * tree of the operations, that are ordered accoring to precedence table
*/
int PSA(TokenList* list)
{
    Stack_t stack;
    TokenPtr start;
    TokenPtr end;
    TokenPtr left_closest_term;
    int code;
    
    
    S_init(&stack);
    int table[14][14] = 
    {
      // false - 0     < - 1      > - 2      = - 3        succes - 4
      
// input  *   /   +   -   .   <   >  <=  >=  === !==  (   )   $  
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // *
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // /
        { 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // +
        { 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // -
        { 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // .
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // <
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // >
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // <=
        { 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 2 , 2 , 1 , 2 , 2}, // >=
        { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 1 , 2 , 2}, // ===
        { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 1 , 2 , 2}, // !==
        { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 3 , 0}, // (
        { 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 2 , 0 , 2 , 2}, // )
        { 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0 , 4}, // $
    };
    if((start = new_token()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    if((end = new_token()) == NULL)
    {
        free(end);
        end = NULL;
        return IFJ_ERR_INTERNAL;
    }
// Create start and end terminals and joint it to the input.
    start->id = IFJ_T_MOD;
    end->id = IFJ_T_MOD;
    TL_Insert_Last(list,end);
    TL_Insert_First(list,start);
    TL_ActiveReset(list);
    TL_ActiveNext(list);
    while (1)
    {
        if (Lclosest_term(list) == start && TL_GetID(list) == end)
        {
            break;
        }
// Simulation of the rule E->i
        if (is_terminal(list->active->content->id)) 
        {
            list->active->is_expression = TRUE;
            TL_ActiveNext(list);
            continue;
        }
// Lookup rule for left-closest TERMINAL on the input and actual terminal on the input.
// There is -2 in in indexes due to starting operator tokens on position 2 in enum.       
        left_closest_term = Lclosest_term(list);
        switch (table[left_closest_term->id-2][list->active->content->id-2])
        {
            
            case 4:
                break;
            case 3:
                TL_ActiveNext(list);
                continue;
            case 2:
                if ((code = work(&stack,list)) != 0)
                {
                    return IFJ_ERR_SYNTAX;
                }
                continue;
            case 1:
// Stack is stopper. < character in slides.
                S_push(&stack,left_closest_term);
                TL_ActiveNext(list);
                continue;
            case 0:            
                return IFJ_ERR_SYNTAX;
                break;
        }
    }
// Structure should have one exact formant at the end.
    if (!S_empty(&stack) || 
        !(list->first->content->id == IFJ_T_MOD &&
          list->last->content->id == IFJ_T_MOD &&
          list->first->RPtr == list->last->LPtr &&
          list->first != list->last))
    {
//        S_dispose(&stack);
        return IFJ_ERR_SYNTAX;
    }
    return 0;
}
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
    open_tag = NULL;
    return code;
}

/*
 * Main function of syntax analyzer - handles file, calls all other functions
 * and so on...
 */
int syntax_analyzer (char* input_filename)
{
    int code;
    FILE* input;
    BUFFER_STRUCT token_content = NULL; // structure for token data
    TokenPtr token = NULL;
    function_hashtablePtr* FHTable = NULL;
    
    //first token
    if ((token = new_token())==NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    
    //initialization of hashtable
    if ((FHTable = function_hashtable_init()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
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
            token_content = NULL;
            return IFJ_ERR_INTERNAL;
        }
    }
    // start syntax analyze itself
    code = check_syntax(input, &token, FHTable, token_content);
    // starts interpret
    if (code == 0)
    {
		code = interpreter(token_content, token);
	}
    
    // free all allocated memory, close file
    fclose(input);
    free(token_content->data);
    token_content->data = NULL;
    free(token_content);
    token_content = NULL;

    return code;
}

/*
 * In infitine loop it reads from input source file and checks their order.
 * Only SYNTAX ERROR or EOF breaks cycle (and of course, INTERNAL ERROR).
 */
int check_syntax (FILE *input, TokenPtr* token_oldPtr, function_hashtablePtr* HTable, BUFFER_STRUCT big_string)
{
    int code;
    TokenPtr token = *token_oldPtr;
    TokenPtr token_new = NULL;
    TokenPtr* ancestorPtr = token_oldPtr;
    while (1)
    {
        if ((code = lex_analyzer(input, token, big_string)) != 0)
        {
            return code;
        }
        if (token->id == IFJ_T_KEYWORD)
        {
            if (strcmp(&big_string->data[token->content], "function\0") == 0) // function declaration
            {
                if((code = check_function_declaration (input, token, big_string)) != 0)
                {
                    return code;
                }
                if ((code = function_hashtable_insert(HTable, token, big_string)) != 0)
                {
                    return code;
                }
                if ((token = new_token())==NULL)
                {
                    return IFJ_ERR_INTERNAL;
                }
                continue;
            }
            else if ((strcmp(&big_string->data[token->content], "if\0") != 0)
                        && (strcmp(&big_string->data[token->content], "while\0") != 0)
                        && (strcmp(&big_string->data[token->content], "return\0") != 0))
            {
                return IFJ_ERR_SYNTAX;
            }
            else // return statement, if-else construction or while cycle
            {
                if((code = check_statement (input, ancestorPtr, big_string)) != 0)
                {
                    return code;
                }
            }
        }
        else if (token->id == IFJ_T_EOF) // in"body", EOF is legal
        {
            return 0;
        }
        else if (token->id == IFJ_T_VARIALBE) // assign statement
        {
            if((code = check_statement (input, ancestorPtr, big_string)) != 0)
            {
                return code;
            }
        }
        else
        {
            return IFJ_ERR_SYNTAX;
        }
        if ((token_new = new_token())==NULL)
        {
            return IFJ_ERR_INTERNAL;
        }
        (*ancestorPtr)->next = token_new;
        ancestorPtr = &(*ancestorPtr)->next;
        token = token_new;
    }
}

/*
 * Checks syntax of whole function declaration - identificator, parameter list
 * in braces and statement list in curly braces.
 */
int check_function_declaration (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string)
{
    int code;

    // first, there must be indentificator
    if ((code = lex_analyzer(input, token_old, big_string)) != 0)
    {
        return code;
    }
    else if (token_old->id != IFJ_T_ID)
    {
        return IFJ_ERR_SYNTAX;
    }
    
    // test of parameter list
    if ((code = check_param_list (input, token_old, big_string)) != 0)
    {
        return code;
    }
    // after parameter list, there must be corrent statement list
    if ((code = check_stat_list (input, &token_old->LPtr, big_string)) != 0)
    {
        return code;
    }
//    printf("\nfinal %d\n",token_old->LPtr->id);

    return 0;
}

/*
 * Checks parameter list for fuction definiton - statement list betwenn curly
 * braces.
 */
int check_param_list (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string)
{
    int code;
    TokenPtr token = NULL;
    
    //setting number of parameters to zero
    if ((token = new_token())==NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    token->id = IFJ_T_MOD;
    token->content = 0;
    token_old->condition = token;

    // check, if there is openin brace '('
    if ((token = new_token())==NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    if ((code = lex_analyzer(input, token, big_string)) != 0)
    {
        free(token);
        token = NULL;
        return code;
    }
    if (token->id != IFJ_T_LB)
    {
        free(token);
        token = NULL;
        return IFJ_ERR_SYNTAX;
    }

    if ((code = lex_analyzer(input, token, big_string)) != 0)
    {
        free(token);
        token = NULL;
        return code;
    }
    while (1)
    {
        if (is_terminal(token->id))
        {
            token_old->condition->content++;
            if ((code = lex_analyzer(input, token, big_string)) != 0)
            {
                free(token);
                token = NULL;
                return code;
            }
            if (token->id == IFJ_T_SEP)
            {
                if ((code = lex_analyzer(input, token, big_string)) != 0)
                {
                    free(token);
                    return code;
                }
                if (!is_terminal(token->id))
                {
                    return IFJ_ERR_SYNTAX;
                }
                continue;
            }
            else if (token->id != IFJ_T_RB)
            {
                return IFJ_ERR_SYNTAX;
            }
        }
        else if (token->id != IFJ_T_RB)
        {
            return IFJ_ERR_SYNTAX;
        }
        break;
    }
    free(token);
    token = NULL;
    return 0;
}

/*
 * Reads whole expression, saves all tokens into list and than calls function,
 * which test the syntax of the expression.
 */
int check_expression (FILE *input, TokenPtr* token_oldPtr,
            unsigned int end_token, bool already_read, BUFFER_STRUCT big_string)
{
    int code;
    int rb_needed = 0;
    TokenPtr token = NULL;
    TokenList t_list;
    TL_Init(&t_list);
    if ((token = new_token())==NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    // if current token does not belong to expression, we need to read one
    if (!already_read)
    {
        if ((token = new_token()) == NULL)
        {
            return IFJ_ERR_INTERNAL;
        }
        if ((token = new_token())==NULL)
        {
            free(token);
            token = NULL;
            return IFJ_ERR_INTERNAL;
        }
        if ((code = lex_analyzer(input, token, big_string)) != 0)
        {
            free(token);
            token = NULL;
            return code;
        }
    }
    else
    {
        token = *token_oldPtr;
    }
    do // until end of file
    {
        if (token->id == end_token && rb_needed == 0) // must be end token and 0
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
//                TL_Dispose(&t_list);
                return IFJ_ERR_SYNTAX;
            }
        }
        // if valid token (literal or variable) ir read, save it to thel list
        else if ((is_operator(token->id) || is_terminal(token->id)
            || token->id == IFJ_T_RB || token->id == IFJ_T_LB) && token->id != IFJ_T_ASSIGN)
        {
            if (token->id == IFJ_T_LB)
            {
                rb_needed++; // every '(' means there needs to be another ')'
            }
            else if (token->id == IFJ_T_RB)
            {
                rb_needed--; // every ')' decrements number of needed ')'
            }

            code = TL_Insert_Last(&t_list, token);
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
        else 
        {
            if ((token = new_token())==NULL)
            {
                return IFJ_ERR_INTERNAL;
            }
            if ((code = lex_analyzer(input, token, big_string)) != 0)
            {
                free(token);
                token = NULL;
                return code;
            }
        }
        
    } while (token->id != IFJ_T_EOF);
    if (token->id == IFJ_T_EOF)
    {
        return IFJ_ERR_SYNTAX;
    }
    /* FROM THIS PLACE, CALL EXPRESSION SYNTAX CHECK*/
    if ((code = PSA(&t_list)) != 0)
    {
        return code;
    }
    if (token->id == IFJ_T_EOF)
    {
        (*token_oldPtr)->next = token;
    }
    *token_oldPtr = t_list.first->RPtr->content;    
//    TL_Dispose(&t_list);
    return 0;
}

/*
 * This function is called either with "if", "while" or "variable" token loaded,
 * or i may be start of "return" expression (in this case, i need to read "return"
 * token first)
 */
int check_statement (FILE *input, TokenPtr* token_oldPtr, BUFFER_STRUCT big_string)
{
    
    int code;
    TokenPtr token_old = *token_oldPtr;
    TokenPtr token_new = NULL;
    TokenPtr token = NULL;
    
    if (token_old->id == IFJ_T_VARIALBE) // possible start of assign
    {   
        if ((token_new = new_token())==NULL)
        {
            return IFJ_ERR_INTERNAL;
        }
        token_new->LPtr = *token_oldPtr;
        *token_oldPtr = token_new;
        token = token_new;
        if ((code = lex_analyzer(input, token, big_string)) != 0)
        {
            return code;
        }
        else if (token->id == IFJ_T_ASSIGN) // there must be '=' after variable
        {
            if ((token_new = new_token())==NULL)
            {
                return IFJ_ERR_INTERNAL;
            }
            token->RPtr = token_new;
            if ((code = lex_analyzer(input, token_new, big_string)) != 0)
            {
                return code;
            }
            else if (token_new->id == IFJ_T_ID)
            {
                // assign of return value
                return check_function_call (input, token_new, big_string);
            }
            else
            {
                // assign from expression
                return check_expression (input, &(token->RPtr), IFJ_T_SEMICOLON, TRUE, big_string);
            }
        }
        else
        {
            return IFJ_ERR_SYNTAX;
        }
    }
    else if (strcmp(&big_string->data[token_old->content], "if\0") == 0)
    {
        return check_if_else (input, token_old, big_string);
    }
    else if (strcmp(&big_string->data[token_old->content], "while\0") == 0)
    {
        return check_while(input, token_old, big_string);
    }
    else if (strcmp(&big_string->data[token_old->content], "return\0") == 0)
    {
        // there must be expression ending with semicolon
        return check_expression (input, &(*token_oldPtr)->LPtr, IFJ_T_SEMICOLON, FALSE, big_string);
    }

    return IFJ_ERR_SYNTAX; // if control gets here, it is definitely error
}

/*
 * Check correct function call - this is called after id token is checked, so
 * it checks only if there is input parameter list in between braces followed
 * by semicolon.
 */
int check_function_call (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string)
{
    int code;
    TokenPtr token = NULL;
    TokenPtr* ancestor = &(token_old->condition);

    // check, if there is openin brace '('
    if ((token = new_token())==NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    if ((code = lex_analyzer(input, token, big_string)) != 0)
    {
        free(token);
        return code;
    }
    if (token->id != IFJ_T_LB)
    {
        free(token);
        token = NULL;
        return IFJ_ERR_SYNTAX;
    }

    if ((code = lex_analyzer(input, token, big_string)) != 0)
    {
        free(token);
        token = NULL;
        return code;
    }
    while (1)
    {
        if (is_terminal(token->id))
        {
            *ancestor = token;
            ancestor = &(token->next);
            if ((token = new_token())==NULL)
            {
                return IFJ_ERR_INTERNAL;
            }
            if ((code = lex_analyzer(input, token, big_string)) != 0)
            {
                free(token);
                token = NULL;
                return code;
            }
            if (token->id == IFJ_T_SEP)
            {
                if ((code = lex_analyzer(input, token, big_string)) != 0)
                {
                    free(token);
                    return code;
                }
                if (!is_terminal(token->id))
                {
                    return IFJ_ERR_SYNTAX;
                }
                continue;
            }
            else if (token->id != IFJ_T_RB)
            {
                return IFJ_ERR_SYNTAX;
            }
        }
        else if (token->id != IFJ_T_RB)
        {
            return IFJ_ERR_SYNTAX;
        }
        break;
    }       
    if ((code = lex_analyzer(input, token, big_string)) != 0)
    {
        free(token);
        token = NULL;
        return code;
    }
    if (token->id != IFJ_T_SEMICOLON)
    {
        free(token);
        token = NULL;
        return IFJ_ERR_SYNTAX;
    }
    free(token);
    token = NULL;
    return 0;
}

// Checks syntax of while construction - condition folowed by statement list
int check_while (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string)
{
    int code;

    if ((code = check_condition (input, token_old, big_string)) != 0)
    {
        return code;
    }

    if ((code = check_stat_list (input, &token_old->LPtr, big_string)) != 0)
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
int check_if_else (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string)
{
    int code;
    TokenPtr token=NULL;
    if ((token = new_token()) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    // condition
    if ((code = check_condition (input, token_old, big_string)) != 0)
    {
        return code;
    }

    // than first statement list for case conditon == TRUE
    if ((code = check_stat_list (input, &token_old->LPtr, big_string)) != 0)
    {
        return code;
    }

    // after that, there must be "else" keyword
    
    if ((code = lex_analyzer (input, token, big_string)) != 0)
    {
        free(token);
        token = NULL;
        return code;
    }
    else if (strcmp(&big_string->data[token->content], "else\0") != 0)
    {
        free(token);
        token = NULL;
        return IFJ_ERR_SYNTAX;
    }
    
    // and statement list for case conditon != TRUE
    if ((code = check_stat_list (input, &token_old->RPtr, big_string)) != 0)
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
int check_stat_list (FILE *input, TokenPtr* token_oldPtr, BUFFER_STRUCT big_string)
{
    int code;

    TokenPtr token = NULL;
    TokenPtr token_next = NULL;
    TokenPtr* ancestorPtr = token_oldPtr;
    if ((token = new_token())==NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    (*token_oldPtr) = token;
    if ((code = lex_analyzer(input, token, big_string)) != 0)
    {
        return code;
    }
    else if (token->id == IFJ_T_LCB) // '{' - start of scope
    {
        if ((code = lex_analyzer(input, token, big_string)) != 0)
        {
            return code;
        }
        // this is illegal - the scope must not be empty ("{}")
        else if (token->id == IFJ_T_RCB)
        {
            return IFJ_ERR_SYNTAX;
        }

        do // until the end of scope '}'
        {
            /*
             * Only variable, if-else, while or return is start of statement.
             * Everything else is means SYNTAX ERROR
             */
            if ((strcmp(&big_string->data[token->content], "if\0") != 0)
                        && (strcmp(&big_string->data[token->content], "while\0") != 0)
                        && (strcmp(&big_string->data[token->content], "return\0") != 0)
                        && (token->id != IFJ_T_VARIALBE))
            {
                return IFJ_ERR_SYNTAX;
            }
            else
            {
                if((code = check_statement (input, ancestorPtr, big_string)) != 0)
                {
                    return code;
                }
            }
            // read of next token
            if ((token_next = new_token())==NULL)
            {
                return IFJ_ERR_INTERNAL;
            }
            (*ancestorPtr)->next = token_next;
            ancestorPtr = &(*ancestorPtr)->next;
            token = token_next;
            
            if ((code = lex_analyzer(input, token, big_string)) != 0)
            {
                return code;
            }
        } while (token->id != IFJ_T_RCB);
        *ancestorPtr = NULL;
        free(token);
        token = NULL;
    }
    else // scope did not start with '}'
    {
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
int check_condition (FILE *input, TokenPtr token_old, BUFFER_STRUCT big_string)
{
    int code;

    TokenPtr token = NULL;
    if ((token = new_token())==NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    if ((code = lex_analyzer(input, token, big_string)) != 0)
    {
        free(token);
        token = NULL;
        return code;
    }
    else if (token->id == IFJ_T_LB) // there must be '(' before expression
    {
        free(token);
        token = NULL;
        return check_expression (input, &token_old->condition, IFJ_T_RB, FALSE, big_string);
    }
    return IFJ_ERR_SYNTAX;
}

// if token is variable or literal, return TRUE
bool is_terminal (int token)
{
    if (IFJ_T_VARIALBE <= token && token <= IFJ_T_STRING)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

// if token id corresponds with operator that could be in expression, return TRUE
bool is_operator (int token)
{
    if ((IFJ_T_ASSIGN <= token && token <= IFJ_T_NOT_SUPER_EQUAL))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*** End of file syntax_analyzer.c ***/
