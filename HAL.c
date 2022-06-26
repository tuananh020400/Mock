/*******************************************************************************
* Include
*******************************************************************************/

#include "HAL.h"

/*******************************************************************************
* Prototypes
*******************************************************************************/

/**
 * @brief Read boot sector
 * 
 * @param[out] bootSector The data storage variable of the boot sector
 * @return StatusRead Notification of successful or failed file reading
 */
static StatusRead HAL_ReadBoot(FATFS_BootSector_Struct_t *bootSector);

/*******************************************************************************
* Variables
*******************************************************************************/

FILE *g_file;
FATFS_BootSector_Struct_t g_bootSector;

/*******************************************************************************
* Code
*******************************************************************************/

static StatusRead HAL_ReadBoot(FATFS_BootSector_Struct_t *bootSector)
{
    uint8_t index = 0;
    uint8_t *buffer = NULL;
    StatusRead readStatus = READ_FILE_SUCCESSFULLY;

    /* Allocation buffer array */
    buffer = (uint8_t*)malloc(512);
    if(buffer == NULL)
    {
        printf("NOT ENOUGH MEMORY!\n");
        readStatus = NOT_ENOUGH_MEMORY;
    }
    else
    {
        if(fread(buffer, 1 , 512, g_file) == 0)
        {
            readStatus = READ_FAILED;
            printf("FAIL TO READ FILE!\n");
        }
        /* Save data of boot sector */
        bootSector->bytesOfSector = (((uint16_t)buffer[0x0B + 1]) << 8) | ((uint16_t)buffer[0x0B]);
        bootSector->sectorsOfCluster = buffer[0x0D];
        bootSector->sectorsOfBoot = ((uint16_t)buffer[0x0E + 1] << 8) | ((uint16_t)buffer[0x0E]);
        bootSector->numOfFAT = buffer[0x10];
        bootSector->entrysOfRDET = (((uint16_t)buffer[0x11 + 1]) << 8) | ((uint16_t)buffer[0x11]);
        bootSector->sectorsOfVolume = ((uint16_t)buffer[0x13 + 1] << 8) | ((uint16_t)buffer[0x13]);
        bootSector->sectorsOfFAT = (((uint16_t)buffer[0x16 + 1]) << 8) | ((uint16_t)buffer[0x16]);
        for(index = 0; index < 8; index++)
        {
            bootSector->typeOfFAT[index] = buffer[index + 0x36];
        }
        /* Free buffer array */
        free(buffer);
    }

    return readStatus;
}

StatusRead HAL_Init(const char * filePath)
{
    StatusRead readStatus = READ_FILE_SUCCESSFULLY;

    /* Open stream to file */
    g_file = fopen(filePath, "rb");
    /* Cannot open stream */
    if(g_file == NULL)
    {
        printf("FAIL TO OPEN FILE!\n");
        readStatus = READ_FAILED;
    }
    /* Read boot sector */
    readStatus = HAL_ReadBoot(&g_bootSector);
    
    return readStatus;
}

uint32_t HAL_ReadSector(uint32_t index, uint8_t *buff)
{
    uint32_t byte = 0;
    /* Point to sector location */
    if(fseek(g_file, (uint32_t)(g_bootSector.bytesOfSector * index), SEEK_SET) != 0)
    {
        byte = 0;
    }
    /* Read sector */
    else
    {
        byte = fread(buff, 1, 512, g_file);
    }

    return byte;
}

uint32_t HAL_ReadMultiSector(uint32_t index, uint32_t num, uint8_t *buff)
{
    uint32_t byte = 0;
    
    /* Point to sector location */
    if(fseek(g_file, (uint32_t)(g_bootSector.bytesOfSector * index), SEEK_SET) != 0)
    {
        byte = 0;
    }
    /* Read sector */
    else
    {
        byte = fread(buff, 1, 512 * num, g_file);
    }

    return byte;
}

/*******************************************************************************
* End of file
*******************************************************************************/
