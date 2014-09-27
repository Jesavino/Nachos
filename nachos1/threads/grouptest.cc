#ifdef CHANGED
#include "grouptest.h"

Condition *full;
Condition *empty;
Lock *pLock;
Lock *cLock;
int prodBuff, helloBuff, conBuff, bufferSize;
int loopCount, numLoops = 20;
char * buff;

void consumerThread(int buffer) {
  DEBUG('t', "in consumer\n");
  
  while (1) {
    cLock->Acquire();
    while (conBuff == prodBuff) empty->Wait(cLock);
    char ch = buff[conBuff];
    buff[conBuff] = '\0';
    conBuff = (conBuff + 1) % bufferSize;
    if (conBuff == 0) {
      loopCount++;
    }
    full->Signal(pLock);
    cLock->Release();

    printf("%c", ch);
    if (ch == 'd') printf("\n");
    if (loopCount >= numLoops) break;
  }
}
  
void producerThread(int buffer) {
  DEBUG('t', "in producer\n");
  char * hello = (char *)"Hello world";

  while (1){
    pLock->Acquire();

    while ((prodBuff + 1) % bufferSize == conBuff) full->Wait(pLock);
    buff[prodBuff] = hello[helloBuff];
    prodBuff = (prodBuff + 1) % bufferSize;
    helloBuff = (helloBuff + 1) % strlen(hello);
    empty->Signal(cLock);

    pLock->Release();
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
  bufferSize = 3;
  loopCount = 0;
  int numThreads = 3;

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
