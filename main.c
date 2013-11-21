/* *****************************************************************************
 *
 * Project name:        IFJ - Team project
 * Filename:            main.c
 * Author:              Marcel Fiala
 * Encoding:            UTF-8
 *
 * Description:         Main source file of team project for course IFJ
 *
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "errors.h"
#include "syntax_analyzer.h"
#include "ial.h"

int main (int argc, char *argv[])
{
    int code = 0;

    if (argc == 2)
    {
        printf("\nNow starting Syntax analyzer...\n");
        code = syntax_analyzer(argv[1]);
        printf("\nNow stopping Syntax analyzer...\n");
    }
    else
    {
        code = IFJ_ERR_INTERNAL;
    }

    /*
     * Ukázka toho, jak krásně nám jede algoritmus z IAL.c - pak to přesuneme
     * tam, kam to patří :-D
     */
    int found = -1;
    char* substring = "Marcel a Bambi jsou SUPER";
    char* string = "ds Maro Marcel a BambiMarcel a Bambi jsou SUPER das sacsaxdsax";
    code = search_substring(substring, string, &found);

    printf("\n**************************************************\nNow testing substring search\n");
    printf("**************************************************\n");
    printf("String:    %s\n", string);
    printf("Substring: ");
    if (found>-1)
    {int i = found; while(i--) printf(" ");}
    printf("%s\n", substring);
    printf("Substring found? %s", (found>-1)? "YES":"NO\n");
    if (found>-1) printf(" (position: %d)\n", found);
    printf("**************************************************\n\n");

    /*
     * Hashtable test
     */
     
    printf("\n**************************************************\nNow testing Hashtable\n");
    printf("**************************************************\n");
    char* hashtable_value1 = "ahoj";
    char* hashtable_value2 = "aloha";
    char* hashtable_value3 = "5";
    char* hashtable_string1 = "$a";
    char* hashtable_string2 = "$bubble";
    char* hashtable_string3 = "$Lubos Vanicek";
	hashtable_item** hashtable = hashtable_init();
	if (hashtable == NULL)
	{
		printf("Allocation Error.\n");
	}
	int error1 = insert_item_hashtable (hashtable, hashtable_string1, 15, hashtable_value1);
	int error2 = insert_item_hashtable (hashtable, hashtable_string2, 15, hashtable_value2);
	int error3 = insert_item_hashtable (hashtable, hashtable_string3, 15, hashtable_value3);		
	if (error1 || error2 || error3)
	{
		printf("Allocation Error.\n");
	}
	/* test print of the table */
	hashtable_item* item;
    for (unsigned int i = 0; i < SIZE_OF_HASHTABLE; ++i)
    {
		item = hashtable[i];
        while (item != NULL)
        {
            printf ("%d: název: %s, hodnota: %s, typ: %d.\n", i, item->string, item->value, item->type);
            item = item->next;
        }
    }
    hashtable_item** hashtable2 = copy_hashtable (hashtable);
	if (hashtable2 == NULL)
	{
		printf("Allocation Error.\n");
	}    
	/* test print of the table */
    for (unsigned int i = 0; i < SIZE_OF_HASHTABLE; ++i)
    {
		item = hashtable2[i];
        while (item != NULL)
        {
            printf ("%d: název: %s, hodnota: %s, typ: %d.\n", i, item->string, item->value, item->type);
            item = item->next;
        }
    }    
	hashtable_free (hashtable);
	hashtable_free (hashtable2);
	printf("**************************************************\n\n");
	
	return code;
}

/*** End of file main.c ***/
