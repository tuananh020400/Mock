/*******************************************************************************
* Include
*******************************************************************************/

#include "FATFS.h"
#include "HAL.h"
#include "Check.h"

/*******************************************************************************
* Variables
*******************************************************************************/

extern FATFS_BootInfor_Struct_t g_bootSector;

/*******************************************************************************
* Code
*******************************************************************************/

int main(void)
{
    FATFS_EntryList_Struct_t *head = NULL;
    uint8_t select;
    
    if(HAL_Init("D:\\OneDrive - vnu.edu.vn\\FPT SoftWare\\Basic_C\\Mock\\floppy.img") == READ_FILE_SUCCESSFULLY)
    {
        head = ReadDirectory(0, head);
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
