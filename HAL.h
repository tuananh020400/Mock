#ifndef _HAL_H_
#define _HAL_H_

/*******************************************************************************
* Include
*******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/*******************************************************************************
* Definition
*******************************************************************************/

typedef struct{
    uint16_t bytesOfSector;
    uint8_t sectorsOfCluster;
    uint16_t sectorsOfBoot;
    uint8_t numOfFAT;
    uint16_t entrysOfRDET;
    uint16_t sectorsOfVolume;
    uint32_t sectorsOfFAT;
    int8_t typeOfFAT[8];
} FATFS_BootInfor_Struct_t;

typedef enum
{
    READ_FAILED = 0,
    NOT_ENOUGH_MEMORY = 1,
    SUCCESSFULLY = 2,
} Status;

/*******************************************************************************
* API
*******************************************************************************/

/**
 * @brief Initialize the reading process
 * 
 * @param[in] filePath The path to the file
 * @return Status Notification of successful or failed file reading
 */
Status HAL_Init(const char *filePath);

/**
 * @brief Read data of a sector
 * 
 * @param[in] index Position of sector
 * @param[inout] buff Data storage array
 * @return uint32_t Number of bytes read
 */
int32_t HAL_ReadSector(uint32_t index, uint8_t *buff);

/**
 * @brief Read data of multiple sector
 * 
 * @param[in] index Position of first sector
 * @param[in] num Number of sectors need to read
 * @param[inout] buff Data storage array
 * @return uint32_t Number of bytes read
 */
int32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff);


#endif /* _HAL_H */

/*******************************************************************************
* End of file
*******************************************************************************/

