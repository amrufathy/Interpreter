#include "parser.h"

using namespace std;

Parser::Parser()
{
    expr[0] = '\0';
    token[0] = '\0';
    tokenNew[0] = '\0';
    e = NULL;
    tokenType = NOTHING;
}

char* Parser::parse(const char newExpr[])
{
    try {
        if ((int)strlen(newExpr) > EXPR_LEN_MAX)
            throw Error(getRow(), getCol(), 200);
        strncpy(expr, newExpr, EXPR_LEN_MAX - 1);
        e = expr;
        answer = 0;
        tokenize();
        if (tokenType == DELIMETER && *token == '\0')
            throw Error(getRow(), getCol(), 4);
        answer = parseAssign();
        // check for garbage at the end of the expression
        // an expression ends with a character '\0' and tokenType = delimeter
        if (tokenType != DELIMETER || *token != '\0'){
            if (tokenType == DELIMETER)
                throw Error(getRow(), getCol(), 101, token);    // user entered a not existing operator like "//"
            else
                throw Error(getRow(), getCol(), 5, token);
        }
        snprintf(answer_str, sizeof(answer_str), "Ans = %g", answer);
    }
    catch (Error err){
        if (err.get_row() == -1)
            snprintf(answer_str, sizeof(answer_str), "Error: %s (col %i)", err.get_msg(), err.get_col());
        else
            snprintf(answer_str, sizeof(answer_str), "Error: %s (ln %i, col %i)", err.get_msg(), err.get_row(), err.get_col());
    }
    return answer_str;
}

bool isDelimeter(const char c)
{
    if (c == 0) return 0;
    return strchr("&|<>=+/*%^!", c) != 0;
}

bool isAlpha(const char c)
{
    if (c == 0) return 0;
    return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ_", toupper(c)) != 0;
}

bool isDigitDot(const char c)
{
    if (c == 0) return 0;
    return strchr("0123456789.", c) != 0;
}

bool isDigit(const char c)
{
    if (c == 0) return 0;
    return strchr("0123456789", c) != 0;
}

/**
 * Get next token in the current string expr.
 * Uses the Parser data expr, e, token, t, tokenType and err
 */
void Parser::tokenize()
{
    tokenType = NOTHING;
    char* tok;
    tok = token;         // let tok point to the first character in token

    strncpy(tokenNew, token, NAME_LEN_MAX-1);

    *tok = '\0';

    while (*e == ' ' || *e == '\t')     // skip space or tab
        e++;

    if (*e == '\0'){    // end of expression
        tokenType = DELIMETER;
        return;
    }

    if (*e == '-'){     // check for minus
        tokenType = DELIMETER;
        *tok = *e;
        e++; tok++;
        *tok = '\0';
        return;
    }

    if (*e == '(' || *e == ')'){    // check for parentheses
        tokenType = DELIMETER;
        *tok = *e;
        e++; tok++;
        *tok = '\0';
        return;
    }

    if (isDelimeter(*e)){   // check for operators / delimeters
        tokenType = DELIMETER;
        while (isDelimeter(*e)){
            *tok = *e;
            e++; tok++;
        }
        *tok = '\0';  // add a null character at the end of token
        return;
    }

    if (isDigitDot(*e)){    // check for a value
        tokenType = NUMBER;
        while (isDigitDot(*e)){
            *tok = *e;
            e++; tok++;
        }
        *tok = '\0';
        return;
    }

    if (isAlpha(*e)){   // check for variables or functions
        while (isAlpha(*e) || isDigit(*e)){
            *tok = *e;
            e++; tok++;
        }
        *tok = '\0';
        // check if this is a variable or a function.
        // a function has a parenthesis '(' open after the name
        char* e2 = NULL;
        e2 = e;
        while (*e2 == ' ' || *e2 == '\t')     // skip space or tab
            e2++;

        if (*e2 == '(')
            tokenType = FUNCTION;
        else
            tokenType = VARIABLE;

        return;
    }
    // syntax error
    tokenType = UNKNOWN;
    while (*e != '\0'){
        *tok = *e;
        e++; tok++;
    }
    *tok = '\0';
    throw Error(getRow(), getCol(), 1, token);

    return;
}

/// assignment of variable or function
double Parser::parseAssign()
{
    //printf("Test 1\n");
    if (tokenType == VARIABLE){
        char *eVar = e;
        TOKENTYPE tokenTypeVar = tokenType;
        char tokenVar[NAME_LEN_MAX+1];
        strcpy(tokenVar, token);
        tokenize();
        if (!strcmp(token, "=")){  // assignment
            tokenize();
            double ans = parseBitshift();
            if (user_var.add(tokenVar, ans) == false){
                throw Error(getRow(), getCol(), 300);
            }
            return ans;
        }
        else{ // go back to previous token
            e = eVar;
            tokenType = tokenTypeVar;
            strcpy(token, tokenVar);
        }
    }
    return parseBitshift();
}

/// bitwise operators and bitshift
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
        ans = evalOp(Op, ans, 0.0); // RHS = 0
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

double Parser::parseFunc()
{
    //printf("Test 9\n");
    if (tokenType == FUNCTION){
        char fName[NAME_LEN_MAX+1];
        strcpy(fName, token);
        tokenize();
        return evalFunc(fName, parseIncrement());
    }
    else{
        return parseIncrement();
    }
}

double Parser::parseIncrement()
{
    //printf("Test 10\n");
    double ans = parseParen();
    int Op = getOpID(token);
    if (Op == PLUSPLUS){
        char varName[NAME_LEN_MAX+1];
        strncpy(varName, tokenNew, NAME_LEN_MAX-1);
        tokenize();
        if (user_var.getValue(varName, &ans))
            ans = evalOp(Op, ans, 0.0);
        if (user_var.add(varName, ans))
            return ans;
    }
    else return ans;

}

/// parenthesized expression or value
double Parser::parseParen()
{
    //printf("Test 11\n");
    if (tokenType == DELIMETER){   // parenthesized expression
        if (token[0] == '(' && token[1] == '\0'){
            tokenize();
            double ans = parseBitshift();
            if (tokenType != DELIMETER || token[0] != ')' || token[1] || '\0')
                throw Error(getRow(), getCol(), 3);
            tokenize();
            return ans;
        }
    }
    return parseNumber();   // if not parenthesized then the expression is a value
}

double Parser::parseNumber()
{
    //printf("Test 12\n");
    double ans = 0;
    if (tokenType == NUMBER){
        ans = strtod(token, NULL);
        tokenize();
    }
    else if (tokenType == VARIABLE){
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
    if (!strcmp(op_name,"++"))  return PLUSPLUS;
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
        case PLUSPLUS:      return lhs + 1;
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
        if (!strcmp(funcNameL,"abs"))       return abs(value);
        if (!strcmp(funcNameL,"exp"))       return exp(value);
        if (!strcmp(funcNameL,"sign"))      return sign(value);
        if (!strcmp(funcNameL,"sqrt"))      return sqrt(value);
        if (!strcmp(funcNameL,"log"))       return log(value);
        if (!strcmp(funcNameL,"log10"))     return log10(value);
        if (!strcmp(funcNameL,"sin"))       return sin(value);
        if (!strcmp(funcNameL,"cos"))       return cos(value);
        if (!strcmp(funcNameL,"tan"))       return tan(value);
        if (!strcmp(funcNameL,"asin"))      return asin(value);
        if (!strcmp(funcNameL,"acos"))      return acos(value);
        if (!strcmp(funcNameL,"atan"))      return atan(value);
        if (!strcmp(funcNameL,"factorial")) return fact(value);
    }
    catch (Error err){
        throw Error(getCol(), getRow(), 102, fn_name);
    }
    // unknown function
    throw Error(getRow(), getCol(), 102, fn_name);
    return 0;
}

double Parser::evalVar(const char var_name[])
{
    char varNameL[NAME_LEN_MAX+1];
    strtolower(varNameL, var_name);
    if (!strcmp(varNameL,"exit")) {exit(0); return 0;}
    if (!strcmp(varNameL,"e"))  return 2.7182818284590452353602874713527;
    if (!strcmp(varNameL,"pi")) return 3.1415926535897932384626433832795;
    if (!strcmp(varNameL,"bibo")) return 98105098111013010;
    if (!strcmp(varNameL,"mode")) {modes.moder(); return 0;}
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
