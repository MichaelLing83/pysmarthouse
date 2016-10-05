#ifndef __LOGGER_CPP__
#define __LOGGER_CPP__

#include "Logger.h"

Logger::Logger(String _fileName):
  fileName(_fileName){
    LogS logs[LOGGER_BUFFER_LENGTH];
    entryNum = 0;
    writeIndex = 0;
    readIndex = 0;

    for (int i=0; i<LOGGER_BUFFER_LENGTH; i++) {
      logs[i].timeMs = 0;
      logs[i].type = EMPTY;
      logs[i].value = 0.0;
    }
}

void Logger::writeToMem(long timeMs, byte type, float value) {
  byte wi = writeIndex;
  writeIndex = (writeIndex + 1) & LOGGER_BUFFER_INDEX_MASK;
  entryNum++;
  logs[wi].timeMs = timeMs;
  logs[wi].type = type;
  logs[wi].value = value;
}

void Logger::writeToFile() {
  byte ri;
  if (entryNum > 0) {
    File dataFile = FileSystem.open(LOGGER_DEFAULT_FILE_NAME, FILE_APPEND);
    while (entryNum > 0) {
      ri = readIndex;
      readIndex = (readIndex + 1) & LOGGER_BUFFER_INDEX_MASK;
      entryNum--;
      #ifdef ENABLE_DEBUG
        DDD.print(logs[ri].timeMs);
        DDD.print("\t");
        DDD.print(logs[ri].type);
        DDD.print("\t");
        DDD.println(logs[ri].value);
      #endif
      dataFile.print(logs[ri].timeMs);
      dataFile.print("\t");
      dataFile.print(logs[ri].type);
      dataFile.print("\t");
      dataFile.println(logs[ri].value);
    }
    dataFile.close();
  }
}

#endif  // __LOGGER_CPP__
