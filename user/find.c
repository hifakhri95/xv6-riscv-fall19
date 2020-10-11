// 
// 


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

// function declaration
char* fmtname(char *path);
int match(char *re, char *text);
int matchhere(char *re, char *text);
int matchstar(int c, char *re, char *text);

void
find(char *path, char* files, int* pnotif){
	char buf[512], *p;
	int fd;
	struct dirent de;
	struct stat st;
	/*int i;*/
	/*int isFound = 0;*/

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
		
		memmove(p, de.name, DIRSIZ);
		p[DIRSIZ] = 0;
		if(stat(buf, &st) < 0){
			printf("find: cannot stat %s\n", buf);
			continue;
		}
		
		switch(st.type){
		case T_FILE:
			if(match(files, fmtname(buf))){
				printf("%s\n", buf);
				*pnotif = 1;
			}
			break;
		case T_DIR:
			find(buf, files, pnotif);
			break;
		default:
			break;
		}
		
	return;
}

int
main(int argc, char *argv[]){
	int notif;
	
	if(argc < 2 || argc > 3){
		printf("find: Input invalid!\n");
		exit();
	}
	find(argv[1], argv[2], &notif);
	if(!notif){
		printf("find: cannot find what you are looking for\n");
	}
	
	exit();
}

// additional function to support regular expressions
char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

int
match(char *re, char *text)
{
  if(re[0] == '^')
    return matchhere(re+1, text);
  do{  // must look at empty string
    if(matchhere(re, text))
      return 1;
  }while(*text++ != '\0');
  return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
  if(re[0] == '\0')
    return 1;
  if(re[1] == '*')
    return matchstar(re[0], re+2, text);
  if(re[0] == '$' && re[1] == '\0')
    return *text == '\0';
  if(*text!='\0' && (re[0]=='.' || re[0]==*text))
    return matchhere(re+1, text+1);
  return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
  do{  // a * matches zero or more instances
    if(matchhere(re, text))
      return 1;
  }while(*text!='\0' && (*text++==c || c=='.'));
  return 0;
}
