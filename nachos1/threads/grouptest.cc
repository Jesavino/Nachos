#ifdef CHANGED
#include "grouptest.h"

Condition *full;
Condition *empty;
Lock *pLock;
Lock *cLock;
int prodBuff,conBuff, inBuffer, BUFFERSIZE = 10;
char * buff;
char * hello;
char strConsumer[12];
char strProducer[12];

void consumerThread(int threadNum) {
  DEBUG('t', "in consumer\n");
  char ch = '\0';
  while (1) {
    pLock->Acquire();

    if (prodBuff == conBuff) {
      empty->Wait(pLock);
    }
    ch = buff[conBuff];
    conBuff++;
    if (conBuff >= BUFFERSIZE) conBuff = 0;
    
    full->Signal(pLock);
    pLock->Release();
    if (ch != 'z')
      printf("*** Consumer %d:   ---> %c\n", threadNum, ch);
    else break;
  }
  //printf("%d\n", inBuffer);
  //printf("Consumer %d done\n", threadNum);

}
  
void producerThread(int threadNum) {
  DEBUG('t', "in producer\n");
  int helloBuff;
  for (helloBuff = 0; hello[helloBuff] != '\0'; helloBuff++) {
    printf("*** Producer %d: %c --->\n", threadNum, hello[helloBuff]);
    pLock->Acquire();

    if ((prodBuff + 1) % BUFFERSIZE == conBuff) full->Wait(pLock);
    //printf("Thread %d put %c\n", threadNum, hello[helloBuff]);
    buff[prodBuff] = hello[helloBuff];
    prodBuff++;
    if (prodBuff >= BUFFERSIZE) prodBuff = 0;
    empty->Signal(pLock);
    pLock->Release();
    
  }
  buff[prodBuff] = 'z';
  //  prodBuff = (prodBuff + 1) % BUFFERSIZE
  //printf("Producer %d done.\n", threadNum);
}
  
void lockTestStart() {
  DEBUG('t', "Entering LockTestStart\n");
  full = new(std::nothrow) Condition("producer");
  empty = new(std::nothrow) Condition("consumer");
  pLock = new(std::nothrow) Lock("pLock");
  cLock = new(std::nothrow) Lock("cLock");

  prodBuff = 0;
  conBuff = 0;
  inBuffer = 0;
  int numThreads = 2;

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
