#ifndef MODCONDUCTOR_MODBUS_H
#define MODCONDUCTOR_MODBUS_H

#include "common.h"

int ModbusReadAddress(STREAM *S, int Address, int Len, char *Bytes);
uint16_t ModBusReadUint16(const char **Input);
uint32_t ModBusReadUint32(const char **Input);
int ModbusReadDataBlock(const char *Host, int BaseAddr, char *Data, int Len);

#endif
