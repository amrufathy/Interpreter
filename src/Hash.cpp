#include "Hash.h"

hash::hash()  /**Constructor*/
{

    for(int i=0; i<tablesize; i++)
    {
        hashtable[i]=new VAR;
        hashtable[i]->name="empty";
        hashtable[i]->value=0;
        hashtable[i]->next=NULL;
    }
}

/**must be changed*/
int hash::Hash(string key)
{
    int hash=0;
    int index;
    for (int i = 0; i <key.length(); i++)
    {
        hash=hash+(int)key[i];
    }
    index=hash%tablesize;


    return index;
}
/// Add a name and value to the table
bool hash::add(const char* name, double value)
{
    int index =Hash(name);
    bool sucess;
    if(hashtable[index]->name=="empty")
    {
        hashtable[index]->name=name;
        hashtable[index]->value=value;
        sucess=true;
    }
    else
    {

        VAR *ptr = hashtable[index];
        VAR *n=new VAR;
        n->name=name;
        n->value=value;
        n->next=NULL;
        while(ptr->next!=NULL)
        {
            ptr=ptr->next;
        }
        ptr->next = n;
        sucess=true;
        }
        return sucess;
}
bool hash::getIndex(const char* name)
{

    int index=Hash(name);
    bool foundname=false;
    VAR *ptr=hashtable[index];
    while(ptr)
    {
        if(ptr->name==name)
        {
            foundname=true;
        }
        ptr=ptr->next;
    }
    if(foundname==true)
    {
        return index;
    }
    else
    {
        return -1;
    }

}

bool hash::getValue(const char* name, double* value)
{
    int index=Hash(name);
    bool foundname=false;
    VAR *ptr=hashtable[index];
    while(ptr)
    {
        if(ptr->name==name)
        {
            foundname=true;
            *value=ptr->value;
        }
        ptr=ptr->next;
    }
    return foundname;
}

int hash::countitems(int index)
{

    int c=0;
    if(hashtable[index]->name=="empty")
    {
        return c;
    }
    else
    {
        c++;
        VAR *ptr=hashtable[index];
        while(ptr->next!=NULL)
        {
            printf("%d\n",c);
            c++;
            ptr=ptr->next;
        }
    }
    return c;
}
void hash::printitem(int index)
{
    VAR *ptr=hashtable[index];
    if(ptr->name=="empty")
    {
        cout<<"index= "<<index<<" is empty \n";
    }
    else
    {
        cout<<"index"<<index<<"contains these items\n";
        while(ptr->next!=NULL)
        {
            cout<<"--------------------------------\n";
            cout<<ptr->name<<endl;
            cout<<ptr->value<<endl;
            cout<<"--------------------------------\n";
            ptr=ptr->next;

        }
    }
}
