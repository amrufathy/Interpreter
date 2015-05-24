#ifndef HASH_H
#define	HASH_H

#include <iostream>
#include <cstdio>
#include <string>

using namespace std;

class hash
{
public:
    hash();
    int Hash(string key);
    void additem(string name, double value);
    bool add(const char* name, double value);
    int countitems(int index);
    void printtable();
    void printitem(int index);
    bool getValue(const char* name, double* value);
    bool getIndex(const char* name);
private:
    static const int tablesize = 1000;

    struct VAR
    {
        string name;
        double value;
        VAR* next;
    };
    VAR* hashtable[tablesize];
};




#endif	/* HASH_H */

