// 
// 


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void child(int*);

int 
main(int argc, char *argv[]){
	int p[2];
	int i;
	int stopbit = 255;
	int limit = 36;
	
	pipe(p);
	switch(fork()){
		case -1:
			// Do nothing
			break;
		case 0: 
			close(p[1]);
			child(&p[0]);
			close(p[0]);
			break;
		default:
			close(p[0]);
			for(i = 2; i < limit; i++){
				write(p[1], &i, 1);
			}
			write(p[1], &stopbit, 1);
			wait();
			close(p[1]);
			break;
	}
	exit();
}

void 
child(int *p){
	int cp[2];
	int icbuff; 
	int icbuff2;

	read(*p, &icbuff, 1);
	printf("%d: prime %d\n", getpid(), icbuff);
	read(*p, &icbuff2, 1);
	if(icbuff2 == 255){
		exit();
	}
	if(icbuff2 % icbuff != 0){
		pipe(cp);
		switch(fork()){
			case -1:
				// Do nothing
				break;
			case 0:
				close(cp[1]);
				child(&cp[0]);
				close(cp[0]);
				break;
			default:
				close(cp[0]);
				write(cp[1], &icbuff2, 1);
				while(1){
					read(*p, &icbuff2, 1);
					if((icbuff2 % icbuff != 0) || (icbuff2 == 255)){
						write(cp[1], &icbuff2, 1);
					}
					if(icbuff2 == 255){
						wait();
						exit();
					}
				}
				wait();
				close(cp[1]);
				break;	
		}
		exit();
	} else {
		exit();
	}
}
