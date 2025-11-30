#include "settings.h"

TSettings *Settings=NULL;


void PrintHelp()
{
printf("usage:\n");
printf("  modconductor.exe <ini file>                                -  read declarations from ini file, query hosts and display output\n");
printf("  modconductor.exe -P socks5:<myproxy>:<port> <ini file>     -  query hosts via socks5 proxy\n");
printf("  modconductor.exe -P ssh:<ssh name> <ini file>              -  query hosts via ssh server configured in ~/.ssh/config\n");
printf("  modconductor.exe -i <host> -b <base addr>                  -  interrogate host, print out 128 registers from hexadecimal <base addr> autodetecting some patterns\n");
printf("  modconductor.exe -?                                        -  this help\n");
printf("  modconductor.exe -h                                        -  this help\n");
printf("  modconductor.exe -help                                     -  this help\n");
printf("  modconductor.exe --help                                    -  this help\n");
}


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
        else if (strcmp(arg, "-b")==0) Settings->BaseAddr=strtol(CommandLineNext(CMD), NULL, 16);
        else if (strcmp(arg, "-i")==0)
        {
            Settings->Action=ACT_INTEROGATE;
            Settings->Host=CopyStr(Settings->Host, CommandLineNext(CMD));
        }
        else if (strcmp(arg, "-?")==0) Settings->Action=ACT_HELP;
        else if (strcmp(arg, "-h")==0) Settings->Action=ACT_HELP;
        else if (strcmp(arg, "-help")==0) Settings->Action=ACT_HELP;
        else if (strcmp(arg, "--help")==0) Settings->Action=ACT_HELP;
        else Settings->IniFile=CopyStr(Settings->IniFile, arg);

        arg=CommandLineNext(CMD);
    }

    return(Settings);
}



PARSER *ConfigFileRead(const char *Path)
{
    PARSER *P=NULL;
    STREAM *S=NULL;
    char *Tempstr=NULL;

    S=STREAMOpen(Path, "r");
    if (S)
    {
        Tempstr=STREAMReadDocument(Tempstr, S);
        STREAMClose(S);
        P=ParserParseDocument("ini", Tempstr);
    }

    Destroy(Tempstr);
    return(P);
}

