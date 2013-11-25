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

void TL_Init (TokenList *List)
{
    List->first = List->active = List->last = NULL;

    return;
}

// inserts new token in the end of list
int TL_Insert (TokenList *List, int token_id, BUFFER_STRUCT token_content)
{
    TokenPtr new = malloc(sizeof(*new));

    if (new == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    new->id = token_id;

    int size = (token_content->position) + 1;

    new->content = malloc(size*sizeof(char));

    while (size--)
    {
        new->content[size] = token_content->data[size];
    }

    if (List->last == NULL)
    {
        List->first = List->last = new;
        new->prev = new->next = NULL;
    }
    else
    {
        new->prev = List->last;
        new->next = NULL;
        List->last->next = new;
        List->last = new;
    }

    return 0;
}

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

void TL_ActiveReset (TokenList *List)
{
    List->active = List->first;
}

void TL_ActiveNext (TokenList *List)
{
    if (List->active != NULL)
    {
        List->active = List->active->next;
    }

    return;
}

void TL_ActivePrev (TokenList *List)
{
    if (List->active != NULL)
    {
        List->active = List->active->prev;
    }

    return;
}

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

void TL_Dispose (TokenList *List)
{
    TokenPtr tmp = List->first;

    if (tmp == NULL) // empty list - just NULLing of pointers
    {
        List->first = List->active = List->last = NULL;
    }
    else if (tmp->next == NULL) // just one token
    {
        free(List->last->content);
        free(List->last);
        List->first = List->active = List->last = NULL;
    }
    else
    {
        do // freeing tokens one by one
        {
            tmp = tmp->next;
            free(tmp->prev->content);
            free(tmp->prev);
        } while (tmp->next != NULL);

        // freeing last token manually
        free(List->last->content);
        free(List->last);
        List->first = List->active = List->last = NULL;
    }

    return;
}

/*** End of file token_list.c ***/
