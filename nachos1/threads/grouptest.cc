#ifdef CHANGED
#include "grouptest.h"

Condition *full;
Condition *empty;
Lock *pLock;
Lock *cLock;
int prodBuff, helloBuff, conBuff, bufferSize;
int loopCount, numLoops = 20;
char * buff;
char * hello;

void consumerThread(int threadNum) {
  DEBUG('t', "in consumer\n");
  
  while (1) {
    cLock->Acquire();
    while (conBuff == prodBuff) empty->Wait(cLock);
    char ch = buff[conBuff];
    conBuff = (conBuff + 1) % bufferSize;
    if (conBuff == 0) {
      loopCount++;
    }
    cLock->Release();
    full->Signal(pLock);

    //printf("Thread %d output '%c'\n", threadNum, ch);
    printf("%c", ch);
    //    if (ch == 'd') printf("\n");
    if (loopCount >= numLoops) break;
  }
}
  
void producerThread(int threadNum) {
  DEBUG('t', "in producer\n");

  while (1){
    pLock->Acquire();

    while (((prodBuff + 1) % bufferSize) == conBuff) full->Wait(pLock);
    buff[prodBuff] = hello[helloBuff];
    prodBuff = (prodBuff + 1) % bufferSize;
    helloBuff = (helloBuff + 1) % strlen(hello);

    pLock->Release();
    empty->Signal(cLock);
    if (loopCount >= numLoops) break;

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
  bufferSize = 5;
  loopCount = 0;
  int numThreads = 2;

  hello = (char *)"Hello world";
  char buffer[bufferSize];
  buff = buffer;

  Thread * consumer[numThreads];
  Thread * producer[numThreads];

  int i;

  //create producer consumer
  for (i = 1; i <= numThreads; i++ ) {
    char * strConsumer = (char *)"consumer";
    char * strProducer = (char *)"producer";

    consumer[i] = new(std::nothrow) Thread(strConsumer);
    producer[i] = new(std::nothrow) Thread(strProducer);

    consumer[i]->Fork(consumerThread, i);
    producer[i]->Fork(producerThread, i);
  }

}

#endif
