#ifndef MODCONDUCTOR_SETTINGS_H
#define MODCONDUCTOR_SETTINGS_H


#include "common.h"


typedef struct
{
char *IniFile;
char *Proxy;
} TSettings;


extern TSettings *Settings;


TSettings *ParseCommandLine(int argc, const char *argv[]);


#endif
