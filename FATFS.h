#ifndef _FATFS_H_
#define _FATFS_H_
/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "HAL.h"

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

typedef struct
{
    int8_t fileName[11];
    uint8_t attribute;
    uint8_t reserved[2];
    uint16_t timeStamp;
    uint16_t dateStamp;
    uint16_t accessDate;
    uint16_t clusterHight;
    uint16_t editDate;
    uint16_t editTime;
    uint16_t clusterLow;
    uint32_t fileSize;
} FATFS_Entry_Struct_t;

typedef struct EntryList
{
    FATFS_Entry_Struct_t entry;
    struct EntryList *next;
} FATFS_EntryList_Struct_t;

typedef enum
{
    READ_FAILED = 0,
    NOT_ENOUGH_MEMORY = 1,
    SUCCESSFULLY = 2,
} Status;
/*******************************************************************************
* API
*******************************************************************************/
void DisplayDirectory(FATFS_EntryList_Struct_t *head);

uint16_t ReadFATValue(uint16_t startCluster);

Status ReadDirectory(uint32_t startCluster, FATFS_EntryList_Struct_t **head);

#endif

