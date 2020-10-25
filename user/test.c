// // 
// // 


#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

// fork test 

int
main(int argc, char *argv[]){

	if(!fork()){
		fprintf(2, "bagian atas");
	} else if(!fork()){
		fprintf(2, "bagian bawah");
	}
	
	
	exit();
}