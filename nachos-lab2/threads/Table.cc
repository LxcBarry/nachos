#include "Table.h"

// create a table to hold at most 'size' entries.
Table::Table(int size)
{
    table = new void*[size];
    tableLock = new Lock("tableLock");
    for (int i = 0; i < size; ++i)
	{
		table[i] = NULL;
	}
    used = 0;
}

Table::~Table()
{
    delete table;
    delete tableLock;
}
// allocate a table slot for 'object'.
// return the table index for the slot or -1 on error
int Table::Alloc(void* object)
{
    int tmp;
    tableLock->Acquire();
    int NowIndex=0;

    while (table[NowIndex]!= NULL &&NowIndex < size){
        NowIndex++;
    }

	if (NowIndex >= size)
		tmp = -1;
    else
    {
        table[NowIndex]=object;
        tmp=NowIndex;
        // return -1;
    }
    tableLock->Release();
    return tmp;
    
}
// return the object from table index 'index' or NULL on error.
// (assert index is in range).  Leave the table entry allocated
// and the pointer in place.
void* Table::Get(int index)
{
    void *tmp;
    tableLock->Acquire();
    // ASSERT(index<size);
    if (index >= size){

        tmp=NULL;
    }
    else
    {
        tmp=table[index];
        tableLock->Release();
    }
    return tmp;
}

void Table::Release(int index)
{
    tableLock->Acquire();
    if (index < 0 || index >= size){
        tableLock->Release();
    }
    else
    {
        table[index] = NULL;
        tableLock->Release();
    }

}

bool Table::Isfull()
{
    return used == size;
}




