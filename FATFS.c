/*******************************************************************************
* Include
*******************************************************************************/

#include "FATFS.h"

/*******************************************************************************
* Prototypes
*******************************************************************************/

/*******************************************************************************
* Variables
*******************************************************************************/

extern FATFS_BootInfor_Struct_t g_bootSector;

static Status ReadFATValue(uint32_t *startCluster);

/*******************************************************************************
* Code
*******************************************************************************/

Status FATFS_AddEntry(FATFS_EntryList_Struct_t **head, FATFS_Entry_Struct_t *buffer)
{
    Status status = SUCCESSFULLY;
    FATFS_EntryList_Struct_t *nodeAdd = NULL;
    FATFS_EntryList_Struct_t *count = NULL;

    /* Allocate a node */
    nodeAdd = (FATFS_EntryList_Struct_t *)malloc(sizeof(FATFS_EntryList_Struct_t));
    if((nodeAdd) == NULL)
    {
        status = NOT_ENOUGH_MEMORY;
    }
    else
    {
        /* Save data */
        memcpy((nodeAdd)->entry.fileName, buffer->fileName, 11);
        (nodeAdd)->entry.attribute = buffer->attribute;
        memcpy((nodeAdd)->entry.reserved, buffer->reserved, 11);
        (nodeAdd)->entry.timeStamp = buffer->timeStamp;
        (nodeAdd)->entry.dateStamp = buffer->dateStamp;
        (nodeAdd)->entry.accessDate = buffer->accessDate;
        (nodeAdd)->entry.clusterHight = buffer->clusterHight;
        (nodeAdd)->entry.editTime = buffer->editTime;
        (nodeAdd)->entry.editDate = buffer->editDate;
        (nodeAdd)->entry.clusterLow = buffer->clusterLow;
        (nodeAdd)->entry.fileSize = buffer->fileSize;
        nodeAdd->next = NULL;
        /* Add nodeAdd to the end of linked list */
        if ((*head) == NULL)
        {
            *head = nodeAdd;
        }
        else
        {
            count = *head;
            while (count->next != NULL)
            {
                count = count->next;
            }
            count->next = nodeAdd;
        }
    }

    return status;
}

static Status FATFS_ReadRootDirectory(FATFS_EntryList_Struct_t **head)
{
    uint8_t *buffer = NULL;
    uint8_t *offset = NULL;
    uint32_t index = 0;
    Status status = SUCCESSFULLY;

    buffer = (uint8_t*)malloc(32 * g_bootSector.entrysOfRDET);
    if(buffer == NULL)
    {
        status = NOT_ENOUGH_MEMORY;
    }
    else
    {
        if(HAL_ReadMultiSector((g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT), \
        ((g_bootSector.entrysOfRDET * 32) / g_bootSector.bytesOfSector), buffer) != (g_bootSector.entrysOfRDET * \
        g_bootSector.bytesOfSector * 32) / g_bootSector.bytesOfSector)
        {
            status = READ_FAILED;
        }
        else
        {
            /* Add entry */
            offset = &buffer[index];
            while (offset[0] != 0 && status == SUCCESSFULLY)
            {
                if(offset[0x0B] == 0x00 || offset[0x0B] == 0x10) /* Main entry */
                {
                    status = FATFS_AddEntry(head, (FATFS_Entry_Struct_t *)offset);
                }
                /* Shift offset to next entry */
                index = index + 32;
                offset = &buffer[index];
            }
        }
        free(buffer);
    }

    return status;
}

static Status FATFS_ReadSubDirectory(FATFS_EntryList_Struct_t **head, uint32_t startCluster)
{
    uint8_t *buffer = NULL;
    uint8_t *offset = NULL;
    uint32_t index = 0;
    Status status = SUCCESSFULLY;

    buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector * g_bootSector.sectorsOfCluster);
    if(buffer == NULL)
    {
        status = NOT_ENOUGH_MEMORY;
    }
    else
    {
        while (startCluster != 0xFFF && status == SUCCESSFULLY)
        {
            /* Read Cluster */
            if(HAL_ReadMultiSector((g_bootSector.sectorsOfBoot) + (g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT) + \
            ((g_bootSector.entrysOfRDET * 32) / g_bootSector.bytesOfSector) + (g_bootSector.sectorsOfCluster * (startCluster - 2)),\
            g_bootSector.sectorsOfCluster, buffer) != (g_bootSector.sectorsOfCluster * g_bootSector.bytesOfSector))
            {
                status = READ_FAILED;
            }
            else
            {
                index = 0;
                offset = &buffer[index];
                while (offset[0] != 0 && status == SUCCESSFULLY)
                {
                    /* Add Entry */
                    if(offset[0x0B] == 0x00 || offset[0x0B] == 0x10)
                    {
                        /* Add .. */
                        if(offset[0x00] == 0x2E)
                        {
                            if(offset[0x01] == 0x2E)
                            {
                                status = FATFS_AddEntry(head, (FATFS_Entry_Struct_t *)offset);
                            }
                        }
                        /* Add file and directory */
                        else
                        {
                            status = FATFS_AddEntry(head, (FATFS_Entry_Struct_t *)offset);
                        }
                    }
                    index = index + 32;
                    offset = &buffer[index];
                }
                status = ReadFATValue(&startCluster);
            }
        }
        free(buffer);
    }
    
    return status;
}

Status ReadDirectory(uint32_t startCluster, FATFS_EntryList_Struct_t **head)
{
    Status status = SUCCESSFULLY;
    FATFS_EntryList_Struct_t *temp = NULL;

    /* Free linked list */
    while ((*head) != NULL)
    {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
    /* ReadDirectory */
    if(startCluster == 0)
    {
        status = FATFS_ReadRootDirectory(head);
    }
    else
    {
        status = FATFS_ReadSubDirectory(head, startCluster);
    }

    return status;
}

Status ReadFile(uint32_t startCluster, uint32_t size, uint8_t *buffer)
{
    uint8_t *bufferCluster = NULL;
    uint32_t indexCluster = 0;
    uint32_t indexBuffer = 0;
    Status status = SUCCESSFULLY;

    bufferCluster = (uint8_t*)malloc(g_bootSector.bytesOfSector * g_bootSector.sectorsOfCluster);
    if(bufferCluster == NULL)
    {
        status = NOT_ENOUGH_MEMORY;
    }
    else
    {
        while ((startCluster != 4095) && (indexBuffer < size) && (status == SUCCESSFULLY))
        {
            indexCluster = 0;
            if(HAL_ReadMultiSector(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT + \
            (g_bootSector.entrysOfRDET * 32) / (g_bootSector.bytesOfSector) + g_bootSector.sectorsOfCluster * (startCluster - 2),\
            g_bootSector.sectorsOfCluster, bufferCluster) != (g_bootSector.sectorsOfCluster * g_bootSector.bytesOfSector))
            {
                status = READ_FAILED;
            }
            while(indexCluster < g_bootSector.bytesOfSector * g_bootSector.sectorsOfCluster && (indexBuffer < size))
            {
                buffer[indexBuffer] = bufferCluster[indexCluster];
                indexBuffer++;
                indexCluster++;
            }
            status = ReadFATValue(&startCluster);
        }
        free(bufferCluster);
    }

    return status;
}

static Status ReadFATValue(uint32_t *startCluster)
{
    uint16_t sectorOfFatValue = 0;
    uint16_t byteOfFatValue = 0;
    uint32_t FATValue = 0;
    uint8_t *buffer = NULL;
    Status status = SUCCESSFULLY;

    sectorOfFatValue = ((*startCluster) * g_bootSector.sectorsOfCluster * 1.5) / g_bootSector.bytesOfSector;
    byteOfFatValue = (uint32_t)((*startCluster) * g_bootSector.sectorsOfCluster * 1.5) % (uint32_t)g_bootSector.bytesOfSector;
    /* If position of FAT: Sector1[end] & Sector2[start] -> Cannot read FAT if read 1 sector */
    buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector * 2);
    if(buffer == NULL)
    {
        status = NOT_ENOUGH_MEMORY;
    }
    else
    {
        if(HAL_ReadMultiSector(g_bootSector.sectorsOfBoot + sectorOfFatValue, 2, buffer) != (g_bootSector.bytesOfSector * 2))
        {
            status = READ_FAILED;
        }
        else
        {
            if((*startCluster) % 2 == 0) /* Start cluster is even number */
            {
                FATValue = (((uint32_t)(buffer[byteOfFatValue + 1] &~ (0xF0))) << 8) | (uint32_t)(buffer[byteOfFatValue]);
            }
            else /* Start cluster is odd number */
            {
                FATValue = ((uint32_t)(buffer[byteOfFatValue + 1]) <<  4) | (uint32_t)(buffer[byteOfFatValue] >> 4);
            }
        }
        free(buffer);
    }
    (*startCluster) = FATValue;

    return status;
}



