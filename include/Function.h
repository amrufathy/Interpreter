#ifndef FUNCTION_H
#define FUNCTION_H

#include <vector>
#include <cstring>
#include <cctype>
#include "myfunctions.h"
#include "constants.h"

using namespace std;

class Function
{
    public:
        bool add(const char* name, char ** params, const int count, const char* exp);
        bool evalExpr(const char* name, char**, char*);
        bool exist(const char* name);
        int  getIndex(const char* name);

    private:
        struct VAR {
            char name[NAME_LEN_MAX+1];
            char expr[EXPR_LEN_MAX+1];
            char *params[PARAM_LEN_MAX];
            int numParams;
        };

        vector <VAR> variables;
};

#endif // FUNCTION_H
