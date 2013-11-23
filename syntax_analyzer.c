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

#include <stdio.h>  // pak můžu smazat
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token_id.h"
#include "syntax_analyzer.h"
#include "errors.h"

int buffer_init(BUFFER_STRUCT buffer)
{
    buffer->data = (char*) malloc(IFJ_SYNTAX_DEF_BUFF_SIZE*sizeof(char));
    if (buffer->data == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    else
    {
        buffer->data[1] = '\0';
        buffer->max_length = IFJ_SYNTAX_DEF_BUFF_SIZE;
        buffer->position = 0;
    }
    return 0;
}

int check_open_tag (FILE* input)
{
    unsigned char open_tag_size = IFJ_OPEN_TAG_SIZE;

    char *open_tag = (char*) malloc(open_tag_size*sizeof(char));

    if (open_tag == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }

    while (open_tag_size--) open_tag[open_tag_size] = '\0';

    if ((fscanf(input, "%5c", open_tag) != 1) || strcmp(open_tag, "<?php") != 0
            || !isspace(fgetc(input)))
    {
        free(open_tag);
        return IFJ_ERR_SYNTAX;
    }
    else
    {
        free(open_tag);
        return 0;
    }

}

int syntax_analyzer (char* input_filename)
{
    int code = 0;

    BUFFER_STRUCT token_data = NULL;
    if ((token_data = (BUFFER_STRUCT) malloc(sizeof(struct buffer_struct))) == NULL)
    {
        return IFJ_ERR_INTERNAL;
    }
    else
    {
        if (buffer_init(token_data) == IFJ_ERR_INTERNAL)
        {
            free(token_data);
            return IFJ_ERR_INTERNAL;
        }
    }

    int token_id = -1;

    FILE* input = fopen(input_filename, "r");
    if (input == NULL)
    {
        free(token_data->data);
        free(token_data);
        return IFJ_ERR_INTERNAL;
    }

    if ((code = check_open_tag (input)) != 0)
    {
        fclose(input);
        free(token_data->data);
        free(token_data);

        return code;
    }

    int token_number = 1;

    while ((code = lex_analyzer(input, &token_id, token_data)) == 0)
    {
         printf("Token number: %i\nToken ID: %i\nToken data: %s\n\n", token_number++,
        token_id, token_data->data);

        (void)token_number;
        token_data->data[0] = '\0';
        token_data->position = 0;

        if (token_id == IFJ_T_EOF) break;
    }

    fclose(input);
    free(token_data->data);
    free(token_data);
    return code;
}

/*** End of file syntax_analyzer.c ***/
