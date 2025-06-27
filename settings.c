#include "settings.h"

TSettings *Settings=NULL;


TSettings *ParseCommandLine(int argc, const char *argv[])
{
    CMDLINE *CMD;
    const char *arg;

    if (! Settings) Settings=(TSettings *) calloc(1, sizeof(TSettings));

    CMD=CommandLineParserCreate(argc, (char **) argv);
    arg=CommandLineNext(CMD);
    while (arg)
    {
        if (strcmp(arg, "-f")==0) Settings->IniFile=CopyStr(Settings->IniFile, CommandLineNext(CMD));
        else if (strcmp(arg, "-P")==0) Settings->Proxy=CopyStr(Settings->Proxy, CommandLineNext(CMD));
        else if (strcmp(arg, "-proxy")==0) Settings->Proxy=CopyStr(Settings->Proxy, CommandLineNext(CMD));
        else Settings->IniFile=CopyStr(Settings->IniFile, arg);

        arg=CommandLineNext(CMD);
    }

    return(Settings);
}

