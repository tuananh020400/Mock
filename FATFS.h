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

/*******************************************************************************
* Prototypes
*******************************************************************************/

/**
 * @brief 
 * 
 * @param startSector 
 * @param head 
 * @return node 
 */
FATFS_EntryList_Struct_t * ReadDirectory(uint32_t startSector, FATFS_EntryList_Struct_t *head);

void FreeLinkedList(FATFS_EntryList_Struct_t **head);

void DisplayDirectory(FATFS_EntryList_Struct_t *head);

uint32_t Elements(FATFS_EntryList_Struct_t *head);

FATFS_EntryList_Struct_t * ChageDirectory(FATFS_EntryList_Struct_t *head, uint32_t cluster);

uint16_t ReadFATValue(uint16_t startCluster);

void ReadFile(FATFS_EntryList_Struct_t *head, uint32_t cluster);

FATFS_EntryList_Struct_t * ReadFileOrChangeDirectory(FATFS_EntryList_Struct_t *head, uint8_t select);

uint8_t CheckSelect(FATFS_EntryList_Struct_t *head, uint32_t position);

#endif
