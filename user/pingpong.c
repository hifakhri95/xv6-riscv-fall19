// 
// 

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int 
main(int argc, char *argv[])
{
	int parent_fd[2];
	int child_fd[2];
	char *buf[2];
	
	buf[0] = "";
	buf[1] = "";
	
	pipe(parent_fd);
	pipe(child_fd);
	if(fork() > 0){
		write(parent_fd[1], "ping", 5);
		wait();
		read(child_fd[0], buf[0], 5);
		printf("%d: recieved %s", getpid(), buf[0]);
		printf("\n");
		close(parent_fd[1]);
		close(child_fd[1]);
	} else {
		read(parent_fd[0], buf[0], 5);
		printf("%d: recieved %s", getpid(), buf[0]);
		printf("\n");
		write(child_fd[1], "pong", 5);
		close(parent_fd[1]);
		close(child_fd[1]);
		exit();
	}
	exit();
}
