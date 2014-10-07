#ifdef CHANGED
#include "grouptest.h"

// We need two conditions, one for when the buffer is full
// and one for when the buffer is empty.
// And we need one lock to protect the buffer.
Condition *full;
Condition *empty;
Lock *lock;

int prodIndex, conIndex, BUFFERSIZE = 5;
char * buff;
char * hello;

// char arrays to hold thread names, allows for knowing 
// which thread when multiple producers or consumers are present
char strConsumer[12];
char strProducer[12];

//----------------------------------------------------------------------
// consumerThread(threadNum) 
//     creates a consumer that takes stuff out of the buffer and 
//     then puts a null character into the space it just consumed
//     threadNum is the number of the thread, used for debugging purposes. 
//----------------------------------------------------------------------

void consumerThread(int threadNum) {
  DEBUG('t', "in consumer\n");
  
  // create a loop that only breaks once a final character was received
  while (1) {

    // at the beginning of each loop, we acquire the lock, 
    // ensuring mutual exclusion
    lock->Acquire();

    // if true, then the buffer is empty, and we need to wait for 
    // more items to consume
    while (prodIndex == conIndex) {
      empty->Wait(lock);
    }

    char ch = buff[conIndex];
    
    // delete the contents of the buffer.  ensures the paradigm of a 
    // consumer is followed
    buff[conIndex] = '\0';

    conIndex++;

    // Make sure that index is never greater than buffer size
    // which would make bad memory accesses
    if (conIndex >= BUFFERSIZE) conIndex = 0;
    
    // If we get this far, we are sure that we have taken a character
    // out of the buffer, and mutual exclusion ensures that no other 
    // has touched the buffer. 
    // so we can signal a producer to fill the vacant spot 
    // in the buffer
    full->Signal(lock);
    
    // release the lock after we need no more accesses to shared data.
    lock->Release();

    // output what was retrieved from the buffer to the console
    printf("*** Consumer %d:   ---> %c\n", threadNum, ch);

    // for 'Hello world' we know we have reached the end of the string
    // because there is only one 'd' in the string
    // so we know we can quit
    if (ch == 'd')  break;
  }
  printf("*** Consumer %d: Now quitting\n", threadNum);
}
  
//----------------------------------------------------------------------
// producerThread(threadNum)
//     creates a producer that puts the characters from 'Hello world',
//     which is a char array with a null terminator, into a shared buffer
//     threadNum is the numberof the thread, used for debugging purposes.
//----------------------------------------------------------------------

void producerThread(int threadNum) {
  DEBUG('t', "in producer\n");
  // we need a per function index of the location in the array from 
  // which to take the characters from 'Hello world'.
  int helloIndex;
  // go through every character in the array.
  // when the null character is hit, we know we are at the end of 
  // the array and can exit the function
  for (helloIndex = 0; hello[helloIndex] != '\0'; helloIndex++) {
    
    // at the beginning of each loop, we acquire the lock, 
    // ensuring mutual exclusion
    lock->Acquire();

    // output what we put into the buffer, for easy to understand results
    printf("*** Producer %d: %c --->\n", threadNum, hello[helloIndex]);

    // if the index of the producer is exactly one behind the consumer
    // we know the buffer is full and cannot put any more data in
    // until we are allowed.
    while ((prodIndex + 1) % BUFFERSIZE == conIndex) full->Wait(lock);

    buff[prodIndex] = hello[helloIndex];
    prodIndex++;
    
    // loop the index if it is greater than the buffer size to get valid
    // access to the buffer
    if (prodIndex >= BUFFERSIZE) prodIndex = 0;

    // mutual exclusion ensures no other item was operating on the buffer
    // while this producer was in here. so we know that there is at least
    // one item, and we can call a consumer to do its job
    empty->Signal(lock);

    // at the end of each loop, we can release the lock.
    lock->Release();
    
  }
}
  
//----------------------------------------------------------------------
// lockTestStart()
//     this function initializes the data and threads for use in the 
//     producer consumer example.
//     numConsumers and numProducers default to 4,
//     but can be declared on the command line
//----------------------------------------------------------------------

void lockTestStart( int numConsumers, int numProducers) {
  DEBUG('t', "Entering LockTestStart\n");

  // initialize the lock and condition variables
  full = new(std::nothrow) Condition("producer");
  empty = new(std::nothrow) Condition("consumer");
  lock = new(std::nothrow) Lock("lock");
  // all indexes set to zero
  prodIndex = 0;
  conIndex = 0;

  // initialize the buffers, one from which to get the data.
  hello = (char *)"Hello world";
  buff = new char[BUFFERSIZE];

  // an array of threads makes initialization easy in a for loop
  Thread * consumer[numConsumers];
  Thread * producer[numProducers];

  int i;

  //create producers and consumers
  for (i = 1; i <= numProducers; i++ ) {
    sprintf(strProducer, "%s%d", "producer", i);
    producer[i] = new(std::nothrow) Thread(strProducer);
    producer[i]->Fork(producerThread, i);
  }

  for (i = 1; i <= numConsumers; i++ ) {
    sprintf(strConsumer, "%s%d", "consumer", i);
    consumer[i] = new(std::nothrow) Thread(strConsumer);
    consumer[i]->Fork(consumerThread, i);
  }

}


#endif
