#ifdef CHANGED
#include "grouptest.h"

Condition *full;
Condition *empty;
Lock *pLock;
Lock *cLock;
int prodBuff, helloBuff, conBuff, BUFFERSIZE = 5;
int loopCount, numLoops = 200;
char * buff;
char * hello;

void consumerThread(int threadNum) {
  DEBUG('t', "in consumer\n");
  
  while (1) {
    cLock->Acquire();
    
    while (conBuff == prodBuff) empty->Wait(cLock);
    char ch = buff[conBuff];
    conBuff = (conBuff + 1) % BUFFERSIZE;
    // only to end the infinite loop after numLoops times
    if (conBuff == 0) {
      loopCount++;
    }
    // 
    
    //if (conBuff == ((prodBuff + 1) % bufferSize))
      full->Signal(pLock);
    cLock->Release();

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

    while (((prodBuff + 1) % BUFFERSIZE) == conBuff) full->Wait(pLock);
    buff[prodBuff] = hello[helloBuff];
    //printf("Thread %d input %c\n", threadNum, buff[prodBuff]);
    prodBuff = (prodBuff + 1) % BUFFERSIZE;
    helloBuff = (helloBuff + 1) % strlen(hello);

    //if (((prodBuff + 2) % bufferSize) == conBuff) 
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
  loopCount = 0;
  int numThreads = 5;

  hello = (char *)"Hello world";
  //printf("%d\n", strlen(hello));
  buff = new char[BUFFERSIZE];

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
