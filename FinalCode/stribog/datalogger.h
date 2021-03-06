/*
 * FAT-32 SD Card Data Logger for ChibiOS
 * Author: Cody Hyman
 * TODO: Licensing info
 * 
 */

#ifndef _DATALOGGER_H_
#define _DATALOGGER_H_

#include <string.h>

#include "ch.h"
#include "hal.h"

#include "ff.h"

#include "sdmmcsimple.h"

#define NFILES 8

/**
 * @brief Data Logger
 */
struct datalogger
{
   sdmmc_t *sdc; 		// SD structure
   FATFS *filesys;		// Filesystem
   char *logPath; 		// Logfile path
   bool driveMounted;		// Drive mounted
   //logfile_t files[NFILES];	// Number of files
   uint16_t nfiles; 		// Number of logfiles open
};
typedef struct datalogger datalogger_t;

/**
 * @brief Logfile instance 
 */
struct logfile
{
    uint16_t id;		// Instance ID
    char *name; 		// Instance name
    datalogger_t *logger; 	// Logger parent
    FIL *file;			// File pointer
    uint32_t wrCount;		// Write counter
    bool open;			// Open flag
};
typedef struct logfile logfile_t;

/**
 * @brief Subclassed separated value log
 */
struct separatedLog
{
    uint16_t id;	// Instance ID
    char separator;	// Separator character
    uint16_t ncols;	// Number of columns
    logfile_t logfile;	// Parent items
};
typedef struct separatedLog separatedLog_t;

int8_t dataLoggerInitialize(datalogger_t *logger, char *logPath, sdmmc_t *sd, SerialDriver *dbg);
int8_t dataLoggerStop(datalogger_t *logger);
FRESULT dataLoggerFileCount(datalogger_t *logger, char *dirpath, uint16_t *count);

int8_t logfileNew(logfile_t *log, datalogger_t *logger, FIL *file, char *fname);
size_t logfileSize(logfile_t *log);

int8_t logfileWrite(logfile_t *log, char *buf, uint16_t length, bool openClose);
int8_t logfileWriteCsv(logfile_t *log, char ** items, char separator, uint16_t nitems);
uint32_t logfileGetWrCount(logfile_t *log);
int8_t logfileClose(logfile_t *log);
int8_t logfileOpenAppend(logfile_t *log);

#endif