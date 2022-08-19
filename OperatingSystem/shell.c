void printString(char* buff);
void readString(char* buff);
void readFile(char*file, char*buff);
void command(char* buff);
void execute(char* buff);
int compare(char* str1, char* str2);
int deleteFile(char* file);
void writeFile(char* file, char* buff, int sectors);
void readSector(char* buff, int sector);
void dir(char* directory);

main(){
  char userline[50];
  enableInterrupts();

  while(1) {
    printString("shell>");
    readString(userline);
    printString("\n\r\0");
    command(userline);
    printString("\n\r\0");

  }
}

void command(char* buff) {
  char commandName[10];
  char argumentName[10];
  char argumentName2[10];
  char output[512];
  int pointer;
  pointer=0;

  while(*buff != ' ') {
    commandName[pointer] = *buff;
    buff++;
    pointer++;
  }
  commandName[pointer]='\0';
  buff++;
  pointer=0;

  while(*buff != '\0' && *buff != ' ') {
    argumentName[pointer] = *buff;
    buff++;
    pointer++;
  }
  argumentName[pointer]='\0';

  buff++;
  pointer=0;

  while(*buff != '\0' && *buff != ' ') {
    argumentName2[pointer] = *buff;
    buff++;
    pointer++;
  }
  argumentName2[pointer]='\0';

  if (compare(commandName, "type\0")==1) {
    readFile(argumentName, output);
    printString(output);
  }
  else if (compare(commandName, "execute\0")==1) {
    execute(argumentName);
  }
  else if (compare(commandName, "delete\0")==1) {
    if (deleteFile(argumentName)==-1) {
      printString("File not found.");
    }
  }
  else if (compare(commandName, "copy\0")==1) {
    readFile(argumentName, output);
    writeFile(argumentName2, output, 26);
  }
  else if (compare(commandName, "dir\0")==1) {
    readSector(output, 2);
    dir(output);
  }
  else {
    printString("not a valid command\0");
  }
}

int compare(char* str1, char* str2) {
  int ind1;
  int ind2;
  ind1=0;
  ind2=0;

  while(str1[ind1]!='\0') {

    if (str1[ind1]==str2[ind2]) {
      ind1++;
      ind2++;
    } else if (str2[ind2]=='\0') {
      return 0;
    } else return 0;
  }

  return 1;
}

void dir(char* directory) {
  int entry;
  int fileIndex;
  for(entry=0; entry<16; entry++) {
    char fileName[6];
    int pointer;
    pointer = entry*32;
    if (directory[pointer]==0x00) {
      continue;
    }

    for(fileIndex=0; fileIndex<6; fileIndex++) {
      if (directory[pointer+fileIndex] != 0x00) {
        fileName[fileIndex] = directory[pointer + fileIndex];
      } else {
        fileName[fileIndex] = '\0';
        break;
      }
    }
    printString(fileName);
    printString("\n\r\0");
  }
}
