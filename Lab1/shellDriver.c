#include "shellDriver.h"


// Note: TRUE AND FALSE IN THE FUNCTIONS BELOW DENOTE IF AN ERROR
// WAS REACHED OR NOT.


// Clears the buffer
void clearBuffer(){
    while(getchar()!='\n');
}




/* This process initializes the first and possibly only command given
 * for any command line. It runs the program specified in the first command's
 * arguments with the required redirections.
 */
void addFirstProcess(CommandLine* cml,int fdesc1[], int fdesc2[]) {
    pid_t pid;
    Command *cmd = &cml->commands[0];

    // Hardcode the file descriptors for a singular process
    if (cml->numOfCommands == 1){
        fdesc2[1] = 1;
        fdesc1[0] = 0;
    }

    // if more than one command then run the pipe command to open new file descriptors
    if(cml->numOfCommands > 1){
        if (pipe(fdesc2) < 0) {
            perror(NULL);
            exit(EXIT_FAILURE);
        }
    }


    // Handle fork error
    if((pid=fork())<0){
        perror(NULL);
        exit(EXIT_FAILURE);
    }

    // If child close the the file descriptor not needed in the child,
    // redirect inputs or output file descriptors and execute the program.
    if(!pid) {
        if(cml->numOfCommands > 1) {
            close(fdesc2[0]);
        }
        redirect(cml,cmd,fdesc1[0],fdesc2[1]);
        if(execvp(cmd->commandArgs[0],cmd->commandArgs) < 0) {
            fprintf(stderr, "Command \"%s\" not found\n", cmd->commandArgs[0]);
            exit(EXIT_FAILURE);
        }
    }

    // Handle parent's file descriptors

    if(cml->numOfCommands > 1)
        close(fdesc2[1]);

    if(fdesc1[0]!=0) {
        close(fdesc1[0]);
        fdesc1[0] = fdesc2[0];
    }
}




/* This function forks the second process and executes it after forming a pipeline
 * between it and the first process.
 * */
void addLastProcess(CommandLine* cml, int fdesc1[]){
    Command *cmd = &cml->commands[cml->numOfCommands - 1];
    pid_t pid;

    // Don't execute function if 1 or no commands were entered.
    if(cml->numOfCommands<=1) {
        return;
    }

    // fork a process
    pid = fork();

    // handle fork error
    if (pid < 0) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    //if returned as the child process then handle redirection and
    //execute the specified program.
    else if (!pid) {

        redirect(cml,cmd,fdesc1[0],1);
        if(execvp(cmd->commandArgs[0],cmd->commandArgs) < 0) {
            fprintf(stderr, "Command \"%s\" not found\n", cmd->commandArgs[0]);
            exit(EXIT_FAILURE);
        }
    }
    //close the "in" file descriptor in the parent due to it not being needed.
    close(fdesc1[0]);

}




/* This program represents the main stage of the shell, i.e
 * the part of the program that deals with execution of processes after the inputs
 *  are parsed. A pipeline of two processeses are created if two commands are entered,
 *  otherwise only one prcoess is initated. If an ampersand was previously detected
 *  then the parent will not wait for the children process(es).
 * */
void launchPipeline(CommandLine* cml){
    int i;
    int fdesc1[2]={0}, fdesc2[2]={0};// file descriptor arrays

    // Initiate the first process
    addFirstProcess(cml,fdesc2,fdesc1);
    addLastProcess(cml,fdesc1); // Initiate the last process, if command 2 exists


    // If ampersand was found, clear the buffer and return instead of waiting for
    // the children processes to terminate.
    if (cml->ampersand){
        clearBuffer();
        return;
    }
    // Wait for children to terminate if no ampersand concurrency operator
    // was used
    for (i = 0; i < cml->numOfCommands; ++i) {
        wait(NULL);
    }
}





//Initialize the CommandLine struct
void initCommandLine(CommandLine* cml){
    int i;
    cml->numOfCommands=0;
    cml->errorFlag = 0;
    cml->ampersand = 0;
    cml->repeatCommand=0;
    cml->pipeBars=0;
    cml->line[0]='\0';

    for (i = 0; i < MAX_COMMANDS; ++i) {
        initCommand(&cml->commands[i]);
    }
}




// Initialize the Command struct
void initCommand(Command* cmd) {
    int i;
    cmd->numArgs = 0;
    cmd->rinFD = 0; // redirection-in file descriptor
    cmd->routFD = 0; // redirection-out file descriptor
    cmd->command = NULL;
    cmd->outFile = NULL;
    cmd->inFile = NULL;

    for (i = 0; i < MAX_ARGS; ++i) {
        cmd->commandArgs[i] = NULL;
    }
}




/* This function tokenizes the commands passed from the command line.
 * Tokens, which in this case are commands, are separated by pipe bars.
 * Return status indicates if an error was found.
 * */
int getCommands(CommandLine* cml){
    int i=0;

    cml->commands[i].command = strtok (cml->line,"|");
    while (cml->commands[i].command != NULL) {
        i++;
        // TOO MANY COMMANDS ENTERED, RETURN ERROR
        if(i >= MAX_COMMANDS+1){
            fprintf(stderr,"Too many commands. Please try again.\n\n");
            return cml->errorFlag=TRUE;
        }
        cml->commands[i].command = strtok(NULL, "|");
    }
    cml->numOfCommands=i; // SAVE NUM OF COMMANDS
    return FALSE;
}




/* This function is responsible for finding the ampersand in the
 * argument lists of each command structure. It returns if an error
 * was found or not.
 * */
int findAmpersand(CommandLine ***cml, Command *cmd) {
    int i = 0;

    for (i = 0; i < cmd->numArgs; ++i) {
        // if & was found before last argument, then return error
        if (!strcmp("&",cmd->commandArgs[i]) && (i != cmd->numArgs-1)) {
            return (**cml)->errorFlag = TRUE;
        }
    }


    if (!strcmp("&",cmd->commandArgs[cmd->numArgs-1])) {
        (**cml)->ampersand = TRUE; // AMPERSAND WAS FOUND ON LAST ARG
    }

    return FALSE; // NO ERROR FOUND
}




/* This function parses the argument array in search of an ampersand
 * operator and the incorrect use of the operator and returns whether an error
 * was encountered or not.
 */
int processAmpersand(CommandLine ** cml){
    Command command1 = (*cml)->commands[0], command2 = (*cml)->commands[1];
    int result1 = 0, result2 = 0, command1Amp = 0;

    // If two commands were entered find the error statuses of each command
    if ((*cml)->numOfCommands == 2) {

        result1 = findAmpersand(&cml, (&command1));
        command1Amp = (*cml)->ampersand;
        (*cml)->ampersand = 0;
        result2 = findAmpersand(&cml, (&command2));
        // if an error or ampersand (in command 1)was reached, return error found
        if (result1 || command1Amp || result2) {
            //Display error message
            fprintf(stderr,"Syntax error, unexpected & token.\n\n");
            return (*cml)->errorFlag = TRUE;
        }

    }
    //if one command was entered, find the error status of the search and display an
    // error message if one was found
    else if ((*cml)->numOfCommands == 1) {
        result1 = findAmpersand(&cml, (&command1));
        if (result1) {
            //Display error message
            fprintf(stderr,"Syntax error, unexpected & token.\n\n");
            return (*cml)->errorFlag = TRUE;
        }

    }

    return FALSE; // Return no error found
}





/* This function calls on separateArgs to tokenize arguments for each command
 * that is entered. Then the arguments are checked for an & character and
 * also for correct syntactical use of the operator. Returns if an error was
 * reached on either tokenizing or parsing the arguments.
 * */
int getCommandArgs(CommandLine* cml){
    int i=0, errorStatus = 0, es2 = 0 ;

    for (i = 0; i < cml->numOfCommands; ++i) {
        // Set the errorStatus variable permanently if an error is returned at some point
        errorStatus |= separateArgs(&cml->commands[i], cml);
    }

    // Store the second error status in es2, which holds the negated return value from
    // processAmpersand. The ampersand is then removed so as to not include it in the
    // execvp call.
    if ((es2 = (!processAmpersand(&cml)))) {
        if (cml->ampersand && cml->numOfCommands==2){
            cml->commands[1].commandArgs[cml->commands[1].numArgs-1] = NULL;
        }
        else if (cml->ampersand && cml->numOfCommands ==1){
            cml->commands[0].commandArgs[cml->commands[0].numArgs-1] = NULL;
        }
    }

    return errorStatus || (!es2);
}




/* This function ensures synactic use of the pipe (bar) operator
 * in order to form a pipeline between two commands. Returns if
 * an error was encountered or not.
 * */
int checkPipeValidity(CommandLine* cml,int j){
    int i=0,noArg=1;// noArg represents "no arguments encountered yet"


    // breaks loop as soon as errorFlag is set
    for (i = 0; i < j && !cml->errorFlag; ++i) {
        //if no arguments encountered yet but a pipe bar is encountered, set errorFlag
        cml->errorFlag = (noArg && cml->line[i]=='|')?1:cml->errorFlag;
        //if the currenct character is a pipe bar, set noArg to 1
        noArg=(cml->line[i]=='|')?1:noArg;
        //if the current character is a non-pipe-bar and not a space, then the function
        //is currently in between an argument (word).
        noArg=(cml->line[i]!='|' && !isspace(cml->line[i]))?0:noArg;

    }

    // Invokes error message and return with error if incorrect syntax is found
    if(cml->errorFlag || cml->line[j-1]=='|' || cml->line[i]=='|' || noArg){
        fprintf(stderr,"Invalid pipe usage.\n\n");
        return (cml->errorFlag=TRUE);
    }
    return FALSE;
}





/*  This function reads input from the user from the shell's prompt.
 *  It takes in command lines up to 80 characters long.
 *  This function will check for both the exit command and the "!!"
 *  command to repeat the last command entered by the user. Finally,
 *  a function call to the checkPipeValidity function is made to
 *  ensure correct syntactic use of the pipe operator.
 * */
int readCommandLine(CommandLine* cml, CommandLine *prevCml){
    int chr, i = 0;
    initCommandLine(cml); // Initialize command line struct

    // Get input from user
    while (TRUE) {
        // If EOF(CTRL-D) then output exit message and exit with EXIT_SUCCESS
        if ((chr = fgetc(stdin)) == EOF) {
            printf("Exiting...\n");
            exit(EXIT_SUCCESS);
        }
        //error handling for command line over 80 characters
        else if (i >= MAX_LINE + 1) {
            fprintf(stderr, "Command line too long. Please try again\n\n");
            return 1;
        }

        else if (chr == '\n') {
            break;
        }

        // update string with new character
        cml->line[i++] = chr;
    }

    // end string
    cml->line[i] = '\0';


    // Check for exit command and exit with EXIT_SUCCESS
    if (!strcmp("exit", cml->line)) {
        exit(EXIT_SUCCESS);
    }

    // Check for "!!" to repeat the last command
    if (!strcmp("!!", cml->line)) {
        if (prevCml->numOfCommands) {
            cml->repeatCommand = 1;
            return FALSE;
        }
        else{
            fprintf(stderr,"No commands in history.\n\n");
            return TRUE;
        }
    }

    // "Error" if command line is empty, restart
    if(!i){
        return cml->errorFlag=TRUE;
    }

    // Check for correct syntactic use of pipe operator
    return checkPipeValidity(cml, i);
}





/*
 * This function serves as a command parser. It tokenizes a command's
 * arguments, which are separated by spaces. These tokens are later used
 * in the execvp function to run a the required program with the given arguments.
 *
 * */
int separateArgs(Command* cmd,CommandLine* cml){
    int j=0,tempNumArgs=0,k; // Initialize counters
    char* tempArgs[MAX_ARGS]; // Temporary storage for tokens

    tempArgs[j] = strtok(cmd->command, " \t");

    while (tempArgs[j] != NULL) {
        j++;

        // Error handling for too many arguments provided
        if (j >= MAX_ARGS + 1) {
            fprintf(stderr,"Too many arguments. Please try again.\n\n");
            return cml->errorFlag = TRUE;
        }

        tempArgs[j] = strtok(NULL, " \t");
    }

    // Store total number of arguments
    tempNumArgs = j;

    // Updates the command (cmd) struct's arguments and finds the appropriate
    // redirections
    for (k = 1; k < tempNumArgs+1; ++k) {
        updateArguments(cmd,tempArgs,&k);
    }

    // Opens files for redirections and stores file descriptors
    return redirectionFileOpen(cml,cmd);
}






/* This function parses the arguments for redirection operators and
 * files to redirect to. The remaining arguments are stored in the command
 * struct.
 * */
void updateArguments(Command* cmd, char* tempArgs[],int* i) {
    // Handle output redirection and store appropriate output file name

    if (!strcmp(tempArgs[*i - 1], ">")) {
        cmd->outFile = tempArgs[*i];
        *i=*i+1;
    }
    // Handle input redirection and store appropriate input file name
    else if (!strcmp(tempArgs[*i - 1], "<")){
        cmd->inFile = tempArgs[*i];
        *i=*i+1;
    }
    // Store remaining arguments in command struct and update count.
    else {
        cmd->commandArgs[cmd->numArgs]=tempArgs[*i-1];
        cmd->numArgs++;
    }
}





/* This function opens the in and out files for redirection that
 * were found in the updateArguments function. These files will be opened
 * for redirection and the appropriate file descriptors will be stored.
 * Return values report whether or not an error was found.
 * */
int redirectionFileOpen(CommandLine* cml, Command* cmd){

    if(cmd->inFile!=NULL) {
        if((cmd->rinFD = open(cmd->inFile, O_RDONLY)) < 0){
            fprintf(stderr, "Unable to open file for output\n");
            return cml->errorFlag = TRUE;

        }
    }
    if(cmd->outFile!=NULL){
        if((cmd->routFD = open(cmd->outFile,O_CREAT|O_WRONLY|O_TRUNC,0666)) < 0){
            fprintf(stderr, "Unable to open file for output\n");
            return cml->errorFlag = TRUE;
        }
    }
    return FALSE;
}




/*  This function redirects inputs and outputs to the
 *  by making a copy of the old file descriptor, "redirecting" from the
 *  old file descriptor to the new file descriptor. This function can handle
 *  input and output redirection simultaneously for the same command.
 * */
void redirect(CommandLine* cml, Command* cmd, int fdIn, int fdOut){
    if (cmd->inFile != NULL)
        dup2(cmd->rinFD, STDIN_FILENO);

    else if(cmd->inFile==NULL && cml->numOfCommands > 1 && fdIn!=0)
        dup2(fdIn, STDIN_FILENO);

    if (cmd->outFile != NULL)
        dup2(cmd->routFD, STDOUT_FILENO);

    else if(cmd->outFile == NULL && cml->numOfCommands>1 && fdOut!=1)
        dup2(fdOut,STDOUT_FILENO);

}