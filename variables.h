#ifndef USER_VARIABLES_H
#define USER_VARIABLES_H

#include <cstdio>
#include <cctype>
#include <cstring>
#include <vector>

#include "constants.h"
#include "myfunctions.h"

using namespace std;

class Variables {
    public:
        bool add(const char* name, double value);
        bool getValue(const char* name, double* value);
        int  getIndex(const char* name);

    private:
        struct VAR {
            char name[NAME_LEN_MAX+1];
            double value;
        };

        vector <VAR> variables;
};

#endif
