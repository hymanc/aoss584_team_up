/**
 * Simple FATFS Data-Logger for ChibiOS
 *
 *
 * @author Cody hyman
 *
 */

#include "datalogger.h"
#include "chprintf.h"

static SerialDriver *DEBUG; // Debug serial port

/**
 * @brief Initializes a Data Logger instance
 * @param logger Data logger struct
 * @param logPath Path to store logs to
 * @param sd SD Card container
 * @param dbg Debug Serial Port
 * @return Success state of data logger initialization
 */
int8_t dataLoggerInitialize(datalogger_t *logger, char *logPath, sdmmc_t *sd, SerialDriver *dbg)
{
    if(sd == NULL || logger == NULL)
	   return false;
    DEBUG = dbg;
    if(logPath == NULL)
	   logPath = "";
    FRESULT err;
    logger->sdc = sd;
    logger->filesys = sdmmcGetFS(sd);
    logger->logPath = logPath;
    err = f_mount(logger->filesys, logPath, 1);
    if(err != FR_OK)
    {
    	chprintf((BaseSequentialStream *) DEBUG, "DATALOGGER: Error Mounting Filesystem,ERR%20d\n",err);
    	return -1;
    }
    logger->driveMounted = true;
    chprintf((BaseSequentialStream *) DEBUG, "DATALOGGER: Datalogger Started\n");
    return 0;
}

/**
 * @brief Stops an ongoing data logging process
 * @param logger Data logger struct
 * @return 
 */
int8_t dataLoggerStop(datalogger_t *logger)
{
    // TODO: Close all files
    chprintf((BaseSequentialStream *) DEBUG, "DATALOGGER: Datalogger Stopped\n");
    f_mount(NULL, NULL, 0);
    logger->driveMounted = false;
    return 0;
}

/**
 *
 *
 */
FRESULT dataLoggerFileCount(datalogger_t *logger, char *dirpath, uint16_t *count)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    uint16_t fcount = 0;

    res = f_opendir(&dir, dirpath);
    if(res == FR_OK)
    {
        while(fcount < 32768)
        {
            res = f_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == NULL)
                break;
            fcount++;
        }
        *count = fcount;
        return FR_OK;
    }
    else
        return res;
}

/**
 * @brief Creates a new logfile instance
 */
int8_t logfileNew(logfile_t *log, datalogger_t *logger, FIL *file, char *fname)
{
    chprintf((BaseSequentialStream *) DEBUG, "DATALOGGER: Creating New Logfile %s\n", fname);
    FRESULT res;
    log->open = FALSE;
    if(!(logger->driveMounted))
    {
       chprintf((BaseSequentialStream *) DEBUG, "DATALOGGER: ERROR Drive not mounted\n");
	   return false;
    }
    log->file = file;
    log->wrCount = 0;
    log->name = fname;
    //chThdSleepMilliseconds(100);
    res = f_open(log->file, fname, FA_CREATE_ALWAYS | FA_WRITE);
    //chprintf((BaseSequentialStream *) DEBUG, "DATALOGGER: FILE OPEN\n");
    //chThdSleepMilliseconds(100);
    if(res)
    {
    	chprintf((BaseSequentialStream *) DEBUG, "DATALOGGER: Error creating Logfile, ERR%02d\n",res);
    	return res;
    }
    log->open = TRUE;
    return 0;
}

/**
 * @brief Checks the size of a logfile
 */
size_t logfileSize(logfile_t *log)
{
    // TODO: Get file size
    return 0;
}

/**
 * @brief Writes a buffer to a logfile
 * @param log
 * @param buf
 * @param length
 * @param openClose
 */
int8_t logfileWrite(logfile_t *log, char *buf, uint16_t length, bool openClose)
{
    FRESULT res;
    int written;
    if(openClose)
    {
    	res = logfileOpenAppend(log);
    	if(res)
    	    return res;
    }
    res = f_write(log->file, buf, length, &written);
    if(res || (written != length))
	   return 1;
    log->wrCount++;
    if(openClose)
    {
    	res = f_close(log->file);
    	if(res)
    	    return res;
    }
    return 0;
}

/**
 * @brief Writes a separated value list to a logfile
 * @param log Logfile instance to write to
 * @param items String items
 * @param separator Separator character
 * @param nitems Total number of items to write
 */
int8_t logfileWriteCsv(logfile_t *log, char **items, char separator, uint16_t nitems)
{
    FRESULT res;
    // Format CSV line
    char * buf;
    uint32_t length;
    int written;
    uint16_t i, j;
    for(i = 0; i < nitems; i++)
    {
    	j = 0;
    	while(items[i][j] != '\0')
    	{
    	    buf[length++] = items[i][j++];
    	}
    	buf[length++] = separator;
    }
    // TODO: Write CSV line to buffer
    //res = f_write(log->file, bufLen, &written);
    //if(res || (written != length))
	//return 1;
    return 0;
}

/**
 * @brief Accessor for logfile write count
 * @param log Logfile container
 * @return Number of lines written to logfile
 */
uint32_t logfileGetWrCount(logfile_t *log)
{
    return log->wrCount;
}

/**
 * @brief Closes a logfile
 * @param log Logfile to close
 * @return Success status
 */
int8_t logfileClose(logfile_t *log)
{
    if(f_close(log->file) == FR_OK)
    {
    	log->wrCount = 0;
    	log->open = FALSE;
    	return FR_OK;
    }
    return -1;
}

/**
 * @brief Opens a logfile and seeks to the end for appending
 * @param log Logfile container
 * @return Status of successful open
 */
int8_t logfileOpenAppend(logfile_t *log)
{
    FRESULT res;
    res = f_open(log->file, log->name, FA_WRITE | FA_OPEN_ALWAYS);
    if(res == FR_OK)
    {
    	res = f_lseek(log->file, f_size(log->file));
    	if(res != FR_OK)
    	    f_close(log->file);
    }
    return res;
}