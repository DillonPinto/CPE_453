#include <stdlib.h>
#include "shellDriver.h"

int main(){

    CommandLine cml, prevCml;
    // Initialize the previous command line struct, i.e set members to defaults
    initCommandLine(&prevCml);
    setbuf(stdout,NULL);

    while (TRUE) {

        printf("osh> ");
        fflush(stdout);

        // Read the input from the user and check if error was returned.
        // True represents an error
        if(readCommandLine(&cml, &prevCml)) {
            // clear the buffer if characters entered
            if(strlen(cml.line)>0){
                clearBuffer();
            }
            continue; // restart
        }

        // if repeat is not required then parse command line
        if (!cml.repeatCommand) {
            // tokenize commands
            if(getCommands(&cml)){
                continue;
            }

            // tokenize arguments
            if (getCommandArgs(&cml)) {
                continue;
            }

            // save the command line for the next iteration
            prevCml = cml;
        }

        // repeat last command
        else {
            // set current command line to previous commands
            cml = prevCml;
        }

        // launch pipelien and processes
        launchPipeline(&cml);

    }




    return EXIT_SUCCESS;
}


