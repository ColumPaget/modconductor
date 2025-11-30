#ifndef MODCONDUCTOR_SETTINGS_H
#define MODCONDUCTOR_SETTINGS_H


#include "common.h"

#define ACT_DISPLAY 0
#define ACT_INTEROGATE 1
#define ACT_HELP 2

typedef struct
{
int Action;
char *IniFile;
char *Host;
char *Proxy;
int BaseAddr;
} TSettings;


extern TSettings *Settings;

void PrintHelp();
TSettings *ParseCommandLine(int argc, const char *argv[]);

PARSER *ConfigFileRead(const char *Path);


#endif
