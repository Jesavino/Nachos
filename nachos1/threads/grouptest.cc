#ifdef CHANGED
#include "grouptest.h"

Condition *full;
Condition *empty;
Lock *pLock;
Lock *cLock;
int prodBuff,conBuff, inBuffer = 0, BUFFERSIZE = 2;
int loopCount, numLoops = 200;
char * buff;
char * hello;
char strConsumer[12];
char strProducer[12];

void consumerThread(int threadNum) {
  DEBUG('t', "in consumer\n");
  char ch = 'a';
  while (ch != '\0') {
    cLock->Acquire();
    if (inBuffer == 0) empty->Wait(cLock);
    ch = buff[conBuff];
    inBuffer--;
    conBuff = (conBuff + 1) % BUFFERSIZE;
    
    full->Signal(pLock);
    cLock->Release();
    printf("%c", ch);
  }
  printf("%d\n", inBuffer);

}
  
void producerThread(int threadNum) {
  DEBUG('t', "in producer\n");
  int helloBuff = 0;
  while (hello[helloBuff] != '\0') {
    pLock->Acquire();
    if (inBuffer == BUFFERSIZE) full->Wait(pLock);
    buff[prodBuff] = hello[helloBuff];
    inBuffer++;
    prodBuff = (prodBuff + 1) % BUFFERSIZE;
    //helloBuff = (helloBuff + 1) % strlen(hello);
    helloBuff++;
    empty->Signal(cLock);
    

    pLock->Release();
  }
  prodBuff = (prodBuff + 1) % BUFFERSIZE;
}
  
void lockTestStart() {
  DEBUG('t', "Entering LockTestStart\n");
  full = new(std::nothrow) Condition("producer");
  empty = new(std::nothrow) Condition("consumer");
  pLock = new(std::nothrow) Lock("pLock");
  cLock = new(std::nothrow) Lock("cLock");

  prodBuff = 0;
  conBuff = 0;
  loopCount = 0;
  int numThreads = 1;

  hello = (char *)"Hello world";
  buff = new char[BUFFERSIZE];

  Thread * consumer[numThreads];
  Thread * producer[numThreads];

  int i;

  //create producer consumer
  for (i = 1; i <= numThreads; i++ ) {

    sprintf(strConsumer, "%s%d", "consumer", i);
    sprintf(strProducer, "%s%d", "producer", i);

    consumer[i] = new(std::nothrow) Thread(strConsumer);
    producer[i] = new(std::nothrow) Thread(strProducer);

    consumer[i]->Fork(consumerThread, i);
    producer[i]->Fork(producerThread, i);
  }

}

#endif
