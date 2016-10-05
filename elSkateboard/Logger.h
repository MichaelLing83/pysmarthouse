
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <Arduino.h>
#include <FileIO.h>

#define LOGGER_BUFFER_LEN_BITS  3
#define LOGGER_BUFFER_LENGTH  (1 << LOGGER_BUFFER_LEN_BITS)
#define LOGGER_BUFFER_INDEX_MASK  ((1 << LOGGER_BUFFER_LEN_BITS) - 1)
#define LOGGER_DEFAULT_FILE_NAME "/mnt/sda1/logs/ArduinoLogs.txt"

enum LogType {
  EMPTY,
  VOLTAGE_OVERALL, // overall system voltage, measured directly from battery
  CURRENT_OVERALL // overall system current, measured directly from battery
};

struct LogS {
  long timeMs;  // in millisecond
  byte type;  // refer to defined types below
  float value; // refer to defined types below
};

class Logger {
  private:
    String fileName;
    LogS logs[LOGGER_BUFFER_LENGTH];
    volatile byte entryNum;
    volatile byte writeIndex;
    volatile byte readIndex;
  public:
    Logger(String _fileName=LOGGER_DEFAULT_FILE_NAME);

    /*
     * Remove log file if it exist.
     */
    void clearFile();

    /*
     * Write log entry in memory, ideal to call from ISR.
     */
    void writeToMem(long timeMs, byte type, float value);

    /*
     * Write available log entries to file. This should not be called from an ISR,
     * instead it should be called from loop() method.
     * This can be called every loop() since nothing will be done if there's no
     * available log entry to write to file system.
     */
    void writeToFile();
};

#endif  // __LOGGER_H__
