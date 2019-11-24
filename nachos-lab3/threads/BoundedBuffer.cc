#include "BoundedBuffer.h"

BoundedBuffer::BoundedBuffer(int maxsize)
{
    this->maxsize = maxsize+1;
    bufferLock = new Lock("bufferLock");
    NotFull = new Condition("buffer is not full");
    NotEmpty = new Condition("buffer is not empty");
    front = 0;
    back = 0;
    usedsize=0;
    // used = 0;
    buffer = new char[maxsize]();
}

BoundedBuffer::~BoundedBuffer()
{
    delete bufferLock;
    delete NotFull;
    delete NotEmpty;
    delete [] buffer;
}

void BoundedBuffer::Read(void* data, int size)
{
    bufferLock->Acquire();

    while(front==back){
        NotEmpty->Wait(bufferLock);
    }
    // get
    int i = 0;
    while(back!=front&& i < size){
        *((char*)data+i)=buffer[back];
        back=(back+1)%maxsize;
        usedsize--;
        i++;
    }
    printf("%s out: %s \n","consumer",data);
    NotFull->Signal(bufferLock);
    bufferLock->Release();
}

void BoundedBuffer::Write(void* data, int size)
{
    bufferLock->Acquire();

    while((front+1)%maxsize == back){
        NotFull->Wait(bufferLock);
    }
    //put
    int i = 0;
    while(((front+1)%maxsize)!=back&&i<size){
        buffer[front]=*((char*)data+i);
        front=(front+1)%maxsize;
        usedsize++;
        i++;
    }
    printf("%s in: %s\n","producer",data);
    // printf("buffer %d \n\n",usedsize);
    NotEmpty->Signal(bufferLock);
    bufferLock->Release();
}




