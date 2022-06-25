/*******************************************************************************
* Include
*******************************************************************************/

#include "header.h"

/*******************************************************************************
* Variables
*******************************************************************************/

extern FATFS_BootSector_Struct_t g_bootSector;

/*******************************************************************************
* Code
*******************************************************************************/

int main(void)
{
    node head = NULL;
    uint8_t select;
    
    HAL_Init("D:\\OneDrive - vnu.edu.vn\\FPT SoftWare\\Basic_C\\Mock\\floppy.img");
    head = ReadDirectory(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT, head);
    Print(head);
    while(1)
    {
        do
        {
            printf("Enter position to add: ");
            select = InputInt();
        } while (CheckSelect(head, select) != 0);
        head = ChageDirectory(head, select);
    }
}
