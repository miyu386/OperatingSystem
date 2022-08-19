//Billy Lee Michael Krause

#define MAIN

#include "proc.h"

void putChar(char character, int row, int column, char color);
void putStr(char* string, int row, int column, char color);
int printInt(int a);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int mod(int a, int b);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int readFile(char *filename, char *buf);
int executeProgram(char* fname);
void terminate();
int writeSector(char *buffer, int sector);
int deleteFile(char *fname);
int writeFile(char *fname, char *buffer, int sectors);
void handleTimerInterrupt(int segment, int stackPointer);
void kStrCopy(char *src, char *dest, int len);
// void returnFromTimer(int segment, int stackPointer);
main() {
  initializeProcStructures();
//  char buffer[13312];
    /* the maximum size of a file*/
//  makeInterrupt21();
  /*read the file into buffer*/
//  interrupt(0x21, 0x03, "messag\0", buffer, 0);
  /*print out the file*/
//  interrupt(0x21, 0x00, buffer, 0, 0);
//  while(1);     /* infinite loop */
//  deleteFile("messag\0");


  makeInterrupt21();

  handleInterrupt21(0x21,0x04,"shell\0",0,0);
  makeTimerInterrupt();



  // makeTimerInterrupt();
  // interrupt(0x21, 0x03, "messag\0", buffer, 0);
  // interrupt(0x21, 0x08, "testin\0", buffer, 4);
  // interrupt(0x21, 0x03, "testin\0", buffer, 0);
  //writeFile("messag\0", buffer, 4);
  //readFile("messag\0", buffer);
  // interrupt(0x21, 0x00, buffer, 0, 0);
  while(1);
}


void putChar(char character, int row, int column, char color) {

    putInMemory(0xB000, 0x8000+(row-1)*160+(column-1)*2, character);
    putInMemory(0xB000, 0x8000+(row-1)*160+(column-1)*2+1, color);

}

void putStr(char* string, int row, int column, char color) {
    int i = 0;

    while(string[i]!='\0') {
	     putChar(string[i], row, column+i, color);
	      i++;
    }

  }
int printInt(int a){
  if(a<10){
    char al;
    char ah;
    int ax;
    al = a + '0';
    ah = 0x0E;
    ax = ah * 256 + al;
    interrupt(0x10, ax, 0, 0, 0);
  }
  else{
    printInt(a/10);
    printInt(mod(a,10));
  }
  return a;
}
int printString(char *str){
    int i = 0;
    char al;
    char ah;
    int ax;
    while(str[i]!='\0'){
      char al = str[i];
      char ah = 0x0E;
      int ax = ah * 256 + al;
      interrupt(0x10, ax, 0, 0, 0);
      i++;
    }
    return i;
}
int readChar(){
  return interrupt(0x16, 0, 0, 0, 0);
}

int readString(char *buf){
    char temp[2];
    int i = 0;
    char ch = readChar();
    while(ch!=0x0D){
      if(ch == 0x08 && i != 0){
        buf[i] = 0x00;
        i--;
      //  temp[0]=0x00;
      //  temp[1]='\0';
      //  printString(temp);
        printString(" \b");
      }
      buf[i] = ch;
      ch = readChar();
      temp[0]=ch;
      temp[1]='\0';
      printString(temp);
      i++;

    }
    buf[i]=0x00;
    return i;
}
int readSector(char *buf, int absSector) {
  int ax = 0x02 * 256 + 0x01;
  int bx = buf;
  int ch = absSector/36;
  int cl = mod(absSector, 18) + 1;
  int cx = ch * 256 + cl;
  int dh = mod(absSector/18,2);
  int dx = dh * 256 + 0x00;

  interrupt(0x13, ax, bx, cx, dx);
}
int writeSector(char* buffer, int sector){
  int ax = 0x03 * 256 + 0x01;
  int bx = buffer;
  int ch = sector/36;
  int cl = mod(sector, 18) + 1;
  int cx = ch * 256 + cl;
  int dh = mod(sector/18,2);
  int dx = dh * 256 + 0x00;

  interrupt(0x13, ax, bx, cx, dx);
}
int mod(int a, int b){
  while(a>=b){
    a=a-b;
  }
  return a;
}
int handleInterrupt21(int ax, int bx, int cx, int dx){

  if(ax == 0x00){
    return printString(bx);
  }

  if(ax == 0x11){
    readString(bx);
    return 1;
  }

  if(ax == 0x01){
    return readString(bx);
  }
  if(ax == 0x02){
    return readSector(bx, cx);
  }
  if(ax == 0x03){
    return readFile(bx,cx);
  }
  if(ax == 0x04){
    return executeProgram(bx);
  }
  if(ax == 0x05){
    return terminate();
  }
  if(ax == 0x07){
    return deleteFile(bx);
  }
  if(ax == 0x08){
    return writeFile(bx, cx, dx);
  }

  return -1;


}
int readFile(char *filename, char *buf){
  char buffer[512];
  int sector[26];
  int fileIndex;
  int i;
  int totalIndex;
  fileIndex = 0;
  i = 1;
  totalIndex = 0;

  readSector(buffer,2);

  while(totalIndex < 512 || fileIndex < 6){

    if(buffer[totalIndex] == filename[fileIndex]){
      fileIndex++;

      if(fileIndex == 6){
        break;
      }
    }
    else{
      fileIndex=0;
    }
    if(i==6){
      totalIndex+=27;
      i=1;
    }
    else{
      totalIndex++;
      i++;
    }
  }
  if(fileIndex == 6){
    int j;
    int counter;
    for(j=0;j<26;j++){
      sector[j]=buffer[totalIndex+j+1];
    }

    counter = 0;
    while(sector[counter]!= 0x00 && counter < 26){
      readSector(buf,sector[counter]);
      buf+=512;
      counter++;
    }
  }
  else{
    return -1;
  }
}

int executeProgram(char* fname){
  char fileBuf[13312];
  struct PCB *nextPCB;
  int i;
  int freeMem;
  setKernelDataSegment();
  if (getFreeMemorySegment()==NO_FREE_SEGMENTS) {
    return -2;
  } else {
    freeMem = getFreeMemorySegment();
  }
  restoreDataSegment();

  if(readFile(fname, fileBuf)==-1){
    return -1;
  }

  for(i = 0; i < 13312;i++){
    putInMemory(freeMem,i, fileBuf[i]);
  }
  setKernelDataSegment();
  // initializeProcStructures();
  nextPCB = getFreePCB();
  kStrCopy(fname, nextPCB -> name, 7);
  nextPCB -> state = READY;
  nextPCB -> stackPointer = 0xFF00;
  addToReady(nextPCB);
  restoreDataSegment();
  initializeProgram(freeMem);

  return 1;

  // launchProgram(freeMem);
}
void terminate(){
  setKernelDataSegment();
  releaseMemorySegment(running -> segment);
  releasePCB(running);
  running -> state = DEFUNCT;
  resetSegments();
  //printString("I'm back!\0");
  restoreDataSegment();

  while(1);
}
int deleteFile(char *fname){
  char mapBuf[512];
  char dirBuf[512];
  int fileIndex;
  int i;
  int j;
  int totalIndex;
  fileIndex = 0;
  i = 1;
  totalIndex = 0;
  readSector(mapBuf, 1);
  readSector(dirBuf, 2);


  while(totalIndex < 512 || fileIndex < 6){

    if(dirBuf[totalIndex] == fname[fileIndex]){
      fileIndex++;

      if(fileIndex == 6){
        break;
      }
    }
    else{
      fileIndex=0;
    }
    if(i==6){
      totalIndex+=27;
      i=1;
    }
    else{
      totalIndex++;
      i++;
    }
  }

  if(fileIndex == 6){

    dirBuf[totalIndex - 5] = 0;


    j=0;

    for(j = 6;j < 32 ;j++){
      mapBuf[dirBuf[totalIndex -5 + j]] = 0;
      if(dirBuf[totalIndex -5 + j]== 0){
        break;
      }
    }

    writeSector(dirBuf, 2);
    writeSector(mapBuf, 1);
  } else {
    return -1;
  }

}
int writeFile(char *fname, char *buffer, int sectors){
  char mapBuf[512];
  char dirBuf[512];
  char sector[26];
  int fileIndex;
  int i;
  int index;
  int j;
  int totalIndex;
  int startIndex;
  int returnVal;
  int count;
  readSector(dirBuf, 2);

  fileIndex = 0;
  i = 1;
  totalIndex = 0;
  while(totalIndex < 512 || fileIndex < 6){

    if(dirBuf[totalIndex] == fname[fileIndex]){
      fileIndex++;

      if(fileIndex == 6){
        break;
      }
    }
    else{
      fileIndex=0;
    }
    if(i==6){
      totalIndex+=27;
      i=1;
    }
    else{
      totalIndex++;
      i++;
    }
  }

  if(totalIndex == 6){
    startIndex = totalIndex-5;

  }else{
  //  int count;
    count = 0;
    while(count < 512){
      if(dirBuf[count]==0){
        startIndex = count;
        break;
      }else{
        count += 32;
      }
    }
  }
  if (count>=512) {
    return -1;
  }

  for(j = 0; j < 6;j++){
    dirBuf[startIndex + j] = fname[j];
    if(fname[j]=='\0'){
      break;
    }
  }


  readSector(mapBuf, 1);

  index=0;

  for(j=0; j<26; j++) {
    while(mapBuf[index]!=0x00) {
      if (index>=512) {
        if(j<sectors) {
          returnVal = -2;
        }
      }
      index++;
    }
    if (index<512) {
      sector[j]=index;
      index++;
    }
  }

  for(j=0; j<26; j++) {
    if (sector[j]==0x00) {
      returnVal = j;
    }
    mapBuf[sector[j]]= 0xFF;
    dirBuf[startIndex+6+j]=sector[j];
    writeSector(buffer, sector[j]);
    buffer+=512;
  }
  if (j=26) {
    returnVal = 26;
  }
  //dirBuf[startIndex+6+512]=0;

  writeSector(dirBuf, 2);
  writeFile(mapBuf, 1);

  return returnVal;

}
void handleTimerInterrupt(int segment, int stackPointer){
    struct PCB *nextProcess;
    // printString("Tic\0");
    // returnFromTimer(segment, stackPointer);
    running -> stackPointer = stackPointer;
    running -> state = READY;
    readyTail -> next = running;
    if (readyHead==readyTail) {
      nextProcess = &idleProc;
    } else {
      nextProcess = removeFromReady();
    }
    nextProcess -> state = RUNNING;
    running = nextProcess;
    returnFromTimer(nextProcess -> segment, nextProcess -> stackPointer);
}
void kStrCopy(char *src, char *dest, int len){
  int i=0;
  for(i = 0; i < len;i++){
    putInMemory(0x1000, dest+i, src[i]);
    if(src[i] == 0x00){
      return;
    }
  }
}
