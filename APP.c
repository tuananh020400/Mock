#include "APP.h"

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



