#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int 
isTherePipe(char *input){
	char *p = input;
	
	while(*p != '\0'){
		p++;
		if(*p == '|')
			return 1;
	}
	
	return 0;
}

void inputParser(char *input){
	
}

void
commandHandler(char *input){
	
}

void
pipelineHandler(char *input){
	char buffer[1024];
	char *input1 = buffer;
	char *p;
	char *input2;
	int pd[2];
	
	strcpy(input1, input);
	p = input1;
	
	while(*p != '|')
		p++;
	*p = '\0';
	
	// point input2 to an adress after '|' symbol
	input2 = p+1;
	
	// Trimming spaces at the end of command
	while(*(--p) == ' ')
		*p = '\0';
	
	// Trimming spaces at the beginning of command
	while(*input2 == ' ')
		input2++;
	
	pipe(pd);
	switch(fork()){
	case -1:
		printf("error: fork error\n");
		break;
	case 0: 
		close(0);
		dup(p[0]);
		close(p[1]);
		fprintf(1, "input2: %s\n", input2);
		commandHandler(input2);
		exit();
		break;
	default:
		close(1);
		dup(p[1]);
		close(p[0]);
		fprintf(1, "input1: %s\n", input1);
		commandHandler(input1);
		wait();
		close(p[1]);
		break;
	}
}

int 
main(int argc, char *argv[]){
	char input_buffer[1024];
	char *ipointer;
	
    while (1)
    {
        fprintf(1, "@ ");
        memset(input_buffer, 0, 1024);
        gets(input_buffer, 1024);

        if (input_buffer[0] == 0)
			exit();
		   
        *strchr(input_buffer, '\n') = '\0';
		ipointer = input_buffer;

		switch(fork()){
		case -1:
			printf("error: fork error\n");
			break;
		case 0: 
			if(isTherePipe(ipointer)){
				pipelineHandler(ipointer);
			} else {
				commandHandler(ipointer);
			}
			exit();
			break;
		default:
			wait();
			break;
		}
    }

    exit();
}