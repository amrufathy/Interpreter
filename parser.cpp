#include "parser.h"

using namespace std;

/// Initializes all data with zeros and empty strings
Parser::Parser()
{
    expr[0] = '\0';
    e = NULL;
    token[0] = '\0';
    token_type = NOTHING;
}

/**
 * parses and evaluates the given expression
 * On error, an error of type Error is thrown
 */
char* Parser::parse(const char new_expr[])
{
    try {
        if ((int)strlen(new_expr) > EXPR_LEN_MAX)
            throw Error(getRow(), getCol(), 200);

        strncpy(expr, new_expr, EXPR_LEN_MAX - 1);
        e = expr;
        ans = 0;
        tokenize();
        if (token_type == DELIMETER && *token == '\0'){
            throw Error(getRow(), getCol(), 4);
        }
        ans = parseAssign();
        // check for garbage at the end of the expression
        // an expression ends with a character '\0' and token_type = delimeter
        if (token_type != DELIMETER || *token != '\0'){
            if (token_type == DELIMETER)
                throw Error(getRow(), getCol(), 101, token);    // user entered a not existing operator like "//"
            else
                throw Error(getRow(), getCol(), 5, token);
        }
        snprintf(ans_str, sizeof(ans_str), "Ans = %g", ans);
    }
    catch (Error err){
        if (err.get_row() == -1)
            snprintf(ans_str, sizeof(ans_str), "Error: %s (col %i)", err.get_msg(), err.get_col());
        else
            snprintf(ans_str, sizeof(ans_str), "Error: %s (ln %i, col %i)", err.get_msg(), err.get_row(), err.get_col());
    }

    return ans_str;
}


/*
 * checks if the given char c is a delimeter
 * minus is checked apart, can be unary minus
 */
bool isDelimeter(const char c)
{
    if (c == 0) return 0;
    return strchr("&|<>=+/*%^!", c) != 0;
}

/*
 * checks if the given char c is a letter or underscore
 */
bool isAlpha(const char c)
{
    if (c == 0) return 0;
    return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ_", toupper(c)) != 0;
}

/*
 * checks if the given char c is a digit or dot
 */
bool isDigitDot(const char c)
{
    if (c == 0) return 0;
    return strchr("0123456789.", c) != 0;
}

/*
 * checks if the given char c is a digit
 */
bool isDigit(const char c)
{
    if (c == 0) return 0;
    return strchr("0123456789", c) != 0;
}


/**
 * Get next token in the current string expr.
 * Uses the Parser data expr, e, token, t, token_type and err
 */
void Parser::tokenize()
{
    token_type = NOTHING;
    char* tok;
    tok = token;         // let tok point to the first character in token
    *tok = '\0';         // set token empty

    while (*e == ' ' || *e == '\t')     // skip space or tab
        e++;

    if (*e == '\0'){    // end of expression
        token_type = DELIMETER;
        return;
    }

    if (*e == '-'){     // check for minus
        token_type = DELIMETER;
        *tok = *e;
        e++;
        tok++;
        *tok = '\0';
        return;
    }

    if (*e == '(' || *e == ')'){    // check for parentheses
        token_type = DELIMETER;
        *tok = *e;
        e++;
        tok++;
        *tok = '\0';
        return;
    }

    if (isDelimeter(*e)){   // check for operators / delimeters
        token_type = DELIMETER;
        while (isDelimeter(*e)){
            *tok = *e;
            e++;
            tok++;
        }
        *tok = '\0';  // add a null character at the end of token
        return;
    }


    if (isDigitDot(*e)){    // check for a value
        token_type = NUMBER;
        while (isDigitDot(*e)){
            *tok = *e;
            e++;
            tok++;
        }

        *tok = '\0';
        return;
    }

    if (isAlpha(*e)){   // check for variables or functions
        while (isAlpha(*e) || isDigit(*e)){
            *tok = *e;
            e++;
            tok++;
        }
        *tok = '\0';

        // check if this is a variable or a function.
        // a function has a parenthesis '(' open after the name
        char* e2 = NULL;
        e2 = e;

        while (*e2 == ' ' || *e2 == '\t')     // skip space or tab
            e2++;

        if (*e2 == '(')
            token_type = FUNCTION;
        else
            token_type = VARIABLE;

        return;
    }

    // syntax error
    token_type = UNKNOWN;
    while (*e != '\0'){
        *tok = *e;
        e++;
        tok++;
    }
    *tok = '\0';
    throw Error(getRow(), getCol(), 1, token);

    return;
}

/// assignment of variable or function
double Parser::parseAssign()
{
    //printf("Test 1\n");
    if (token_type == VARIABLE){
        char* eNew = e;
        TOKENTYPE tokenTypeNew = token_type;
        char tokenNew[NAME_LEN_MAX+1];
        strcpy(tokenNew, token);

        tokenize();
        if (strcmp(token, "=") == 0){  // assignment
            tokenize();
            double ans = parseBitshift();
            if (user_var.add(tokenNew, ans) == false){
                throw Error(getRow(), getCol(), 300);
            }
            return ans;
        }
        else{ // go back to previous token
            e = eNew;
            token_type = tokenTypeNew;
            strcpy(token, tokenNew);
        }
    }
    return parseBitshift();
}

/// conditional operators and bitshift
double Parser::parseBitshift()
{
    //printf("Test 2\n");
    double ans = parseCond();
    int Op = getOpID(token);
    while (Op == AND || Op == OR || Op == BITSHIFTLEFT || Op == BITSHIFTRIGHT){
        tokenize();
        ans = evalOp(Op, ans, parseCond());
        Op = getOpID(token);
    }
    return ans;
}

/// conditional operators
double Parser::parseCond()
{
    //printf("Test 3\n");
    double ans = parseOp1();
    int Op = getOpID(token);
    while (Op == EQUAL || Op == UNEQUAL || Op == SMALLER || Op == LARGER || Op == SMALLEREQ || Op == LARGEREQ){
        tokenize();
        ans = evalOp(Op, ans, parseOp1());
        Op = getOpID(token);
    }
    return ans;
}

/// add or subtract
double Parser::parseOp1()
{
    //printf("Test 4\n");
    double ans = parseOp2();
    int Op = getOpID(token);
    while (Op == PLUS || Op == MINUS){
        tokenize();
        ans = evalOp(Op, ans, parseOp2());
        Op = getOpID(token);
    }
    return ans;
}

/// multiply, divide, modulus, xor
double Parser::parseOp2()
{
    //printf("Test 5\n");
    double ans = parsePower();
    int Op = getOpID(token);
    while (Op == MULTIPLY || Op == DIVIDE || Op == MODULUS || Op == XOR){
        tokenize();
        ans = evalOp(Op, ans, parsePower());
        Op = getOpID(token);
    }
    return ans;
}

double Parser::parsePower()
{
    //printf("Test 6\n");
    double ans = parseFact();
    int Op = getOpID(token);
    while (Op == POW){
        tokenize();
        ans = evalOp(Op, ans, parseFact());
        Op = getOpID(token);
    }
    return ans;
}

double Parser::parseFact()
{
    //printf("Test 7\n");
    double ans = parseNeg();
    int Op = getOpID(token);
    while (Op == FACTORIAL){
        tokenize();
        // factorial does not need a value right from the
        // operator, so zero is filled in.
        ans = evalOp(Op, ans, 0.0);
        Op = getOpID(token);
    }
    return ans;
}

/// unary minus
double Parser::parseNeg()
{
    //printf("Test 8\n");
    int Op = getOpID(token);
    if (Op == MINUS){
        tokenize();
        double ans = parseFunc();
        return -ans;
    }
    else{
        return parseFunc();
    }
}

/// functions
double Parser::parseFunc()
{
    //printf("Test 9\n");
        if (token_type == FUNCTION){
        char fn_name[NAME_LEN_MAX+1];
        strcpy(fn_name, token);
        tokenize();
        return evalFunc(fn_name, parseParen());
    }
    else{
        return parseParen();
    }
}

/// parenthesized expression or value
double Parser::parseParen()
{
    //printf("Test 10\n");
    // check if it is a parenthesized expression
    if (token_type == DELIMETER){
        if (token[0] == '(' && token[1] == '\0'){
            tokenize();
            double ans = parseBitshift();
            if (token_type != DELIMETER || token[0] != ')' || token[1] || '\0'){
                throw Error(getRow(), getCol(), 3);
            }
            tokenize();
            return ans;
        }
    }
    // if not parenthesized then the expression is a value
    return parseNumber();
}

double Parser::parseNumber()
{
    //printf("Test 11\n");
    double ans = 0;
    if (token_type == NUMBER){
        ans = strtod(token, NULL);
        tokenize();
    }
    else if (token_type == VARIABLE){
        ans = evalVar(token);
        tokenize();
    }
    else{
        if (token[0] == '\0')
                throw Error(getRow(), getCol(), 6);
            else
                throw Error(getRow(), getCol(), 7);
    }
    return ans;
}

int Parser::getOpID(const char op_name[])
{
    if (!strcmp(op_name,"&"))   return AND;
    if (!strcmp(op_name,"|"))   return OR;
    if (!strcmp(op_name,"<<"))  return BITSHIFTLEFT;
    if (!strcmp(op_name,">>"))  return BITSHIFTRIGHT;
    if (!strcmp(op_name,"=="))  return EQUAL;
    if (!strcmp(op_name,"<>"))  return UNEQUAL;
    if (!strcmp(op_name,"<"))   return SMALLER;
    if (!strcmp(op_name,">"))   return LARGER;
    if (!strcmp(op_name,"<="))  return SMALLEREQ;
    if (!strcmp(op_name,">="))  return LARGEREQ;
    if (!strcmp(op_name,"+"))   return PLUS;
    if (!strcmp(op_name,"-"))   return MINUS;
    if (!strcmp(op_name,"*"))   return MULTIPLY;
    if (!strcmp(op_name,"/"))   return DIVIDE;
    if (!strcmp(op_name,"%"))   return MODULUS;
    if (!strcmp(op_name,"||"))  return XOR;
    if (!strcmp(op_name,"^"))   return POW;
    if (!strcmp(op_name,"!"))   return FACTORIAL;

    return -1;
}

double Parser::evalOp(const int op_id, const double &lhs, const double &rhs)
{
    switch (op_id){
        case AND:           return int(lhs) & int(rhs);
        case OR:            return int(lhs) | int(rhs);
        case BITSHIFTLEFT:  return int(lhs) << int(rhs);
        case BITSHIFTRIGHT: return int(lhs) >> int(rhs);
        case EQUAL:         return lhs == rhs;
        case UNEQUAL:       return lhs != rhs;
        case SMALLER:       return lhs < rhs;
        case LARGER:        return lhs > rhs;
        case SMALLEREQ:     return lhs <= rhs;
        case LARGEREQ:      return lhs >= rhs;
        case PLUS:          return lhs + rhs;
        case MINUS:         return lhs - rhs;
        case MULTIPLY:      return lhs * rhs;
        case DIVIDE:        return lhs / rhs;
        case MODULUS:       return int(lhs) % int(rhs);
        case XOR:           return int(lhs) ^ int(rhs);
        case POW:           return pow(lhs, rhs);
        case FACTORIAL:     return fact(lhs);
    }

    throw Error(getRow(), getCol(), 104, op_id);
    return 0;
}

double Parser::evalFunc(const char fn_name[], const double &value)
{
    try{
        char funcNameL[NAME_LEN_MAX+1];
        strtolower(funcNameL, fn_name);

        if (!strcmp(funcNameL,"abs"))        return abs(value);
        if (!strcmp(funcNameL,"exp"))        return exp(value);
        if (!strcmp(funcNameL,"sign"))       return sign(value);
        if (!strcmp(funcNameL,"sqrt"))       return sqrt(value);
        if (!strcmp(funcNameL,"log"))        return log(value);
        if (!strcmp(funcNameL,"log10"))      return log10(value);
        if (!strcmp(funcNameL,"sin"))        return sin(value);
        if (!strcmp(funcNameL,"cos"))        return cos(value);
        if (!strcmp(funcNameL,"tan"))        return tan(value);
        if (!strcmp(funcNameL,"asin"))       return asin(value);
        if (!strcmp(funcNameL,"acos"))       return acos(value);
        if (!strcmp(funcNameL,"atan"))       return atan(value);
        if (!strcmp(funcNameL,"factorial"))  return fact(value);
    }
    catch (Error err){
        // retrow error, add information about column and row of occurrence
        // TODO: doesnt work yet
        throw Error(getCol(), getRow(), err.get_id(), err.get_msg());
    }

    // unknown function
    throw Error(getRow(), getCol(), 102, fn_name);
    return 0;
}

double Parser::evalVar(const char var_name[])
{
    char varNameL[NAME_LEN_MAX+1];
    strtolower(varNameL, var_name);

    if (!strcmp(varNameL,"e"))  return 2.7182818284590452353602874713527;
    if (!strcmp(varNameL,"pi")) return 3.1415926535897932384626433832795;

    double ans;
    if (user_var.getValue(var_name, &ans))
        return ans;

    throw Error(getRow(), getCol(), 103, var_name);
    return 0;
}

/// returns the line of the currently handled expression
int Parser::getRow()
{
    return -1;
}

/// returns the column (position) where the last token starts
int Parser::getCol()
{
    return e-expr-strlen(token)+1;
}
