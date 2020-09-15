// Create a sleep program
// Program to pause for a user-specified number of ticks

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
	int time;
	
	if(argc <= 1 || argc > 2){
		printf("Invalid input format!!!\n");
	} else {
		time = atoi(argv[1]);
		sleep(time);
	}
	exit();
}
