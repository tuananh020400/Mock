#include "APP.h"
#include "math.h"

static uint32_t StringToInt(char *ch)
{
    uint32_t num = 0;
    uint8_t len = strlen(ch);
    int i = 0;
    
    for(i = len - 1; i >= 0; i--)
    {
        num += (int)(ch[i] - '0') * pow(10,len - i - 1);
    }
    return num;
}

uint32_t InputInt()
{
    char ch[8] = "";
    uint8_t check = 1;
    int i = 0;
    uint8_t num = 0;
    
    while(check)
    {
        fflush(stdin);
        gets(ch);
        num = 0;
        
        /* Check input number */
        if((ch[0] < '0') || ch[0] > '9')
        {
            num++;
        }
        for(i = 1; ch[i] != 0; i++)
        {
                if((ch[i] < '0') || ch[i] > '9')
                {
                    num++;
                }
        }
        
        if(num == 0) /* num > 0 <=> Contains wrong characters */
        {
            check = 0; /* break */
        }
        else
        {
            printf("Error: Syntax error. Please re-enter: ");
        }
    }
    return StringToInt(ch);
}

FATFS_EntryList_Struct_t * FATFS_ReadFileAndDirectory(FATFS_EntryList_Struct_t *head, uint8_t select)
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
        system("cls");
        ReadDirectory(temp->entry.clusterLow, &head);
        DisplayDirectory(head);
    }
    else
    {
        system("cls");
        PrintFile(temp->entry.clusterLow, temp->entry.fileSize);
        printf("\n");
        system("pause");
        system("cls");
        DisplayDirectory(head);
    }
    
    return head;
}

void PrintFile(uint32_t cluster, uint32_t size)
{
    uint8_t *buffer = NULL;

    buffer = (uint8_t*)malloc(size);
    ReadFile(cluster, size, buffer);
    for(int i = 0; i < size; i++)
    {
        printf("%c", buffer[i]);
    }
}
void RunApp()
{
    FATFS_EntryList_Struct_t *head = NULL;
    uint8_t select;
    FATFS_EntryList_Struct_t * FATFS_ReadFileAndDirectory(FATFS_EntryList_Struct_t *head, uint8_t select);
    
    HAL_Init("D:\\OneDrive - vnu.edu.vn\\FPT SoftWare\\Basic_C\\Mock2\\floppy.img");
        ReadDirectory(0, &head);
        DisplayDirectory(head);
        while(1)
        {
            do
            {
                printf("\nEnter your select: ");
                select = InputInt();
            } while (CheckSelect(head, select) != 0);
            head = FATFS_ReadFileAndDirectory(head, select);
        }
}



