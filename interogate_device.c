#include "interogate_device.h"
#include "modbus.h"

char *InterogateFindString(char *RetStr, const char *data, const char *end, const char **StringTo)
{
const char *ptr;

RetStr=CopyStr(RetStr, "");
if ((StringTo != NULL) && (*StringTo > data)) return(RetStr);

for (ptr=data; (ptr < end) && (*ptr >= 32) && (*ptr < 127); ptr++)
{
RetStr=AddCharToStr(RetStr, *ptr);
}
*StringTo=ptr;

return(RetStr);
}


void InterogateDevice(const char *Host)
{
    uint16_t *p_word, *p_end;
    char *Tempstr=NULL;
    char *Data=NULL;
    const char *DataEnd=NULL, *StringTo=NULL;
    int Addr=0, Value=0, len;

    Data=(char *) calloc(1, 4096);
    len=ModbusReadDataBlock(Host, 0, Data, 4096);
    if (len > 0)
    {
        DataEnd=Data+len;
        p_end=(uint16_t *) DataEnd;
        for (p_word=(uint16_t *) Data; p_word < p_end; p_word++)
        {
            Value=(int) *p_word;
            if (Value > 0) 
	    {
		printf("%x:  %u", Addr, Value);
		Tempstr=InterogateFindString(Tempstr, Data, DataEnd, &StringTo);
		if (StrLen(Tempstr) > 4) printf("possible ascii: %s\n", Tempstr);
		printf("\n");
	    }
        }

    }

    Destroy(Tempstr);
    Destroy(Data);
}

