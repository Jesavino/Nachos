#ifdef CHANGED

Locktest() {
  full = new(std::nothrow) Condition("producer");
  empty = new(std::nothrow) Condition("consumer");
  pLock = new(std::nothrow) Lock("pLock");
  cLock = new(std::nothrow) Lock("cLock");
  n = 0;
  m = 0;
  i = 0;
  size = 2;
  inBuffer = 0;
}


  void consumerThread(int buffer) {
    char * buff = (char *) buffer;
    
    while (1) {
      cLock->Acquire();
      if (inBuffer == 0) empty->Wait(cLock);
      std::cout << buff[i] << std::endl;
      inBuffer--;
      i = (i + 1) % size;
      cLock->Release();
      full->Signal(pLock);
      
    }
  }
  
  void producerThread(int buffer) {
    char * buff = (char *) buffer;
    char * hello = "Hello world";
    int size = 2;
    while (1){
      pLock->Acquire();
      if (inBuffer == size) full->Wait(pLock);
      buff[n] = hello[m];
      inBuffer++;
      n = (n + 1) % size;
      m = (m + 1);
      
      pLock->Release();
      empty->Signal(cLock);
      if (m >= strlen(hello)) break;
    }
    
  }
  
void LockTest() {
  DEBUG('t', "Entering Locktest\n");
  //create producer consumer
  char buffer[size];
  char * pbuffer = buffer;
  Thread * consumer = new(std::nothrow) Thread("consumer");
  Thread * producer = new(std::nothrow) Thread("producer");

  consumer->Fork(Locktest::consumerThread, (int ) pbuffer);
  Locktest::producerThread((int ) pbuffer);

}

#endif
