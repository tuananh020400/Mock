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
extern FILE *g_file;
extern FATFS_BootSector_Struct_t g_bootSector;

node CreateNode(uint8_t *buffer)
{
    node temp;
    uint8_t nameCount = 0;
    uint8_t typeCount = 0;

    temp = (node)malloc(sizeof(FATFS_EntryList_Struct_t));
    temp->next = NULL;
    for (nameCount = 0; nameCount < 8; nameCount++)
    {
        temp->entry.name[nameCount] = buffer[nameCount];
    }
    for (typeCount= 0; typeCount < 3; typeCount++)
    {
        temp->entry.type[typeCount] = buffer[0x08 + typeCount];
    }
    temp->entry.size = ((uint32_t)buffer[0x1C]) | ((uint32_t)buffer[1 + 0x1C] << 8) | \
    ((uint32_t)buffer[2 + 0x1C] << 16) | ((uint32_t)buffer[3 + 0x1C] << 24); 
    temp->entry.timeEntry.day = buffer[0x18] & (~0b11100000);
    temp->entry.timeEntry.month = ((buffer[0x18] & (~0b00011111)) >> 5) | ((buffer[0x18 + 0x01] & (~0b11111110)) << 3);
    temp->entry.timeEntry.year = ((buffer[0x18 + 0x01] & (~0b00000001)) >> 1);
    temp->entry.timeEntry.second = buffer[0x16] & (~0b11100000);
    temp->entry.timeEntry.minute = ((buffer[0x16] & (~0b00011111)) >> 5) | ((buffer[0x16 + 0x01] & (~0b11111000)) << 3);
    temp->entry.timeEntry.hour = (buffer[0x16 + 0x01] & (~0b00000111)) >> 3;
    temp->entry.startCluster = (uint16_t)(buffer[0x1A]) | ((uint16_t)(buffer[0x1B]) << 8);
    return temp;
}

uint32_t Elements(node head)
{
    uint32_t elements = 0;
    node temp = head;

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

node AddEntry(node head, uint8_t *buffer)
{
    node temp = CreateNode(buffer);
    node count = NULL;

    if (head == NULL)
    {
        head = temp;
    }
    else
    {
        count = head;
        while (count->next != NULL)
        {
            count = count->next;
        }
        count->next = temp;
    }
    return head;
}

node ReadDirectory(uint32_t startSector, node head)
{
    uint8_t *buffer = NULL;
    uint8_t *offset = NULL;
    uint32_t i = 0;

    buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector);
    HAL_ReadSector(startSector, buffer);
    offset = &buffer[i];
    while (offset[0] != 0)
    {
        if(offset[0x0B] == 0x00 || offset[0x0B] == 0x10)
        head = AddEntry(head, offset);
        i = i + 32;
        offset = &buffer[i];
    }
    return head;
}

node FreeLinkedList(node head)
{
    node temp = NULL;

    while (head != NULL)
    {
        temp = head;
        head = head->next;
        free(temp);
    }
    return head;
}

uint8_t CheckSelect(node head, uint32_t position)
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

node ChageDirectory(node head, uint8_t select)
{
    system("cls");
    uint8_t count = 1;
    node temp = head;
    uint16_t cluster;

    while (count != select)
    {
        temp = temp->next;
        count++;
    }
    cluster = temp->entry.startCluster;
    head = FreeLinkedList(head);
    if(cluster != 0)
    {
        head = ReadDirectory(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT + g_bootSector.entrysOfRDET/16 + g_bootSector.sectorsOfCluster * (cluster - 2) , head);
    }
    else
    {
        head = ReadDirectory(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT, head);
    }
    Print(head);
    return head;
}

uint16_t ReadFAT(uint16_t startCluster)
{
    uint16_t clusterOfFatValue;
    uint16_t byteOfFatValue;
    uint8_t *buffer = NULL;

    buffer = (uint8_t*)malloc(g_bootSector.bytesOfSector);

    sectorOfFatValue = (startCluster * g_bootSector.sectorsOfCluster * 1.5) / g_bootSector.bytesOfSector;
    byteOfFatValue = (startCluster * g_bootSector.sectorsOfCluster * 1.5) % g_bootSector.bytesOfSector;
    ReadDirectory();
}

void Print(node head)
{
    uint8_t no = 0;
    node count = NULL;
    
    printf("%-4s%-15s%-9s%-25s%-17s%-10s\n", "No", "Name", "Size", "Type", "Day Modified", "Time Modified");
    printf("---------------------------------------------------------------------------------------\n");
    for(count = head; count != NULL; count = count->next)
    {
        no++;
        printf("%-4d", no);
        /* Print name */
        for(int i = 0; i < 8; i++)
        {
            printf("%c", count->entry.name[i]);
        }
        /* Print type */
        if(strcmp(count->entry.type, "TXT") == 0)
        {
            printf(".");
            for(int i = 0; i < 3; i++)
            {
                printf("%c", count->entry.type[i]);
            }
            printf("%-3s%-9d", "", count->entry.size);
            printf("%-20s", "Text Document");
        }
        else if(strcmp(count->entry.type, "DOC") == 0)
        {
            printf(".");
            for(int i = 0; i < 3; i++)
            {
                printf("%c", count->entry.type[i]);
            }
            printf("%-3s%-9d", "", count->entry.size);
            printf("%-20s", "Word Document");
        }
        else if(strcmp(count->entry.type, "PDF") == 0)
        {
            printf(".");
            for(int i = 0; i < 3; i++)
            {
                printf("%c", count->entry.type[i]);
            }
            printf("%-3s%-9d", "", count->entry.size);
            printf("%-20s", "Portable Document");
        }
        else
        {
            printf("%-16s%-20s","","File Folder");
        }
        /* Print time */
        printf("%5s%02d/%02d/%-4d", "", count->entry.timeEntry.day, count->entry.timeEntry.month, count->entry.timeEntry.year + 1980);
        printf("%7s%02d:%02d:%02d", "", count->entry.timeEntry.hour, count->entry.timeEntry.minute, count->entry.timeEntry.second * 2);
        printf("\n"); 
    } 
}

