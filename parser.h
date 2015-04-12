#ifndef PARSER_H
#define PARSER_H

// declarations
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <cmath>

#include "constants.h"
#include "error.h"
#include "myfunctions.h"
#include "variables.h"

using namespace std;

class Parser
{
    public:
        Parser();
        char* parse(const char expr[]);

    private:

        enum TOKENTYPE {NOTHING = -1, DELIMETER, NUMBER, VARIABLE, FUNCTION, UNKNOWN};

        enum OPERATOR_ID {AND, OR, BITSHIFTLEFT, BITSHIFTRIGHT, EQUAL, UNEQUAL, SMALLER, LARGER,
                          SMALLEREQ, LARGEREQ, PLUS, MINUS, MULTIPLY, DIVIDE, MODULUS, XOR,
                          POW, FACTORIAL};

    private:
        char expr[EXPR_LEN_MAX+1];
        char* e;
        char token[NAME_LEN_MAX+1];
        TOKENTYPE token_type;
        double ans;
        char ans_str[255];
        Variables user_var;

    private:
        void tokenize();

        double parseAssign();
        double parseBitshift();
        double parseCond();
        double parseOp1();
        double parseOp2();
        double parsePower();
        double parseFact();
        double parseNeg();
        double parseFunc();
        double parseParen();
        double parseNumber();

        int getOpID(const char op_name[]);
        double evalOp(const int op_id, const double &lhs, const double &rhs);
        double evalFunc(const char fn_name[], const double &value);
        double evalVar(const char var_name[]);

        int getRow();
        int getCol();
};

#endif
