#include "header.h"

extern FILE *file;



void Print(Entry entry)
{
    static uint8_t no = 0;

    if(entry.name[0] != 0)
    {
        no++;
        printf("%-4d", no);
        if(strcmp(entry.type, "TXT") == 0)
        {
            for(int i = 0; i < 8; i++)
            {
                printf("%c", entry.name[i]);
            }
            printf(".");
            for(int i = 0; i < 3; i++)
            {
                printf("%c", entry.type[i]);
            }
            printf("   %-7d", entry.size);
            printf("%-10s", "Text Document");
            printf("%5s%02d/%02d/%-4d", "", entry.day, entry.month, entry.year + 1980);
            printf("%7s%02d:%02d:%02d", "", entry.hour, entry.minute, entry.second * 2);
        }
        else
        {
            for(int i = 0; i < 8; i++)
            {
                printf("%c", entry.name[i]);
            }
            printf("%-14s%-10s","","File Folder");
            printf("%7s%02d/%02d/%-4d", "", entry.day, entry.month, entry.year + 1980);
            printf("%7s%02d:%02d:%02d", "", entry.hour, entry.minute, entry.second * 2);
        }
        printf("\n"); 
    }
}

void ReadEntryRDET(uint8_t startSector, uint8_t *buffer, Entry *entryArray)
{
    uint32_t count = 0x00;
    uint8_t i = 0;
    uint8_t nameCount = 0;
    uint8_t typeCount = 0;
    
    printf("%-4s%-15s%-7s%-18s%-17s%-10s\n", "No", "Name", "Size", "Type", "Day Modified", "Time Modified");
    printf("---------------------------------------------------------------------------------------\n");
    HAL_ReadSector(startSector, buffer);
    while (buffer[count] != 0x00 && count != 512) 
    {
        if((buffer[count + 0x0B] == 0x00) || (buffer[count + 0x0B] == 0x10))
        {
            for (nameCount = 0; nameCount < 8; nameCount++)
            {
                entryArray[i].name[nameCount] = buffer[count + 0x00 + nameCount];
            }
            for (typeCount= 0; typeCount < 3; typeCount++)
            {
                entryArray[i].type[typeCount] = buffer[count + 0x08 + typeCount];
            }
            entryArray[i].size = ((uint32_t)buffer[count + 0x1C]) | ((uint32_t)buffer[count + 1 + 0x1C] << 4) | \
            ((uint32_t)buffer[count + 2 + 0x1C] << 8) | ((uint32_t)buffer[count + 3 + 0x1C] << 12); 
            entryArray[i].day = buffer[count + 0x18] & (~0b11100000);
            entryArray[i].month = ((buffer[count + 0x18] & (~0b00011111)) >> 5) | ((buffer[count + 0x18 + 0x01] & (~0b11111110)) << 3);
            entryArray[i].year = ((buffer[count + 0x18 + 0x01] & (~0b00000001)) >> 1);
            entryArray[i].second = buffer[count + 0x16] & (~0b11100000);
            entryArray[i].minute = ((buffer[count + 0x16] & (~0b00011111)) >> 5) | ((buffer[count + 0x16 + 0x01] & (~0b11111000)) << 3);
            entryArray[i].hour = (buffer[count + 0x16 + 0x01] & (~0b00000111)) >> 3;

            Print(entryArray[i]);
        }
        // if((buffer[count + 0x0B] == 0x00))
        // {
        //     for (nameCount = 0; nameCount < 8; nameCount++)
        //     {
        //         entry.name[nameCount] = buffer[count + 0x00 + nameCount];
        //     }
        //     for (typeCount= 0; typeCount < 3; typeCount++)
        //     {
        //         entry.type[typeCount] = buffer[count + 0x08 + typeCount];
        //     }
        //     entry.size = ((uint32_t)buffer[count + 0x1C]) | ((uint32_t)buffer[count + 1 + 0x1C] << 4) | \
        //     ((uint32_t)buffer[count + 2 + 0x1C] << 8) | ((uint32_t)buffer[count + 3 + 0x1C] << 12);
        //     entry.day = buffer[count + 0x18] & (~0b11100000);
        //     entry.month = ((buffer[count + 0x18] & (~0b00011111)) >> 5) | ((buffer[count + 0x18 + 0x01] & (~0b11111110)) << 3);
        //     entry.year = ((buffer[count + 0x18 + 0x01] & (~0b00000001)) >> 1);
        //     entry.second = buffer[count + 0x16] & (~0b11100000);
        //     entry.minute = ((buffer[count + 0x16] & (~0b00011111)) >> 5) | ((buffer[count + 0x16 + 0x01] & (~0b11111000)) << 3);
        //     entry.hour = (buffer[count + 0x16 + 0x01] & (~0b00000111)) >> 3;
        //     Print(entry);
        // }
        // else if ((buffer[count + 0x0B] == 0x10))
        // {
        //     for (nameCount = 0; nameCount < 8; nameCount++)
        //     {
        //         entry.name[nameCount] = buffer[count + 0x00 + nameCount];
        //     }
        //     strcpy(entry.type,"   ");
        //     entry.size = 0;
        //     entry.day = buffer[count + 0x18] & (~0b11100000);
        //     entry.month = ((buffer[count + 0x18] & (~0b00011111)) >> 5) | ((buffer[count + 0x18 + 0x01] & (~0b11111110)) << 3);
        //     entry.year = ((buffer[count + 0x18 + 0x01] & (~0b00000001)) >> 1);
        //     entry.second = buffer[count + 0x16] & (~0b11100000);
        //     entry.minute = ((buffer[count + 0x16] & (~0b00011111)) >> 5) | ((buffer[count + 0x16 + 0x01] & (~0b11111000)) << 3);
        //     entry.hour = (buffer[count + 0x16 + 0x01] & (~0b00000111)) >> 3;
        //     Print(entry);
        // }
        count = count + 32;
    }
}


int main(void)
{
    BootSectorFAT12 bootSector;
    uint8_t buffer[512];
    Entry entryArray[16];
    
    file = fopen("D:\\OneDrive - vnu.edu.vn\\FPT SoftWare\\Basic_C\\Mock\\floppy.img", "rb");
    if(file == NULL)
    {
        printf("Error: Fail to open file");
    }
    else
    {
        HAL_ReadSector(0, buffer);
        ReadBootSector(&bootSector);
        ReadEntryRDET(bootSector.sectorsOfBoot + bootSector.sectorsOfFAT*bootSector.numOfFAT, buffer, entryArray);
    }
    fclose(file);
    
    return 0;
}



