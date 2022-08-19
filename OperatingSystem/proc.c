#include "proc.h"

void initializeProcStructures() {
  int i;
  for(i=0; i<8; i++) {
    memoryMap[i] = FREE;
    pcbPool[i].name[0] = NULL;
    pcbPool[i].state = DEFUNCT;
    pcbPool[i].segment = 0x0000;
    pcbPool[i].stackPointer = 0x0000;
  }

  idleProc.name[0] = 'I';
  idleProc.name[1] = 'D';
  idleProc.name[2] = 'L';
  idleProc.name[3] = 'E';
  idleProc.name[4] = '\0';

  idleProc.state = READY;
  idleProc.segment = 0x0000;
  idleProc.stackPointer = 0x0000;
  running = &idleProc;
  readyHead = NULL;
  readyTail = NULL;
}

int getFreeMemorySegment() {
  int i;
  for (i=0; i<8; i++) {
    if (memoryMap[i]==FREE) {
      memoryMap[i] = USED;
      return (i+2)*0x1000;
    }
  }
  return NO_FREE_SEGMENTS;
}

void releaseMemorySegment(int seg) {
  int i;
  i = (seg/0x1000) - 2;
  memoryMap[i] = FREE;
}

/*
 * Return a pointer to an available PCB from the PCB pool.  All of the
 * available PCBs should be marked as DEFUNCT.  The returned PCB should
 * be marked as STARTING. Return NULL if there are no PCBs available.
 */
struct PCB *getFreePCB() {
  int i;
  for (i=0; i<8; i++) {
    if (pcbPool[i].state==DEFUNCT) {
      pcbPool[i].state = STARTING;
      return &pcbPool[i];
    }
  }
  return NULL;
}

/*
 * Release the provided PCB by setting its state to DEFUNCT, setting
 * its next and prev pointers to NULL and setting the first character
 * of its name to 0x00.
 */
void releasePCB(struct PCB *pcb) {
  pcb -> state = DEFUNCT;
  pcb -> next = NULL;
  pcb -> prev = NULL;
  pcb -> name[0] = 0x00;
}

/*
 * Add the provided PCB to the tail of the ready queue.
 */
void addToReady(struct PCB *pcb) {
  if (readyHead == NULL) {
    readyHead = pcb;
    readyTail = pcb;
  } else {
    readyTail -> next = pcb;
    readyTail = pcb;
  }
}

/*
 * Remove the PCB at the head of the ready queue and return a
 * pointer to it.
 */
struct PCB *removeFromReady() {
  struct PCB *removedPCB;
  removedPCB = readyHead;
  if (readyHead==readyTail) {
    readyHead = NULL;
    readyTail = NULL;
  } else {
    readyHead = readyHead -> next;
  }
  return removedPCB;
}
