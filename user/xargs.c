// // 
// // 


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int 
main(int argc, char *argv[])
{
	char c;
	char buf2[32][32];
	char buf[512];
	char *arguments[MAXARG];
	int i, j, k, n;
	
	// // for debugging purposes
	// int l = 0;
	
	if(argc < 2){
		printf("xargs: input invalid!\n");
	}
	for(i = 1; i < argc; i++){
		strcpy(buf2[i-1], argv[i]);
		arguments[i-1] = &buf2[i-1][0];
	}
	
	i = 0;
	j = -1;
	k = 0;
	while((n = read(0, &c, 1)) > 0){
		if(c == ' '){
			buf[i] = '\0';
			arguments[argc + j] = &buf[k];
			j++;
			i++;
			k = i;
		} else if(c == '\n'){
			buf[i] = '\0';
			arguments[argc + j] = &buf[k];
			j++;
			arguments[argc + j] = 0;
			
			// // for debugging purposes
			// while(arguments[l] != 0){
				// printf("arguments[%d]: %s\n", l, arguments[l]);
				// l++;
			// }
			// l = 0;
			
			switch(fork()){
			case -1:
				printf("xargs: fork error\n");
				break;
			case 0: 
				exec(arguments[0], arguments);
				break;
			default:
				wait();
				break;
			}
			buf[0] = '\0';
			i = 0;
			j = -1;
			k = 0;
		} else {
			buf[i] = c;
			i++;
		}
	}
	if(n < 0){
		printf("xargs: read error\n");
	}
	exit();
}
