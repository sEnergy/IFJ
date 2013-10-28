/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            lex_analyzer.h
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Header file  of Lexical analyzer for team project
 *                      of IFJ course.
 *
*******************************************************************************/

typedef struct buffer_struct
{
	unsigned long long int max_length;
	unsigned long long int position;
	char* data;
}* BUFFER_STRUCT;

/* Comment this function */
int lex_analyzer (FILE *input, int *token_id, BUFFER_STRUCT buffer);

/*** End of file lex_analyzer.h ***/
