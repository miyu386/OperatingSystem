void printString(char* buff) {
  interrupt(0x21, 0, buff, 0, 0);
}

void readString(char* buff) {
  interrupt(0x21, 1, buff, 0, 0);
}

void readFile(char* file, char* buff) {
  interrupt(0x21, 3, file, buff, 0);
}

void execute(char* buff) {
  interrupt(0x21, 4, buff, 0x2000, 0);
}

int deleteFile(char* file) {
  interrupt(0x21, 7, file, 0, 0);
}

void writeFile(char* file, char* buff, int sectors) {
  interrupt(0x21, 8, file, buff, sectors);
}

void readSector(char* buff, int sector) {
  interrupt(0x21, 2, buff, 2, 0);
}
