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
                startCluster = ReadFATValue(startCluster);
            }
        }
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

void ReadFile(uint32_t startCluster, uint32_t size, uint8_t *bufferPrint)
{
    uint8_t *buffer = NULL;
    uint32_t indexLine = 0;
    uint32_t index = 0;

    buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector * g_bootSector.sectorsOfCluster);
    while ((startCluster != 4095) && (index < size))
    {
        indexLine = 0;
        HAL_ReadMultiSector(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT + g_bootSector.entrysOfRDET/16 + g_bootSector.sectorsOfCluster * (startCluster - 2), g_bootSector.sectorsOfCluster, buffer);
        while(indexLine < g_bootSector.bytesOfSector * g_bootSector.sectorsOfCluster && (index < size))
        {
            //printf("%c", buffer[indexLine]);
            bufferPrint[index] = buffer[indexLine];
            index++;
            indexLine++;
            //printf("index: %d - %d\n", index, indexLine);
        }
        //printf("%d\n", cluster);
        startCluster = ReadFATValue(startCluster);
    }
    free(buffer);
}

uint16_t ReadFATValue(uint16_t startCluster)
{
    uint16_t sectorOfFatValue = 0;
    uint16_t byteOfFatValue = 0;
    uint32_t FATValue = 0;
    uint8_t *buffer = NULL;

    sectorOfFatValue = (startCluster * g_bootSector.sectorsOfCluster * 1.5) / g_bootSector.bytesOfSector;
    byteOfFatValue = (int)(startCluster * g_bootSector.sectorsOfCluster * 1.5) % (int)g_bootSector.bytesOfSector;
    if(byteOfFatValue == g_bootSector.bytesOfSector - 1)
    {
        buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector * 2);
        HAL_ReadMultiSector(g_bootSector.sectorsOfBoot + sectorOfFatValue, 2, buffer);
    }
    else
    {
        buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector);
        HAL_ReadSector(g_bootSector.sectorsOfBoot + sectorOfFatValue, buffer);
    }
    if(startCluster % 2 == 0)
    {
        FATValue = (((uint32_t)(buffer[byteOfFatValue + 1] &~ (0xF0))) << 8) | (uint32_t)(buffer[byteOfFatValue]);
    }
    else
    {
        FATValue = ((uint32_t)(buffer[byteOfFatValue + 1]) <<  4) | (uint32_t)(buffer[byteOfFatValue] >> 4);
    }
    free(buffer);
    
    return FATValue;
}

void DisplayDirectory(FATFS_EntryList_Struct_t *head)
{
    uint8_t no = 1;
    FATFS_EntryList_Struct_t *count = NULL;
    
    printf("---------------------------------------------------------------------------------------\n");
    printf("%-4s%-15s%-9s%-25s%-17s%-10s\n", "No", "Name", "Size", "Type", "Day Modified", "Time Modified");
    printf("---------------------------------------------------------------------------------------\n");
    for(count = head; count != NULL; count = count->next)
    {
        if(count->entry.fileName[0] == 0x2E)
        {
            printf("%-4d< BACK\n\n", no);
        }
        else
        {
            printf("%-4d", no);
            /* Print name */
                printf("%s - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %x - %d \n", count->entry.fileName,count->entry.attribute, count->entry.reserved[0], count->entry.reserved[1], count->entry.timeStamp, count->entry.dateStamp\
                , count->entry.accessDate, count->entry.clusterHight, count->entry.editTime, count->entry.editDate, count->entry.editTime, count->entry.clusterLow, count->entry.fileSize);

        }
        no++;
    } 
}

uint32_t Elements(FATFS_EntryList_Struct_t *head)
{
    uint32_t elements = 0;
    FATFS_EntryList_Struct_t *temp = head;

    if(head != NULL)
    {
        while (temp != NULL) /* Browse all element */
        {
            temp = temp->next;
            elements++;
        }
    }
    return elements;
}

uint8_t CheckSelect(FATFS_EntryList_Struct_t *head, uint32_t position)
{
    uint8_t check = 0;
    uint32_t elements = Elements(head);

    if(position < 1 || position > elements) /* Limit input position 1 - numbers element of list */
    {
        check++;
        printf("Error: Input position is not in range 1 - %d!\n", elements);
    }
    return check;
}


