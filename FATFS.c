/*******************************************************************************
* Include
*******************************************************************************/

#include "FATFS.h"

/*******************************************************************************
* Prototypes
*******************************************************************************/

/*******************************************************************************
* Code
*******************************************************************************/
extern FATFS_BootInfor_Struct_t g_bootSector;

Status FATFS_CreateNode(FATFS_EntryList_Struct_t **node, FATFS_Entry_Struct_t *buffer)
{
    Status status = ADD_SUCCESSFULLY;

    (*node) = (FATFS_EntryList_Struct_t *)malloc(sizeof(FATFS_EntryList_Struct_t));
    if((*node) == NULL)
    {
        status = NOT_ENOUGH_MEMORY;
    }
    else
    {
        /* Next = NULL */
        (*node)->next = NULL;
        /* Save data */
        memcpy((*node)->entry.fileName, buffer->fileName, 11);
        (*node)->entry.attribute = buffer->attribute;
        memcpy((*node)->entry.reserved, buffer->reserved, 11);
        (*node)->entry.timeStamp = buffer->timeStamp;
        (*node)->entry.dateStamp = buffer->dateStamp;
        (*node)->entry.accessDate = buffer->accessDate;
        (*node)->entry.clusterHight = buffer->clusterHight;
        (*node)->entry.editTime = buffer->editTime;
        (*node)->entry.editDate = buffer->editDate;
        (*node)->entry.clusterLow = buffer->clusterLow;
        (*node)->entry.fileSize = buffer->fileSize;
    }

    return status;
}

void FATFS_AddEntry(FATFS_EntryList_Struct_t **head, FATFS_EntryList_Struct_t *nodeAdd)
{

    FATFS_EntryList_Struct_t *count = NULL;

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

FATFS_EntryList_Struct_t * FreeLinkedList(FATFS_EntryList_Struct_t *head)
{
    FATFS_EntryList_Struct_t *temp = NULL;

    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
    return head;
}

FATFS_EntryList_Struct_t *ReadDirectory(uint32_t startSector, FATFS_EntryList_Struct_t *head)
{
    uint8_t *buffer = NULL;
    uint8_t *offset = NULL;
    FATFS_EntryList_Struct_t *entry = NULL;
    uint32_t i = 0;

    buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector);
    HAL_ReadSector(startSector, buffer);
    offset = &buffer[i];
    while (offset[0] != 0)
    {
        if(offset[0x0B] == 0x00 || offset[0x0B] == 0x10)
        {
            if(offset[0x00] == 0x2E)
            {
                if(offset[0x01] == 0x2E)
                {
                    FATFS_CreateNode(&entry, (FATFS_Entry_Struct_t*)offset);
                    FATFS_AddEntry(&head, entry);
                }
            }
            else
            {
                FATFS_CreateNode(&entry, (FATFS_Entry_Struct_t*)offset);
                FATFS_AddEntry(&head, entry);
            }
        }
        i = i + 32;
        offset = &buffer[i];
    }
    
    return head;
}

FATFS_EntryList_Struct_t * ChageDirectory(FATFS_EntryList_Struct_t *head, uint32_t cluster)
{
    system("cls");
    head = FreeLinkedList(head);
    if(cluster != 0)
    {
        head = ReadDirectory(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT + g_bootSector.entrysOfRDET/16 + g_bootSector.sectorsOfCluster * (cluster - 2) , head);
    }
    else
    {
        head = ReadDirectory(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT, head);
    }
    DisplayDirectory(head);

    return head;
}

void ReadFile(FATFS_EntryList_Struct_t *head, uint32_t cluster)
{
    uint8_t *buffer = NULL;
    uint32_t indexLine = 0;

    system("cls");
    buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector * g_bootSector.sectorsOfCluster);
    while (cluster != 4095)
    {
        HAL_ReadMultiSector(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT + g_bootSector.entrysOfRDET/16 + g_bootSector.sectorsOfCluster * (cluster - 2), g_bootSector.sectorsOfCluster, buffer);
        for(indexLine = 0; indexLine < (g_bootSector.sectorsOfCluster * g_bootSector.bytesOfSector); indexLine++)
        {
            printf("%c", buffer[indexLine]);
        }
        cluster = ReadFATValue(cluster);
    }
    printf("\n");
    system("pause");
    system("cls");
    DisplayDirectory(head);
}

FATFS_EntryList_Struct_t * ReadFileOrChangeDirectory(FATFS_EntryList_Struct_t *head, uint8_t select)
{
    uint8_t count = 1;
    FATFS_EntryList_Struct_t *temp = head;

    while (count != select)
    {
        temp = temp->next;
        count++;
    }
    if(temp->entry.attribute == 0x10)
    {
        head = ChageDirectory(head, temp->entry.clusterLow);
    }
    else
    {
        ReadFile(head, temp->entry.clusterLow);
    }
    
    return head;
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
            /* Print type */
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
