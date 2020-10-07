// 
// 


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void
find(char *path, char* files){
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;
	int i;
	int isFound = 0;

	if((fd = open(path, 0)) < 0){
		fprintf(2, "find: cannot open %s\n", path);
		return;
	}

	if(fstat(fd, &st) < 0){
		fprintf(2, "find: cannot stat %s\n", path);
		close(fd);
		return;
	}

	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
		printf("find: path too long\n");
		return;
	}

	strcpy(buf, path);
	p = buf+strlen(buf);
	*p++ = '/';
	while(read(fd, &de, sizeof(de)) == sizeof(de)){
		if(de.inum == 0)
			continue;
		if(de.name[0] == '.' && de.name[1] == '\0')
			continue;
		if(de.name[0] == '.' && de.name[1] == '.' && de.name[2] == '\0')
			continue;
		
		if(strlen(de.name) != strlen(files)){
			isFound = 0;
		} else {
			for(i=0; i<strlen(files); i++){
				if(de.name[i] != files[i]){
					isFound = 0;
					break;
				} else {
					isFound = 1;
				}
			}	
		}
		
		memmove(p, de.name, DIRSIZ);
		p[DIRSIZ] = 0;
		if(stat(buf, &st) < 0){
			printf("find: cannot stat %s\n", buf);
			continue;
		}
		if(isFound){
			break;
		} else {
			if(st.type == T_DIR){
				find(buf, files);
				if(isFound)
					break;
			}
		}
	}
	if(isFound){
		printf("%s\n", buf);
	}
	return;
}

int
main(int argc, char *argv[]){
	
	if(argc < 2 || argc > 3){
		printf("Input invalid!\n");
		exit();
	}
	find(argv[1], argv[2]);
	
	exit();
}
