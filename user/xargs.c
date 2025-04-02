#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("xargs: Invalid params");
		exit(1);
	}
	
	char *exec_args[MAXARG];
	//  copy initial cmd-line args into  exex_args[]  
	for(int i = 1; i < argc; i++){
		exec_args[i - 1] = argv[i];	
	}
	char **p_exec = &exec_args[argc - 1];

	char c;
	char arg_split[64];
	int st = 0;
	char* args[MAXARG];
	char** p_args = args; 
	char buf[512];
	char* p_buf = buf;

	// Read line: pipe_out(fd = 0) ==c,c,c...==> buf[512]
	while(read(0, &c, 1) != 0){
		if(c == ' ' || c == '\n'){
			// add '0', copy split --> exec_args[]
			arg_split[st] = '\0';

			// add arg_split to args: {..., char arg_split[]}
			strcpy(p_buf, arg_split);
			*p_args = p_buf;
			p_buf += strlen(arg_split) + 1;
			p_args++;
			st = 0;
			
			if(c == '\n'){
				// add args[] --> exec_args:{..., char* args[1], char* args[2]..., 0}
				for(char** p = args; p < p_args; p++){
					*p_exec = *p;
					p_exec++;
				}
				*p_exec = 0;		
				if(fork() == 0){
					exec(argv[1], exec_args);
					printf("xarg: exec failed\n");
					exit(1);
				}else{
					wait(0);
					p_args = args;
					p_exec = &exec_args[argc - 1];	
				}

			}
		
		}else {
			arg_split[st++] = c;
		}
	}
	exit(0);
}
