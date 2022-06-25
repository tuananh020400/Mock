#ifndef _HAL_H_
#define _HAL_H_

/*******************************************************************************
* Include
*******************************************************************************/

#include "header.h"

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
} FATFS_BootSector_Struct_t;

/*******************************************************************************
* API
*******************************************************************************/
uint32_t HAL_ReadSector(uint32_t index, uint8_t *buff);

uint32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff);

void HAL_Init(const char * filePath);

#endif /* _HAL_H */

/*******************************************************************************
* End of file
*******************************************************************************/
