#include "modbus.h"


const char *ModbusExceptionString(int Code)
{

switch (Code)
{
case 1: return("Illegal function - function not supported by server"); break;
case 2: return("Illegal data address - address out of range"); break;
case 3: return("Illegal data value - value out of range for datatype"); break;
case 4: return("Server failure"); break;
case 5: return("Acknowledge - request is being processed, may take some time"); break;
case 6: return("Server busy - server is busy with another request, try later"); break;
case 7: return("Refused - server cannot perform action, consult diagnostics"); break;
case 8: return("Memory parity error - memory corruption, retry request"); break;
case 10: return("Gateway path unavailable - modbus gateway misconfigured"); break;
case 11: return("Gateway target no response - target device off network or failed to respond"); break;


}

return("unknown exception");
}



void ModbusException(int Code, int Address, int Len)
{
        fprintf(stderr, "MODBUS EXCEPTION: %d (%x) \"%s\" when reading %d bytes from %d\n", Code, Code, ModbusExceptionString(Code), Len, Address);
}



int ModbusSendReadRequest(STREAM *S, int Address, int Len)
{
static uint16_t TransactionID=0;
uint16_t word;
char *Tempstr=NULL, *ptr;

//    TransactionID=0x1234;
TransactionID++;

    Tempstr=SetStrLen(Tempstr, 1024);
    ptr=Tempstr;

    //transation ID should really be unique every time
    word=htons(TransactionID);
    memcpy(ptr, &word, 2);
    ptr+=2;

    //protocol identifier (0x0 for modbus over TCP)
    word=htons(0x0);
    memcpy(ptr, &word, 2);
    ptr+=2;

    //remaining bytes in this frame
    word=htons(0x6);
    memcpy(ptr, &word, 2);
    ptr+=2;


    //'unit identifier', not normally used but set to 1 here
    *ptr=0x1;
    ptr++;

    //this is the 'instruction code', in this case 0x3 is 'read from registers'
    *ptr=0x3;
    ptr++;

    //base address to start reading from
    word=htons(Address);
    memcpy(ptr, &word, 2);
    ptr+=2;

    //number of registers to read
    word=htons(Len);
    memcpy(ptr, &word, 2);
    ptr+=2;

    STREAMWriteBytes(S, Tempstr, ptr-Tempstr);
    STREAMFlush(S);

Destroy(Tempstr);

return((int) TransactionID);
}




int ModbusReadAddress(STREAM *S, int Address, int Len, char *Bytes)
{
    char *Tempstr=NULL, *ptr;
    uint16_t word, TransactionID, *sptr;
    int i, result, out_len;


    TransactionID=ModbusSendReadRequest(S, Address, Len);

    Tempstr=SetStrLen(Tempstr, 1024);
    result=STREAMReadBytes(S, Tempstr, 1024);

    /*
        for (i=0; i < result; i++)
        {
            printf("%x ", Tempstr[i] & 0xFF);
        }
        printf("\n");

        for (i=0; i < result; i++)
        {
            printf("%u ", Tempstr[i] & 0xFF);
        }
        printf("\n");
    */


    ptr=Tempstr;
    word=ntohs( *(uint16_t *) ptr);

//   if (word==TransactionID) printf("Transaction Matches\n");

    //protocol indentifer will consume another two bytes here
    ptr=Tempstr+4;


    //remaining bytes in packet
    word=ntohs( *(uint16_t *) ptr);
//   printf("Reply Length: %u\n", word & 0xFFFF);
    ptr+=2;

    ptr++; //skip unit identifier

    if (*ptr == 0x3)
    {
        ptr=Tempstr+8;
        out_len=*ptr & 0xFF;
//   printf("DataLength: %d\n", out_len);

        ptr++;
        memcpy(Bytes, ptr, out_len);
    }
    else 
    {
        ptr++;
        ModbusException(*ptr & 0xFF, Address, Len);
	out_len=-1;
    }


    Destroy(Tempstr);

    return(out_len);
}


uint16_t ModBusReadUint16(const char **Input)
{
    const char *iptr;
    uint16_t word;

    iptr=*Input;
    word=ntohs(* (uint16_t *) iptr);
    iptr+=2;
    *Input=iptr;

    return(word);
}


uint32_t ModBusReadUint32(const char **Input)
{
    uint32_t quad;
    uint16_t word;
    const char *iptr;
    char *optr;

    iptr=*Input;
    word=ntohs(* (uint16_t *) iptr);
    optr=(char *) &quad;
    memcpy(optr, &word, 2);

    iptr+=2;
    optr+=2;

    word=ntohs(* (uint16_t *) iptr);
    memcpy(optr, &word, 2);
    iptr+=2;

    *Input=iptr;

    return(quad);
}


int ModbusReadDataBlock(const char *Host, int BaseAddr, char *Data, int Len)
{
    char *Tempstr=NULL;
    STREAM *S;
    int len=-1;

    Tempstr=FormatStr(Tempstr, "tcp:%s:502", Host);
    S=STREAMOpen(Tempstr, "r timeout=30");

    if (S)
    {
        len=ModbusReadAddress(S, BaseAddr, Len / 2, Data);
        STREAMClose(S);
    }

    Destroy(Tempstr);

    return(len);
}


