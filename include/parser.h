#ifndef PARSER_H
#define PARSER_H

#include <cstdlib>
#include <cctype>
#include <cmath>

#include "constants.h"
#include "error.h"
#include "myfunctions.h"
#include "variables.h"
#include "Function.h"
#include "Modes.h"

using namespace std;

class Parser
{
    public:
        Parser();
        char* parse(const char expr[]);

    private:
        enum TOKENTYPE {NOTHING = -1, DELIMETER, NUMBER, VARIABLE, FUNCTION, UNKNOWN};
        enum OPERATOR_ID {AND, OR, BITSHIFTLEFT, BITSHIFTRIGHT, EQUAL, UNEQUAL, SMALLER, LARGER,
                          SMALLEREQ, LARGEREQ, PLUS, PLUSPLUS, MINUS, MINUSMINUS, MULTIPLY, DIVIDE, MODULUS, XOR,
                          POW, FACTORIAL};

    private:
        char expr[EXPR_LEN_MAX+1];
        char *e;
        char token[NAME_LEN_MAX+1], tokenNew[NAME_LEN_MAX+1];
        TOKENTYPE tokenType, tokenTypeNew;
        double answer;
        char answer_str[255];
        Variables user_var;
        Function user_func;
        Modes modes;

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
        double parseIncrement();
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
