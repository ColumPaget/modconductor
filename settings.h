#ifndef MODCONDUCTOR_SETTINGS_H
#define MODCONDUCTOR_SETTINGS_H


#include "common.h"

#define ACT_DISPLAY 0
#define ACT_INTEROGATE 1

typedef struct
{
int Action;
char *IniFile;
char *Host;
char *Proxy;
} TSettings;


extern TSettings *Settings;


TSettings *ParseCommandLine(int argc, const char *argv[]);


#endif
