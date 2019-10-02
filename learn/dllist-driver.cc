
#include "dllist.h"

void InsertList(int N, DLList *list)
{
    for (int i = 0; i < N; i++)
    {
        void *item;
        int key=Random()%100;
        //int key=i;
        int *items=new int;
        *items=-key;
        item=items;
        printf("%s in:%d %d\n",currentThread->getName(),key,*item);
        //currentThread->Yield();
        list->SortedInsert(item,key);
        //currentThread->Yield();
    }
    list->show();
}

void RemoveList(int N, DLList *list)
{
    for (int i = 0; i < N; i++)
    {
        int key;
        void *item;
        item=list->Remove(&key);
        printf("%s out:%d %d\n",currentThread->getName(),key,*(int*)item);
        delete item;
        //currentThread->Yield();
    }
    list->show();
}

