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
    
    if(HAL_Init("D:\\OneDrive - vnu.edu.vn\\FPT SoftWare\\Basic_C\\Mock\\floppy.img") == READ_FILE_SUCCESSFULLY)
    {
        head = ReadDirectory(g_bootSector.sectorsOfBoot + g_bootSector.sectorsOfFAT * g_bootSector.numOfFAT, head);
        DisplayDirectory(head);
        while(1)
        {
            do
            {
                printf("\nEnter your select: ");
                select = InputInt();
            } while (CheckSelect(head, select) != 0);
            head = ReadFileOrChangeDirectory(head, select);
        }
    }

    return 0;
}

/*******************************************************************************
* End of file
*******************************************************************************/
