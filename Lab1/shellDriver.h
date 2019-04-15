#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef SHELLDRIVER_H
    #define SHELLDRIVER_H
    #define TRUE  1
    #define FALSE 0
    #define MAX_LINE 80
    #define MAX_COMMANDS 2
    #define MAX_ARGS 40

    typedef struct {
        int   numArgs;
        char *command;
        char *commandArgs[MAX_ARGS + 1];
        char *outFile;
        char *inFile;
        int   rinFD;
        int   routFD;
    } Command;

    typedef struct {
        int  errorFlag;
        int  numOfCommands;
        int  pipeBars;
        int  ampersand;
        int  repeatCommand;
        char line[MAX_LINE + 1];
        Command commands[MAX_COMMANDS];
    } CommandLine;

    int getCommandArgs(CommandLine* cml);
    int getCommands(CommandLine* cml);
    void clearBuffer();
    void initCommand(Command* cmd);
    void launchPipeline(CommandLine* cml);
    int  readCommandLine(CommandLine* cml, CommandLine *prevCml);
    void initCommandLine(CommandLine* cml);
    int separateArgs(Command* cmd, CommandLine* cml);
    void updateArguments(Command* cmd, char* arg[], int* i);
    int redirectionFileOpen(CommandLine* cml, Command* cmd);
    void redirect(CommandLine* cml, Command* cmd, int fdIn, int fdOut);
#endif

