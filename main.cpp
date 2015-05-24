#include <cstdio>

#include "parser.h"

using namespace std;

int main()
{
    char expr[255];

    Parser prs;

    do{
        printf(">> ");
        gets(expr);

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
    } while (strcmp(expr, "exit"));


    return EXIT_SUCCESS;
}
