#include "header.h"

uint32_t PrintData(uint32_t fat)
{
    uint8_t *bufferData = NULL;
    bufferData = (uint8_t*)malloc(512);
    uint32_t sectorFAT = (fat * 1.5) / 512;
    printf("Sector FAT: %d\n", sectorFAT);
    uint32_t byteFat = (uint32_t)(fat * 1.5) % 512;
    printf("Byte FAT: %d\n", byteFat);
    return 0;
}

void ReadFile(Entry entry)
{
    uint8_t *bufferFat = NULL;
    bufferFat = (uint8_t*)malloc(512);
    uint32_t fat = entry.startCluster;

    PrintData(fat);
}


