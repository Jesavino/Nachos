#ifdef CHANGED
#include "grouptest.h"

Condition *full;
Condition *empty;
Lock *pLock;
Lock *cLock;
int prodBuff, helloBuff, conBuff, bufferSize, inBuffer;;
int loopCount;

void consumerThread(int buffer) {
  DEBUG('t', "in consumer");
  char * buff = (char *) buffer;
  
  while (1) {
    if (loopCount >= 1) break;
    cLock->Acquire();
    while (inBuffer == 0) empty->Wait(cLock);
    std::cout << buff[conBuff];
    inBuffer--;
    conBuff = (conBuff + 1) % bufferSize;
    if (conBuff == 0) {
      std::cout << "\n";
      loopCount++;
    }
    cLock->Release();

    if (inBuffer == 1) full->Signal(pLock);
    
  }
}
  
void producerThread(int buffer) {
  DEBUG('t', "in producer");
  char * buff = (char *) buffer;
  char * hello = (char *)"Hello world";

  while (1){
    if (loopCount >= 1) break;

    pLock->Acquire();
    while (inBuffer == bufferSize) full->Wait(pLock);
    buff[prodBuff] = hello[helloBuff];
    inBuffer++;
    prodBuff = (prodBuff + 1) % bufferSize;
    helloBuff = (helloBuff + 1) % strlen(hello);
    pLock->Release();
    
if ( inBuffer == bufferSize - 1) empty->Signal(cLock);
  }
  
}
  
void lockTestStart() {
  DEBUG('t', "Entering LockTestStart\n");
  full = new(std::nothrow) Condition("producer");
  empty = new(std::nothrow) Condition("consumer");
  pLock = new(std::nothrow) Lock("pLock");
  cLock = new(std::nothrow) Lock("cLock");
  prodBuff = 0;
  helloBuff = 0;
  conBuff = 0;
  bufferSize = 88;
  inBuffer = 0;
  loopCount = 0;
  //create producer consumer
  char buffer[bufferSize];
  char * pbuffer = buffer;

  Thread * consumer[5];
  Thread * producer[5];
  int i;
  for (i = 0; i < 5; i++ ) {
    //    char numStr[2];
    char * strConsumer = (char *)"consumer";
    char * strProducer = (char *)"producer";
    //sprintf(numStr, "%d", i);
    //char * strConsumer = strcat(con, numStr);
    //char * strProducer = strcat(prod, numStr);

    consumer[i] = new(std::nothrow) Thread(strConsumer);
    producer[i] = new(std::nothrow) Thread(strProducer);
    consumer[i]->Fork(consumerThread, (int) pbuffer);
    producer[i]->Fork(producerThread, (int) pbuffer);
  }

  /*
  Thread * consumer1 = new(std::nothrow) Thread("consumer1");
  Thread * consumer2 = new(std::nothrow) Thread("consumer2");
  Thread * consumer3 = new(std::nothrow) Thread("consumer3");

  Thread * producer1 = new(std::nothrow) Thread("producer1");
  Thread * producer2 = new(std::nothrow) Thread("producer2");
  Thread * producer3 = new(std::nothrow) Thread("producer3");

  consumer1->Fork(consumerThread, (int ) pbuffer);
  producer1->Fork(producerThread, (int ) pbuffer);
  consumer2->Fork(consumerThread, (int ) pbuffer);
  producer2->Fork(producerThread, (int ) pbuffer);
  consumer3->Fork(consumerThread, (int ) pbuffer);
  producer3->Fork(producerThread, (int ) pbuffer);
  */
}

#endif
