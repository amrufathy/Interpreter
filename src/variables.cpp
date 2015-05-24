#include "variables.h"

/// Add a name and value to the list
bool Variables::add(const char* name, double value)
{
    /*VAR newVar;
    strncpy(newVar.name, name, 30);
    newVar.value = value;

    int index = getIndex(name);
    if (index == -1)
        variables.push_back(newVar); // variable does not exist
    else
        variables[index] = newVar;  // variable exists*/


    return HASH.add(name,value);
}

/// Get value of variable with known name
bool Variables::getValue(const char* name, double* value)
{/*
    int index = getIndex(name);
    if (index != -1){
        *value = variables[index].value;
        return true;
    }*/
    return HASH.getValue(name,value);
}

/**
 * Returns the index of the given name in the variable list.
 * Returns -1 if name is not present in the list.
 * Name is case sensitive.
 */
int Variables::getIndex(const char* name)
{/*
    for (unsigned i = 0; i < variables.size(); i++){
        if(!strcmp(name, variables[i].name))
            return i;
    }*/
    return -1;
}
