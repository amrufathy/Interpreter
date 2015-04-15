#ifndef MODES_H
#define MODES_H

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cmath>
#include "constants.h"

class Modes
{
    public:
        Modes();
        void moder();

    private:
        enum MODETYPE {EQN};

    private:
        char expression[EXPR_LEN_MAX+1];
        char *eptr;
        char mode[EXPR_LEN_MAX+1];
        MODETYPE mode_type;

    private:
        void getToken();
        char* getMode(const char newExpr[]);

};

#endif // MODES_H
