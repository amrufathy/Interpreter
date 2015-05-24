#ifndef VARIABLES_H
#define VARIABLES_H

#include <vector>
#include <cstring>
#include <cctype>
#include "myfunctions.h"
#include "constants.h"
#include "Hash.h"

using namespace std;

class Variables {
    public:
        bool add(const char*, double);
        bool getValue(const char*, double*);
        int  getIndex(const char*);

    private:
    hash HASH;
        /*struct VAR {
            char name[NAME_LEN_MAX+1];
            double value;
        };

        vector <VAR> variables;*/

};

#endif
