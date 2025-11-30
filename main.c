#include "common.h"
#include "modbus.h"
#include "settings.h"
#include "interogate_device.h"


int GetDataItemSize(const char *Data)
{
char *Type=NULL;
int size=0;

		GetToken(Data, ",", &Type, 0);

    if (strcasecmp(Type, "ip")==0) size=4;
    else if (strcmp(Type, "32")==0) size=4;
    else if (strcmp(Type, "16")==0) size=2;
    else if (strcmp(Type, "8")==0) size=1;

fprintf(stderr, "SIZE: %d %s\n", size, Type); 

    Destroy(Data);

    return(size);
}




const char *ProcessOp(const char *ptr, double *value)
{
    char *Token=NULL;

    ptr=GetToken(ptr, "\\S|*|/|+|-", &Token, GETTOKEN_MULTI_SEP | GETTOKEN_INCLUDE_SEP);

    if (strcmp(Token, "*")==0)
    {
        ptr=GetToken(ptr, "\\S|*|/|+|-", &Token, GETTOKEN_MULTI_SEP | GETTOKEN_INCLUDE_SEP);
        *value=*value * strtof(Token, NULL);
    }
    else if (strcmp(Token, "/")==0)
    {
        ptr=GetToken(ptr, "\\S|*|/|+|-", &Token, GETTOKEN_MULTI_SEP | GETTOKEN_INCLUDE_SEP);
        *value=*value / strtof(Token, NULL);
    }
    else if (strcmp(Token, "-")==0)
    {
        ptr=GetToken(ptr, "\\S|*|/|+|-", &Token, GETTOKEN_MULTI_SEP | GETTOKEN_INCLUDE_SEP);
        *value=*value - strtof(Token, NULL);
    }
    else if (strcmp(Token, "+")==0)
    {
        ptr=GetToken(ptr, "\\S|*|/|+|-", &Token, GETTOKEN_MULTI_SEP | GETTOKEN_INCLUDE_SEP);
        *value=*value + strtof(Token, NULL);
    }

    Destroy(Token);

    return(ptr);
}


double ProcessConvert(uint32_t word, const char *Convert)
{
    const char *ptr;
    double value;

    value=(double) word;

    ptr=Convert;
    while (ptr)
    {
        ptr=ProcessOp(ptr, &value);
    }

    return(value);
}



const char *OutputItem(const char *DataPtr, const char *Name, const char *Type, const char *Convert)
{
    const char *dptr;
    uint32_t quad;
    uint16_t word;
    double value;

    if (CompareStr(Name, "IP")==0) return(DataPtr);
    if (CompareStr(Name, "BaseAddr")==0) return(DataPtr);

    dptr=DataPtr;

    if (strcmp(Type, "ip")==0)
    {
        quad=ModBusReadUint32(&dptr);
        printf("  %s: %s\n", Name, IPtoStr(quad));
    }
    else if (strcmp(Type, "32")==0)
    {
        quad=ModBusReadUint32(&dptr);
        value=ProcessConvert((uint32_t) quad, Convert);
        printf("  %s: %.2f\n", Name, value);
    }
    else if (strcmp(Type, "16")==0)
    {
        word=ModBusReadUint16(&dptr);
        value=ProcessConvert((uint32_t) word & 0xFFFF, Convert);
        printf("  %s: %.2f\n", Name, value);
    }
    else if (strcmp(Type, "8")==0)
    {
        word=ModBusReadUint16(&dptr);
        value=ProcessConvert((uint32_t) word & 0xFF, Convert);
        printf("  %s: %.2f\n", Name, value);
    }


    return(dptr);
}


void ReadBlock(char *Host, int BaseAddr, int BlockLen, ListNode *Values)
{
    char *Data=NULL, *Type=NULL, *Convert=NULL;
    const char *ptr, *dptr;
    ListNode *Curr;
    int len;

    Data=SetStrLen(Data, 4096);
    len=ModbusReadDataBlock(Host, BaseAddr, Data, BlockLen);
    if (len > 0)
    {
        dptr=Data;

        Curr=ListGetNext(Values);
        while (Curr)
        {
            ptr=GetToken((const char *) Curr->Item, ",", &Type, 0);
            ptr=GetToken(ptr, ",", &Convert, 0);
            dptr=OutputItem(dptr, Curr->Tag, Type, Convert);
            Curr=ListGetNext(Curr);
        }
    }
    else printf("ERROR: Failed to connect to or read from modbus device at %s\n", Host);

    Destroy(Convert);
    Destroy(Type);
    Destroy(Data);
}




void ProcessDataBlock(ListNode *Block)
{
    ListNode *BlockItems, *Curr;
    char *IP=NULL, *Config=NULL;
    long BaseAddr=0, BlockLen=0;

    printf("%s\n", Block->Tag);
    BlockItems=(ListNode *) Block->Item;
    Curr=ListGetNext(BlockItems);
    while (Curr)
    {
        if (CompareStr(Curr->Tag, "IP")==0) IP=CopyStr(IP, (const char *) Curr->Item);
        else if (CompareStr(Curr->Tag, "BaseAddr")==0) BaseAddr=strtol((const char *) Curr->Item, NULL, 16);
				else BlockLen += GetDataItemSize((const char *) Curr->Item);
        Curr=ListGetNext(Curr);
    }

fprintf(stderr, "RB: %d\n", BlockLen);

    ReadBlock(IP, BaseAddr, BlockLen, BlockItems);


    printf("\n");

    Destroy(Config);
    Destroy(IP);
}


void DeviceDisplayFromIni(const char *IniPath)
{
    PARSER *P=NULL, *Curr;

    P=ConfigFileRead(IniPath);
    Curr=ListGetNext(P);
    while (Curr)
    {
        ProcessDataBlock(Curr);
        Curr=ListGetNext(Curr);
    }
}




int main(int argc, const char *argv[])
{
    TSettings *Settings;

    if (argc < 2)
    {
        printf("No path to config file given on command-line\n");
        return(1);
    }

    Settings=ParseCommandLine(argc, argv);

    if (StrValid(Settings->Proxy)) SetGlobalConnectionChain(Settings->Proxy);

    switch (Settings->Action)
    {
    case ACT_INTEROGATE:
        InterogateDevice(Settings->Host, Settings->BaseAddr);
        break;

		case ACT_HELP:
			PrintHelp();
			break;

    default:
        DeviceDisplayFromIni(Settings->IniFile);
        break;
    }


    return(0);
}
