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

char *InterogateFindIP(char *RetStr, const char *data, const char *end, const char **StringTo)
{
		char *Tempstr=NULL;
		uint32_t IP;
		const char *ptr;
    int i;

    RetStr=CopyStr(RetStr, "");
		if ((data + 4) > end) return(RetStr);
    if ((StringTo != NULL) && (*StringTo > data)) return(RetStr);

		IP=ntohl(* (uint32_t *)data);

		ptr=&IP;
		if ( ((*ptr & 0xFF)== 192) && ((*(ptr+1) & 0xFF) ==168) ) RetStr=CopyStr(RetStr, IPtoStr(IP));

		Destroy(Tempstr);

    return(RetStr);
}

void InterogateDevice(const char *Host, int BaseAddr)
{
    uint16_t *p_word, *p_end;
    char *Tempstr=NULL;
    char *Data=NULL;
    const char *DataEnd=NULL, *StringTo=NULL;
    int Addr=0, Value=0, len, req_len;

    Data=(char *) calloc(1, 4096);

    for (req_len=128; req_len > 8; req_len /= 2)
    {
        len=ModbusReadDataBlock(Host, BaseAddr, Data, req_len);
        if (len > 0) break;

        printf("Bad response when asking for %d registers. Wait 5s then try fewer\n", req_len);
        sleep(5);
    }

    if (len > 0)
    {
        DataEnd=Data+len;
        p_end=(uint16_t *) DataEnd;
        for (p_word=(uint16_t *) Data; p_word < p_end; p_word++)
        {
            Value=ntohs((int) *p_word);
            if (Value > 0)
            {
                printf("0x%04x:  %u ", BaseAddr + Addr, Value);
                Tempstr=InterogateFindString(Tempstr, (char *) p_word, DataEnd, &StringTo);
                if (StrLen(Tempstr) > 4) printf("possible ascii: %s", Tempstr);
                else
								{
                	Tempstr=InterogateFindIP(Tempstr, (char *) p_word, DataEnd, &StringTo);
									if (StrValid(Tempstr)) printf("possible IP Address: %s", Tempstr);
                  else if (Value > 32767) printf("possible signed: %d", (int) 0- (~(Value -1) & 0xFF));
								}
                printf("\n");
            }
            Addr++;
        }

    }

    Destroy(Tempstr);
    Destroy(Data);
}

