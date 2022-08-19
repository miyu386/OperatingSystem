

main()
{
  int i=0;
  int j=0;
  int k=0;
  enableInterrupts();
  for(i=0; i<1000; i++) {
    interrupt(0x21, 0, "Hello\n\r\0", 0, 0);
    for(j=0; j<10000; j++) {
      for(k=0; k<1000; k++) {
    }
   }
  }
  interrupt(0x21, 0x05, 0, 0, 0);
}
