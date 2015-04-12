#include <cstdlib>
#include <cstdio>

#include "parser.h"

using namespace std;

int main()
{
    char expr[255];

    Parser prs; // object of class parser

    printf("Enter an expression an press Enter to calculate the result.\n");
    puts("Enter an empty expression to quit.\n\n");

    do{
        printf("> ");
        //fflush(stdin);
        gets(expr);
        //fflush(stdout);

        if (strcmp(expr, "")){
            try{    // parse
                char* result;
                result = prs.parse(expr);
                printf("\t%s\n", result);
            }
            catch (...){
                printf("\tError: Unknown error occurred in parser\n");
            }
        }
    } while (strcmp(expr, ""));

    return EXIT_SUCCESS;
}
