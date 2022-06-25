#ifndef _FATFS_H_
#define _FATFS_H_
/*******************************************************************************
* Include
*******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
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
    uint16_t startCluster;
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

node ReadDirectory(uint32_t startSector, node head);

node FreeLinkedList(node head);

void Print(node head);

uint32_t Elements(node head);

node ChageDirectory(node head, uint8_t select);
#endif
