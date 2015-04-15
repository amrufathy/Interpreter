#include "Modes.h"

Modes::Modes()
{
    eptr = NULL;
    expression[0] = '\0';
    mode[0] = '\0';
}

void Modes::moder()
{
    char expr[EXPR_LEN_MAX+1];

    printf("> ");
    gets(expr);

    while(strcmp(expr,"exit") && strcmp(expr,"")){
        char* result;
        result = getMode(expr);
        printf("\t%s\n",result);
        printf("> ");
        fflush(stdin);
        gets(expr);
    }
}

bool isAlpha2(const char c)
{
    if (c == 0) return 0;
    return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ_", toupper(c)) != 0;
}

void Modes::getToken()
{
    char *mod;
    mod = mode;
    *mod = '\0';

    while (*eptr == ' ' || *eptr == '\t')
        eptr++;

    if (isAlpha2(*eptr)){
        while (isAlpha2(*eptr)){
            *mod = *eptr;
            eptr++; mod++;
        }
        mod = '\0';
        if (!strcmp(mode,"eqn")){
            mode_type = EQN;
            return;
        }
    }
}

char* Modes::getMode(const char newExpr[])
{
    strncpy(expression, newExpr, EXPR_LEN_MAX-1);
    eptr = expression;
    char ans_string[EXPR_LEN_MAX+1];
    getToken();
    if(mode_type == EQN){
        char eqnExpr[EXPR_LEN_MAX+1], *end1, *end2;
        printf("Please enter the coefficients of the quadratic equation\n");
        gets(eqnExpr);
        int a = strtod(eqnExpr,&end1);
        int b = strtod(end1,&end2);
        int c = strtod(end2,NULL);
        float x1 = ((float)(-1.0 * b) + (sqrt((b * b) - (4 * a * c)))) / (2.0 * a);
        float x2 = ((float)(-1.0 * b) - (sqrt((b * b) - (4 * a * c)))) / (2.0 * a);
        snprintf(ans_string,sizeof(ans_string),"X1 = %g , X2 = %g",x1,x2);
    }
    return ans_string;
}
