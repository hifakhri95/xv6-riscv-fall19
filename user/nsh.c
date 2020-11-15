#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


// Global variable for user input command Handlerr
char inputBuffer[1024];
char *pInputBuffer, *isTherePipe;

// Function declaration
char *pipecommandHandlerr(char *);
char *trim(char *);
void redirect(int, int *);
void commandHandler(char *);
void redirectcommandHandlerr(void);

int main(int argc, char *argv[]){
    while (1){
        fprintf(1, "@ ");
        memset(inputBuffer, 0, 1024);
        gets(inputBuffer, 1024);

		// If user input is empty, then exit immediately
		if(inputBuffer[0] == 0)
			exit();

		// replace user '\n' (enter) input with '\0' (EOF)
		*strchr(inputBuffer, '\n') = '\0';


		switch(fork()){
			case -1:
				fprintf(1, "nsh: fork error!\n");
				break;
			case 0:
				pInputBuffer= inputBuffer;
				// get an argument after '|' (pipe) if any
				isTherePipe = pipecommandHandlerr(pInputBuffer);
				// commandHandler the redirection and comands
				redirectcommandHandlerr();
				break;
			default:
				wait();
				break;
		}
    }

    exit();
}

char *pipecommandHandlerr(char *pointer){
    while (*pointer != '\0' && *pointer != '|')
        pointer++;
	
	// if can't find '|' (no pipe), return 0
    if (*pointer == '\0')
        return 0;

	// if there is pipe, replace '|' with '\0'
    *pointer = '\0';

    return pointer + 1;
}

char *trim(char *c){
    char *e = c;
    while (*e)
        e++;
    while (*c == ' ')
        *(c++) = '\0';
    while (*(--e) == ' ')
        ;
    *(e + 1) = '\0';
    return c;
}

void redirect(int k, int pd[])
{
    close(k);
    dup(pd[k]);
    close(pd[0]);
    close(pd[1]);
}

void redirectcommandHandlerr(){
    if (pInputBuffer){
		// allocate pipe to do interprocess communication
        int pd[2];
        pipe(pd);

        if(!fork()){
            // if there is a pipe, redirect stdout into pipe
            if(isTherePipe) 
				redirect(1, pd);
            commandHandler(pInputBuffer);
        } else if(!fork()) {
            if(isTherePipe) {
				// redirect stdin into pipe
                redirect(0, pd);
				// try to find out and commandHandler if there is another level of pipeline
                pInputBuffer = isTherePipe;
                isTherePipe = pipecommandHandlerr(pInputBuffer);
                redirectcommandHandlerr();
            }
        }

		// close unused fd and wait child process to finsih
        close(pd[0]);
        close(pd[1]);
        wait();
        wait();
    }

    exit();
}

void commandHandler(char *input)
{
    char argBuffer[32][32];
    char *argPointer[32];
    int argc = 0;

	// trim spaces before and after arguments
    input = trim(input);

	// point every single argBuffer element with argPointer
    for (int i = 0; i < 32; i++)
        argPointer[i] = argBuffer[i];

	// helper pointer to move arguments from input into argBuffer
    char *c = argBuffer[argc];
	
    int inputFile = 0; 
	int outputFile = 0;
	
    for (char *p = input; *p; p++)
    {
        if (*p == ' ' || *p == '\n'){
			// if spaces found, replace it with EOF
            *c = '\0';
			// point to the next buffer
            argc++;
            c = argBuffer[argc];
        } else {
			// if there is file redirection. then take that file argument
            if(*p == '<') {
                inputFile = argc + 1;
            } if(*p == '>') {
                outputFile = argc + 1;
            }
			
			// if input char is not spaces, then move input char into argBuffer
            *c++ = *p;
        }
    }
	
	// put EOF in the last argument
    *c = '\0';
	
	// fill the buffer after the last argument with 0
    argc++;
    argPointer[argc] = 0;

	// redirect stdout and stdin into file if any 
    if(inputFile) {
        close(0);
        open(argPointer[inputFile], O_RDONLY);
    }
    if(outputFile) {
        close(1);
        open(argPointer[outputFile], O_WRONLY | O_CREATE);
    }

	// skip the '<' and '>' sign argument, also skip file name argument 
	// for executing command
    char *argPointer2[32];
    int argc2 = 0;
    for(int j = 0; j < argc; j++) {
        if(j == inputFile - 1) 
			j += 2;
        if(j == outputFile - 1) 
			j += 2;
        argPointer2[argc2++] = argPointer[j];
    }
    argPointer2[argc2] = 0;


	switch(fork()){
		case -1:
			fprintf(1, "nsh: fork error!\n");
			break;
		case 0:
			exec(argPointer2[0], argPointer2);
			break;
		default:
			wait();
			break;
	}
}
