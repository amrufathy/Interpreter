#include "myfunctions.h"

using namespace std;

double fact(double value)
{
    double res = 1;
    int newVal = int(value);

    if (value != double(newVal)) // check if casting went right
        throw Error(-1, -1, 400, "factorial");

    for(int i = 1; i <= newVal; i++){
        res *= i;
    }

    return res;
}

double sign(double value)
{
    if (value == 0) return 0;
    return value > 0 ? 1 : -1;
}

/**
 * str is copied to lower and made lowercase
 * upper is the returned string
 * str should be null-terminated
 */
void strtolower(char* lower, const char* str)
{
    int i = -1;
    do{
        i++;
        lower[i] = tolower(str[i]);
    }
    while (str[i] != '\0');
}
