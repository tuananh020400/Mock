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
    uint8_t day;
    uint8_t month;
    uint8_t year;
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
} FATFS_TimeEntry_Struct_t;

typedef struct
{
    int8_t name[8];
    char type[3];
    uint32_t size;
    FATFS_TimeEntry_Struct_t timeEntry;
    uint32_t startCluster;
} FATFS_Entry_Struct_t;

typedef struct EntryList
{
    FATFS_Entry_Struct_t entry;
    struct EntryList *next;
} FATFS_EntryList_Struct_t;
typedef FATFS_EntryList_Struct_t *node;

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
node ReadDirectory(uint32_t startSector, node head);

node FreeLinkedList(node head);

void DisplayDirectory(node head);

uint32_t Elements(node head);

node ChageDirectory(node head, uint32_t cluster);

uint16_t ReadFATValue(uint16_t startCluster);

void ReadFile(node head, uint32_t cluster);

node ReadFileOrChangeDirectory(node head, uint8_t select);
#endif
