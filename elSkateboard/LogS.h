
#ifndef __LOGS_H__
#define __LOGS_H__

struct LogS {
  long timeMs;  // in millisecond
  byte type;  // refer to defined types below
  double value; // refer to defined types below
  boolean isWritten;  // indicates if this entry has been written
  boolean isRead; // indicates if this entry has been read
  boolean overwritten;  // indicates if this entry overwrittes an un-read entry
};

/*
 * Define types that can be used for LogS.type field
 */
// Voltages, values will be in Volts
#define LOG_TYPE_VOL_0  0
#define LOG_TYPE_VOL_1  1
#define LOG_TYPE_VOL_2  2
// Currents, values will be in Amps
#define LOG_TYPE_CUR_0  3
#define LOG_TYPE_CUR_1  4
#define LOG_TYPE_CUR_2  5
// Speed, values will be in km/h
#define LOG_TYPE_SPD_0  6
#define LOG_TYPE_SPD_1  7
#define LOG_TYPE_SPD_2  8
#define LOG_TYPE_NONE   255

#define LOG_ENTRY_NUM 8
LogS logs[LOG_ENTRY_NUM];
volatile byte logWriteIndex = 0;
volatile byte logReadIndex = 0;

void logClear() {
  for (int i=0;i<LOG_ENTRY_NUM;i++) {
    logs[i].timeMs = 0;
    logs[i].type = LOG_TYPE_NONE;
    logs[i].value = 0.0;
    logs[i].isWritten = false;
    logs[i].isRead = false;
    logs[i].overwritten = false;
  }
}

void logWrite(LogS* pLog) {
  byte index = logWriteIndex;
  logWriteIndex = (logWriteIndex + 1) & 0x7;
  logs[index].timeMs = pLog->timeMs;
  logs[index].type = pLog->type;
  logs[index].value = pLog->value;
  if (logs[index].isWritten && !logs[index].isRead) {
    logs[index].overwritten = true;
  } else {
    logs[index].overwritten = false;
  }
  logs[index].isWritten = true;
  logs[index].isRead = false;
}

#endif  // __LOGS_H__
