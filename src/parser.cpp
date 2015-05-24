#include "parser.h"

using namespace std;

bool flag = false;
bool closedParen = false;

Parser::Parser()
{
    expr[0] = '\0';
    token[0] = '\0';
    tokenNew[0] = '\0';
    e = NULL;
    tokenType = NOTHING;
    tokenTypeNew = NOTHING;
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
            //else
                //throw Error(getRow(), getCol(), 5, token);
        }
        snprintf(answer_str, sizeof(answer_str), "Ans = %g", answer);
    }
    catch (Error err){
        if (err.get_row() == -1)
            snprintf(answer_str, sizeof(answer_str), "Error: %s (col %d)", err.get_msg(), err.get_col());
        else
            snprintf(answer_str, sizeof(answer_str), "Error: %s (ln %d, col %d)", err.get_msg(), err.get_row(), err.get_col());
    }
    return answer_str;
}

bool isDelimeter(const char c)
{
    if (!c) return 0;
    return strchr("&|<>=+/*%^!", c) != 0;
}

bool isAlpha(const char c)
{
    if (!c) return 0;
    return strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZ_", toupper(c)) != 0;
}

bool isDigitDot(const char c)
{
    if (!c) return 0;
    return strchr("0123456789.", c) != 0;
}

bool isDigit(const char c)
{
    if (!c) return 0;
    return strchr("0123456789", c) != 0;
}

bool isParam(const char c)
{
    if (!c) return 0;
    return isAlpha(c) || c == ',' || isDigitDot(c);
}


bool isPreDefinedFunction(const char fn_name[]);

/**
 * Get next token in the current string expr.
 * Uses the Parser data expr, e, token, t, tokenType and err
 */
void Parser::tokenize()
{
    strncpy(tokenNew, token, NAME_LEN_MAX-1);
    tokenTypeNew = tokenType;
    tokenType = NOTHING;
    char* tok;
    tok = token;         // let tok point to the first character in token
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

        if (*e2 == '('){
            tokenType = FUNCTION;
            e2++;
            if(isParam(*e2) && flag){
                closedParen = false;
                *tok = '(';tok++;
                while((isParam(*e2) || *e2=='(' || *e2==')' || isDelimeter(*e2)) && flag){
                    *tok = *e2;
                    tok++; e2++;
                    while(*e2 == ' ' || *e2 == '\t')
                        e2++;
                    if (isDigit(*e2) && *(e2+1) ==')') {
                        *tok=*e2;
                        tok++;e2++;
                        *tok=*e2;
                        tok++;*tok='\0';
                        e=e2;
                        if (*e2 != ')') throw Error(getRow(), getCol(), 3);
                        return;
                    }
                    if (*e2 == ')') {closedParen = true; e=e2;}
                }
                *tok = '\0';
                if (!closedParen) throw Error(getRow(), getCol(), 3);
                e=e2;
            }
        }
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
    //printf("Level 1\n");
    if (tokenType == VARIABLE){
        char *eVar = e;
        TOKENTYPE tokenTypeVar = tokenType;
        char tokenVar[NAME_LEN_MAX+1];
        strcpy(tokenVar, token);
        tokenize();
        if (!strcmp(token, "=")){  // assignment
            tokenize();
            double ans = parseBitshift();
            if (!user_var.add(tokenVar, ans)){
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
    //printf("Returning Level 1\n");
    return parseBitshift();
}

/// bitwise operators and bitshift
double Parser::parseBitshift()
{
    //printf("Level 2\n");
    double ans = parseCond();
    int Op = getOpID(token);
    while (Op == AND || Op == OR || Op == BITSHIFTLEFT || Op == BITSHIFTRIGHT){
        tokenize();
        ans = evalOp(Op, ans, parseCond());
        Op = getOpID(token);
    }
    //printf("Returning Level 2\n");
    return ans;
}

/// conditional operators
double Parser::parseCond()
{
    //printf("Level 3\n");
    double ans = parseOp1();
    int Op = getOpID(token);
    while (Op == EQUAL || Op == UNEQUAL || Op == SMALLER || Op == LARGER || Op == SMALLEREQ || Op == LARGEREQ){
        tokenize();
        ans = evalOp(Op, ans, parseOp1());
        Op = getOpID(token);
    }
    //printf("Returning Level 3\n");
    return ans;
}

/// add or subtract
double Parser::parseOp1()
{
    //printf("Level 4\n");
    double ans = parseOp2();
    int Op = getOpID(token);
    while (Op == PLUS || Op == MINUS){
        tokenize();
        ans = evalOp(Op, ans, parseOp2());
        Op = getOpID(token);
    }
    //printf("Returning Level 4\n");
    return ans;
}

/// multiply, divide, modulus, xor
double Parser::parseOp2()
{
    //printf("Level 5\n");
    double ans = parsePower();
    int Op = getOpID(token);
    while (Op == MULTIPLY || Op == DIVIDE || Op == MODULUS || Op == XOR){
        tokenize();
        ans = evalOp(Op, ans, parsePower());
        Op = getOpID(token);
    }
    //printf("Returning Level 5\n");
    return ans;
}

double Parser::parsePower()
{
    //printf("Level 6\n");
    double ans = parseFact();
    int Op = getOpID(token);
    while (Op == POW){
        tokenize();
        ans = evalOp(Op, ans, parseFact());
        Op = getOpID(token);
    }
    //printf("Returning Level 6\n");
    return ans;
}

double Parser::parseFact()
{
    //printf("Level 7\n");
    double ans = parseNeg();
    int Op = getOpID(token);
    while (Op == FACTORIAL){
        tokenize();
        ans = evalOp(Op, ans, 0.0); // RHS = 0
        Op = getOpID(token);
    }
    //printf("Returning Level 7\n");
    return ans;
}

/// unary minus
double Parser::parseNeg()
{
    //printf("Level 8\n");
    double ans;
    int Op = getOpID(token);
    if (Op == MINUS){
        tokenize();
        ans = parseFunc();
        ans = -ans;
    }
    else{
        ans = parseFunc();
    }
    //printf("Returning Level 8\n");
    return ans;
}

double Parser::parseFunc()
{
    //printf("Level 9\n");
    double ans = 0;
    //flag = false;
    if (tokenType == FUNCTION){
        char fName[NAME_LEN_MAX+1];
        char * parts = strtok(token,"(,)");
        strcpy(fName,parts);
        while(*e != '(')
                e--;
        if(isPreDefinedFunction(fName)){
            flag = false;
            //printf("Predefined Function, %s\n",fName);
            tokenize();
            if (!strcmp(fName,"print")){
                tokenize();
                char str[EXPR_LEN_MAX+1];
                strcpy(str,token);
                char* evaluated;
                evaluated = parse(str);
                if(evaluated[0] == 'A'){
                    char* str = parse(evaluated);
                    str = strtok(str,"="); str = strtok(NULL,"=");
                    ans = strtod(str,NULL);
                    //printf("\tAns = %g\n",ans);
                }
                else{
                    printf("%s\n",str);
                }

                return ans;
            }


            if (!strcmp(fName,"if")){
                char condition[EXPR_LEN_MAX+1];
                condition[0] = '\0';
                for(int i = 0; i < 3; i++){
                    tokenize();
                    strcat(condition,token);
                }
                if(condition[strlen(condition)-1] == '-'){
                    tokenize();
                    strcat(condition,token);
                }
                char* str = parse(condition); if(str[0] == 'E') throw Error(getRow(), getCol()+3, 104, condition);
                str = strtok(str,"="); str = strtok(NULL,"=");
                ans = strtod(str,NULL);

                char expression[EXPR_LEN_MAX+1];
                char else_expr[EXPR_LEN_MAX+1];

                bool broke = false;

                do{
                    printf("   ");
                    gets(expression);
                    if(!strcmp(expression,"endif")) break;
                    if(ans && strcmp(expression,"else")) parse(expression);
                    if(!strcmp(expression,"else")){
                        do{
                            printf("   ");
                            gets(else_expr);
                            if(!strcmp(else_expr,"endif")) {broke = true; break;}
                            if(!ans) parse(else_expr);
                        } while (true);
                    }
                    if(broke) break;
                } while (true);

                broke = false;

                return ans;
            }
            /// if predefined but not 'if'
            ans = evalFunc(fName, parseIncrement());
        }
        else{ /// if not predefined
            flag = true;
            for(unsigned i = 0; i < strlen(token); i++)
                e--;
            tokenize();
            //printf("Userdefined Function, %s\n",token);
            char funcName[NAME_LEN_MAX+1];
            int countParams = -1;
            char * params[NAME_LEN_MAX+1];
            char * funcExpr = NULL;
            char * parts = strtok(token,"(,)");
            strcpy(funcName,parts);
            while(parts){
                parts = strtok(NULL,"(,)");
                params[++countParams] = parts;
            }

            if(params[countParams-1][0] == '='){
                parts = strtok(params[countParams-1],"=");
                funcExpr = parts;
                countParams--;
            }

            flag = false;
            if (user_func.exist(funcName)){
                char evaluated[EXPR_LEN_MAX+1];
                if (user_func.evalExpr(funcName, params, evaluated)){
                    char * str = strpbrk(parse(evaluated),"0123456789"); /// finds numbers in string
                    ans = strtod(str,NULL);
                }
            }
            else{
                if (!funcExpr) throw Error(getRow(), getCol(), 102, funcName);
                if (!user_func.add(funcName,params,countParams,funcExpr))
                    throw Error(getRow(), getCol(), 301);
                ans = 1;
            }

            tokenize();
        }
    }
    else{
        ans = parseIncrement();
    }
    //printf("Returning Level 9\n");
    return ans;
}

double Parser::parseIncrement()
{
    //printf("Level 10\n");
    double ans = parseParen();
    int Op = getOpID(token);
    if (Op == PLUSPLUS){
        char varName[NAME_LEN_MAX+1];
        strncpy(varName, tokenNew, NAME_LEN_MAX-1);
        TOKENTYPE varTokenType = tokenTypeNew;
        tokenize();
        if (user_var.getValue(varName, &ans) && varTokenType != NUMBER)
            ans = evalOp(Op, ans, 0.0);
        else
            ans = evalOp(Op, strtod(varName,NULL), 0.0);
        if (user_var.add(varName, ans) && varTokenType != NUMBER)
            return ans;
    }
    //printf("Returning Level 10\n");
    return ans;
}

/// parenthesized expression or value
double Parser::parseParen()
{
    //printf("Level 11\n");
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
    //printf("Returning Level 11\n");
    return parseNumber();   // if not parenthesized then the expression is a value
}

double Parser::parseNumber()
{
    //printf("Level 12\n");
    double ans = 0;
    if (tokenType == NUMBER){
        ans = strtod(token, NULL);
        tokenize();
    }
    else if (tokenType == VARIABLE){
        ans = evalVar(token);
        //printf("var is %s = %g\n",token,ans);
        tokenize();
    }
    else if (getOpID(token) == MINUS){
        int c = 1;
        while (getOpID(token) == MINUS){
            tokenize();
            c++;
        }
        if (tokenType == NUMBER && !(c%2))
            ans = -strtod(token, NULL);
        else if (tokenType == NUMBER && c%2)
            ans = strtod(token, NULL);
        tokenize();
    }
    else{
        if (token[0] == '\0')
                throw Error(getRow(), getCol(), 6);
            else
                throw Error(getRow(), getCol(), 7);
    }
    //printf("Returning Level 12\n");
    return ans;
}

int Parser::getOpID(const char opName[])
{
    if (!strcmp(opName,"&"))   return AND;
    if (!strcmp(opName,"|"))   return OR;
    if (!strcmp(opName,"<<"))  return BITSHIFTLEFT;
    if (!strcmp(opName,">>"))  return BITSHIFTRIGHT;
    if (!strcmp(opName,"=="))  return EQUAL;
    if (!strcmp(opName,"!="))  return UNEQUAL;
    if (!strcmp(opName,"<"))   return SMALLER;
    if (!strcmp(opName,">"))   return LARGER;
    if (!strcmp(opName,"<="))  return SMALLEREQ;
    if (!strcmp(opName,">="))  return LARGEREQ;
    if (!strcmp(opName,"+"))   return PLUS;
    if (!strcmp(opName,"++"))  return PLUSPLUS;
    if (!strcmp(opName,"-"))   return MINUS;
    if (!strcmp(opName,"--"))  return MINUSMINUS;
    if (!strcmp(opName,"*"))   return MULTIPLY;
    if (!strcmp(opName,"/"))   return DIVIDE;
    if (!strcmp(opName,"%"))   return MODULUS;
    if (!strcmp(opName,"||"))  return XOR;
    if (!strcmp(opName,"^"))   return POW;
    if (!strcmp(opName,"!"))   return FACTORIAL;

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
        case MINUSMINUS:    return lhs - 1;
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

bool isPreDefinedFunction(const char fn_name[])
{
    char funcNameL[NAME_LEN_MAX+1];
    strtolower(funcNameL, fn_name);
    return (!strcmp(funcNameL,"abs"))
        || (!strcmp(funcNameL,"exp"))
        || (!strcmp(funcNameL,"sign"))
        || (!strcmp(funcNameL,"sqrt"))
        || (!strcmp(funcNameL,"log"))
        || (!strcmp(funcNameL,"log10"))
        || (!strcmp(funcNameL,"sin"))
        || (!strcmp(funcNameL,"cos"))
        || (!strcmp(funcNameL,"tan"))
        || (!strcmp(funcNameL,"asin"))
        || (!strcmp(funcNameL,"acos"))
        || (!strcmp(funcNameL,"atan"))
        || (!strcmp(funcNameL,"factorial"))
        || (!strcmp(funcNameL,"if"))
        || (!strcmp(funcNameL,"else"))
        || (!strcmp(funcNameL,"print"));
}

double Parser::evalFunc(const char fn_name[], const double &value)
{
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
