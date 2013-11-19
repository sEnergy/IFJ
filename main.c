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

    return code;

}

/*** End of file main.c ***/
