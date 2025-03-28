#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char* dir_path, char* file_name){
	struct stat st;
	struct dirent de;
	int fd;

	printf("=== find in dir: %s\n ===", dir_path);

	fd = open(dir_path, 0);
	if(fd < 0){
		printf("find: cannot open %s\n", dir_path);
		return;
	}

	if(fstat(fd, &st) < 0){
		printf("find: cannot stat %s\n", dir_path);
		close(fd);
		return;
	}
	
	if(st.type != T_DIR){
		close(fd);
		return;
	}
	
	// child direcotries
	while(read(fd, &de, sizeof(de)) == sizeof(de)){
		if(de.inum == 0) continue;
		
		if(strcmp(".", de.name) == 0 || strcmp("..", de.name) == 0)
			continue;

	        // fomating child file's path:
		// eg: ./file_name'\0'	
		char buf[512];
		char *p;
		strcpy(buf, dir_path);
		p = buf + strlen(buf);
		*p++ = '/';  // find: father/
		memmove(p, de.name, DIRSIZ);// find: father/child
		p[DIRSIZ] = 0; // add '\0'
		
		// find child in the given dir, print path
		if(strcmp(file_name, de.name) == 0){
			printf("%s\n", buf);
		}
		
		// stat child path
		if(stat(buf, &st) < 0){
			printf("find: cannot stat %s", buf);
		}
		
		if(st.type == T_DIR){
			find(buf, file_name);
		}
	}
	close(fd);
}


int main(int argc, char* argv[]){
	// find . b
	char* dir_path = argv[1];
	char* file_name = argv[2];
	find(dir_path, file_name);
	exit(0);
}
