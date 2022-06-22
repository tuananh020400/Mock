#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    uint16_t bytesOfSector;
    char pad;
    uint8_t sectorsOfCluster;
    char pad1;
    uint16_t sectorsOfBoot;
    uint8_t numOfFAT;
    char pad2;
    uint16_t entrysOfRDET;
    uint16_t sectorsOfVolume;
    uint32_t sectorsOfFAT;
    uint8_t typeOfFAT[8];
} BootSectorFAT12;

typedef struct{
    char name[8];
    char type[3];
    char pad[1];
    uint8_t day;
    uint8_t month;
    uint8_t year;
    char pad1[1];
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    char pad2[1];
    uint32_t size;
} Entry;

uint32_t HAL_ReadSector(uint32_t index, uint8_t *buff);

void ReadBootSector(BootSectorFAT12 *bootSector);
