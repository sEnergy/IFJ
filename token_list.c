/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            token_list.c
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Implementation of funtions for list of tokens.
 *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "errors.h"
#include "token_list.h"
#include "syntax_analyzer.h"

// stack init
void S_init(Stack_t* S)
{
    S->Top = NULL;
}
// is stack empty?
bool S_empty(Stack_t* S)
{
    return (S->Top == NULL);
}
// insert token
void S_insert(Stack_t* S, TokenPtr token)
{
    Stack_itemPtr new_item = (Stack_itemPtr) malloc(sizeof(struct Stack_item));
    if (new_item == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    new_item->content = TokenPtr;
    new_item->next = S->Top;
    S->Top = new_item;
}
// top&pop
void S_top_pop(Stack_t* S, TokenPtr* token)
{
    if (S_Empty(S)) return -1;
    *token = S->Top->content;
    Stack_itemPtr tmp = S->Top;
    S->Top = S->Top->next;
    free(S->Top);
}
// dispose
void S_dispose(Stack_t* S)
{
    if (S_empty(s)) return;
    Stack_itemPtr tmp = S->Top;
    Stack_itemPtr next = S->Top->next;
    while (tmp != NULL)
    {
        free(tmp);
        tmp = next;
        if (next != NULL) 
        {
            next = next->next;
        }
    }
}

// initializes token list
void TL_Init (TokenList *List)
{
    List->first = List->active = List->last = NULL;

    return;
}

// inserts new item to the end of token list
int TL_Insert (TokenList *List, TokenPtr token)
{
    List_itemPtr new = malloc(sizeof(struct List_item));

    if (new == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    new->content = token;
    

    if (List->last == NULL)
    {
        List->first = List->last = new;
        new->LPtr = new->RPtr = NULL;
    }
    else
    {
        new->LPtr = List->last;
        new->RPtr = NULL;
        List->last->RPtr = new;
        List->last = new;
    }

    return 0;
}

// returns true if token list is empty; returns false otherwise
bool TL_IsEmpty (TokenList *List)
{
    if (List->first == NULL)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// sets first item as active
void TL_ActiveReset (TokenList *List)
{
    List->active = List->first;
}

// shifts activity one token right, if possible
void TL_ActiveNext (TokenList *List)
{
    if (List->active != NULL)
    {
        List->active = List->active->RPtr;
    }

    return;
}

// shifts activity one token left, if possible
void TL_ActivePrev (TokenList *List)
{
    if (List->active != NULL)
    {
        List->active = List->active->LPtr;
    }

    return;
}

// sets *id to id of active token if possilbe; else sets *id to -1
void TL_GetID (TokenList *List, unsigned int *id)
{
    if (List->active != NULL)
    {
        *id = List->active->id;
    }
    else
    {
        *id = -1;
    }

    return;
}

/*
 * Sets *content to pointer to content of active token if possilbe; else sets
 * *content to NULL
 * PROBABLY NOT NEEDED
void TL_GetContent (TokenList *List, char** content)
{
    if (List->active != NULL)
    {
        *content = List->active->content;
    }
    else
    {
        *content = NULL;
    }

    return;
}
*/
// deletes whole token list
void TL_Dispose (TokenList *List)
{
    TokenPtr tmp = List->first;

    if (tmp == NULL) // empty list - just NULLing of pointers
    {
        List->first = List->active = List->last = NULL;
    }
    else if (tmp->RPtr == NULL) // just one token
    {
        free(List->last->content);
        free(List->last);
        List->first = List->active = List->last = NULL;
    }
    else
    {
        do // freeing tokens one by one
        {
            tmp = tmp->RPtr;
            free(tmp->LPtr->content);
            free(tmp->LPtr);
        } while (tmp->RPtr != NULL);

        // freeing last token manually
        free(List->last->content);
        free(List->last);
        List->first = List->active = List->last = NULL;
    }

    return;
}

/*** End of file token_list.c ***/
