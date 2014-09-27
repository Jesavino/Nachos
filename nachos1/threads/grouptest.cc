#ifdef CHANGED
#include "grouptest.h"

Condition *full;
Condition *empty;
Lock *pLock;
Lock *cLock;
int prodBuff, helloBuff, conBuff, bufferSize, inBuffer;;

void consumerThread(int buffer) {
  char * buff = (char *) buffer;
  
  while (1) {
    cLock->Acquire();
    if (inBuffer == 0) empty->Wait(cLock);
    std::cout << buff[conBuff];
    inBuffer--;
    conBuff = (conBuff + 1) % bufferSize;
    cLock->Release();
    full->Signal(pLock);
    
  }
}
  
void producerThread(int buffer) {
  char * buff = (char *) buffer;
  char * hello = "Hello world";

  while (1){
    pLock->Acquire();
    if (inBuffer == bufferSize) full->Wait(pLock);
    buff[prodBuff] = hello[helloBuff];
    inBuffer++;
    prodBuff = (prodBuff + 1) %bufferSize;
    helloBuff = (helloBuff + 1) % strlen(hello);
    
    pLock->Release();
    empty->Signal(cLock);
  }
  
}
  
void lockTestStart() {
  full = new(std::nothrow) Condition("producer");
  empty = new(std::nothrow) Condition("consumer");
  pLock = new(std::nothrow) Lock("pLock");
  cLock = new(std::nothrow) Lock("cLock");
  prodBuff = 0;
  helloBuff = 0;
  conBuff = 0;
  bufferSize = 2;
  inBuffer = 0;
  DEBUG('t', "Entering Locktest\n");
  //create producer consumer
  char buffer[bufferSize];
  char * pbuffer = buffer;
  Thread * consumer = new(std::nothrow) Thread("consumerThread");
  Thread * producer = new(std::nothrow) Thread("producerThread");

  consumer->Fork(consumerThread, (int ) pbuffer);
  producer->Fork(producerThread, (int ) pbuffer);

}

#endif
