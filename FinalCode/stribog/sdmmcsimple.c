#include "sdmmcsimple.h"
#include "chprintf.h"
#include "evtimer.h"

/**
 * @brief Opaque SD card struct
 */
/*
struct sdmmc
{
    MMCDriver *mmcd;	// HAL MMC-SPI Driver
    SPIDriver *spid;	// HAL SPI Driver
    FATFS *filesys;	// FAT Filesystem
    bool mmcReady; 	// MMC Driver ready flag
    bool fsReady;	// Filesystem ready flag    
};*/

static sdmmc_t *SD;

static SerialDriver *serialPort;

static FATFS FS;

/** Low Speed SPI Configuration */
static SPIConfig lsCfg =
{
    NULL,
    SD_CS_PORT,
    SD_CS_PIN,
    SPI_CR1_BR_2 | SPI_CR1_BR_1
};

/** High Speed SPI Configuration */
static SPIConfig hsCfg =
{
    NULL,
    SD_CS_PORT,
    SD_CS_PIN,
    SPI_CR1_BR_1
};

static MMCConfig mmcCfg =
{
    &SD_SPID,
    &lsCfg,     // Low-speed configuration
    &hsCfg      // High speed configuration
};
    
/**
 * @brief Checks if an SD card is inserted
 */

bool sd_is_card_inserted(MMCDriver *mmcd)
{
    (void) mmcd;
    if(palReadPad(SD_CD_PORT, SD_CD_PIN) == PAL_LOW)
	   return TRUE;
    return FALSE;
}

/**
 * @brief Checks if an SD write protect flag is on
 */

bool sd_is_write_protected(MMCDriver *mmcd)
{
    (void) mmcd;
    return FALSE;
    //return palReadPad(SD_WP_PORT, SD_WP_PIN);
}


/**
 * @brief Initializes an SD card
 */
int8_t sdmmcInitialize(sdmmc_t *sd, MMCDriver *mld, SerialDriver *sp)
{    
    serialPort = sp;
    // Initialze SD object
    sd->mmcd = mld;
    sd->spid = &SD_SPID;
    sd->filesys = NULL;
    sd->fsReady = false;
    
    SD = sd;
    
    int8_t status;
    	FRESULT err;

    mmcObjectInit(sd->mmcd);
    mmcStart(sd->mmcd, &mmcCfg);

    // Try to connect
    if(sd_is_card_inserted(sd->mmcd))
    {
    	chprintf((BaseSequentialStream *) serialPort, "SD/MMC:Card Found\n");
    	palClearPad(SD_CS_PORT, SD_CS_PIN);
    	chThdSleepMilliseconds(200); // wait
    	chprintf((BaseSequentialStream *) serialPort, "SD/MMC:Connecting\n");
    	status = mmcConnect(sd->mmcd);
    	chprintf((BaseSequentialStream *) serialPort, "SD/MMC:End of Connection attempt\n");
	if(status == HAL_FAILED)
	{
	    chprintf((BaseSequentialStream *) serialPort, "SD/MMC:ERROR Connect,ERR%02d\n", status);
	    return status;
	}
	chprintf((BaseSequentialStream *) serialPort, "SD/MMC: Connection Done\n");
	sd->filesys = &FS;
	err = f_mount(sd->filesys, "/", 0);
	if(err != FR_OK)
	{
	    chprintf((BaseSequentialStream *) serialPort, "SD/MMC:File System Mount Error\n");
	    mmcDisconnect(sd->mmcd);
	    sd->filesys = NULL;
	    return -2;
	}
	else
	    chprintf((BaseSequentialStream *) serialPort, "SD/MMC:File System Mounted\n");
	sd->fsReady = true;
    }
    else
    {
    	chprintf((BaseSequentialStream *) serialPort, "SD/MMC:No SD Card Found\n");
    	return -3;
    }
    
    return 0;
}

/**
 * @brief Accessor to the HAL MMC driver
 * @param sd SD/MMC card object to check
 * @return Pointer to MMC driver, NULL if initialization incomplete
 */
MMCDriver * sdmmGetMMCDriver(sdmmc_t *sd)
{
    if(sd->mmcReady)
	return sd->mmcd;
    return NULL;
}

/**
 * @brief Accessor to the FAT filesystem object
 * @param sd SD/MMC card object to check
 * @return Pointer to SD/MMC filesystem, returns NULL if not mounted
 */
FATFS * sdmmcGetFS(sdmmc_t *sd)
{
    if(sd->fsReady)
	return sd->filesys; 
    return NULL;
}

/**
 * @brief Returns the filesystem mounting status
 * @param sd SD/MMC card object to check
 * @return Status of FAT file system being mounted
 */
bool sdmmcFSMounted(sdmmc_t *sd)
{
   return sd->fsReady;  
}