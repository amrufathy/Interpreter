#ifndef VARIABLES_H
#define VARIABLES_H

#include <vector>
#include <cstring>
#include "myfunctions.h"
#include "constants.h"

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
