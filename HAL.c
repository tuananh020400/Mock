#include "header.h"

FILE *file;
uint32_t HAL_ReadSector(uint32_t index, uint8_t *buff)
{
    uint32_t byte = 0;

    if(fseek(file, (long int)(512 * index), SEEK_SET) != 0)
    {
        byte = 0;
    }
    else
    {
        byte = fread(buff, 1, 512, file);
    }

    return byte;
}

uint32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff)
{
    uint32_t byte = 0;
    
    if(fseek(file, (long int)(512 * index), SEEK_SET) != 0)
    {
        byte = 0;
    }
    else
    {
        byte = fread(buff, 1, 512 * num, file);
    }

    return byte;
}

void ReadBootSector(BootSectorFAT12 *bootSector)
{
    uint8_t i = 0;
    uint8_t *buffer = NULL;

    buffer = (uint8_t*)malloc(512);
    if(buffer == NULL)
    {
        printf("Not enough memory!\n");
    }
    HAL_ReadMultiSector(0, 1, buffer);

    bootSector->bytesOfSector = (((uint16_t)buffer[0x0B + 1]) << 8) | ((uint16_t)buffer[0x0B]);
    bootSector->sectorsOfCluster = buffer[0x0D];
    bootSector->sectorsOfBoot = ((uint16_t)buffer[0x0E + 1] << 8) | ((uint16_t)buffer[0x0E]);
    bootSector->numOfFAT = buffer[0x10];
    bootSector->entrysOfRDET = (((uint16_t)buffer[0x11 + 1]) << 8) | ((uint16_t)buffer[0x11]);
    bootSector->sectorsOfVolume = ((uint16_t)buffer[0x13 + 1] << 8) | ((uint16_t)buffer[0x13]);
    bootSector->sectorsOfFAT = (((uint16_t)buffer[0x16 + 1]) << 8) | ((uint16_t)buffer[0x16]);
    for(i = 0; i < 8; i++)
    {
        bootSector->typeOfFAT[i] = buffer[i + 0x36];
    }
//    printf("Number of bytes in a sector: %d\n", bootSector->bytesOfSector);
//    printf("Number of sectors in a cluster: %d\n", bootSector->sectorsOfCluster);
//    printf("Number of sectors in the Boot sector: %d\n", bootSector->sectorsOfBoot);
//    printf("Number of file allocation table: %d\n", bootSector->numOfFAT);
//    printf("Number of entry in Root Directory Entry Table : %d\n", bootSector->entrysOfRDET);
//    printf("Number of sectors in a volume: %d\n", bootSector->sectorsOfVolume);
//    printf("Number of sectors in a file allocation table: %d\n", bootSector->sectorsOfFAT);
//    printf("Name of FAT: %s\n", bootSector->typeOfFAT);
    printf("---------------------------------------------------------------------------------------\n");
}

