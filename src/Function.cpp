#include "Function.h"

bool Function::add(const char* name, char ** params, const int count, const char* exp)
{
    VAR newVar;
    strncpy(newVar.name, name, NAME_LEN_MAX-1);
    strncpy(newVar.expr, exp, NAME_LEN_MAX-1);
    newVar.numParams = count;

    for (int i = 0; i < newVar.numParams; i++){
        newVar.params[i] = params[i];
    }

    int index = getIndex(name);
    if (index == -1)
        variables.push_back(newVar); // variable does not exist
    else
        variables[index] = newVar;  // variable exists

    return true;
}

bool Function::evalExpr(const char* name, char ** params, char* evaluated)
{
    int index = getIndex(name);
    if (index != -1){

        /*printf("Name: %s\nExpr: %s\nParams: ",variables[index].name,variables[index].expr);
        for(int i = 0; i < variables[index].numParams; i++) {
            printf("%s ",params[i]);
        }*/

        char temp[EXPR_LEN_MAX+1], temp2[EXPR_LEN_MAX+1];
        int paramIdx = -1;
        strcpy(temp, variables[index].expr);
        strcpy(temp2,temp);

        char * parts = strtok(temp2,"&|<>=+*%/^!");
        char * bibos[PARAM_LEN_MAX+1];
        int counter = -1;
        bibos[++counter] = parts;
        while (parts){
            parts = strtok(NULL,"&|<>=+*%/^!");
            bibos[++counter] = parts;
        }

        evaluated[0] = '\0';
        int mybibo = -1;
        for(unsigned i = 0; i < strlen(temp); i++){
            if (isalpha(temp[i])) {
                strcat(evaluated,params[++paramIdx]);
                for(unsigned j = 0; j < strlen(bibos[++mybibo])-1; j++){
                    i++;
                }
            }
            else{
                strncat(evaluated,&temp[i],1);
            }
        }
        //printf("\nevaluated: %s\n",evaluated);
        return true;
    }
    return false;
}

/**
 * Returns the index of the given name in the variable list.
 * Returns -1 if name is not present in the list.
 * Name is case sensitive.
 */
int Function::getIndex(const char* name)
{
    for (unsigned i = 0; i < variables.size(); i++){
        if(!strcmp(name, variables[i].name))
            return i;
    }
    return -1;
}

bool Function::exist(const char* name)
{
    return getIndex(name) != -1;
}
