#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(){
	int p1[2];
	int p2[2];
	char buffer[64];
	
	pipe(p1);
	pipe(p2);
	// 1.Parent sending byte to the child
	if(fork() == 0){
		// Child reading from the pipe1
		close(p1[1]);  // close pipe1_write
		
		close(0);      // close default stdin
		dup(p1[0]);    // copy pipe1_read 
		close(p1[0]);  // close pipe1_read
			       //
		read(0, buffer, sizeof(buffer));  // read "ping" from pipe
		printf("%d: received %s", getpid(), buffer);

		// Child writting to the pipe --> parent
		close(p2[0]);   // closing pipe2_read 
		write(p2[1], "pong\n", 5); // write to pipe2
		close(p2[1]);  // close pipe2_write;
		exit(0);	
	} else{
		// Parent writting to pipe1
		close(p1[0]);
		write(p1[1], "ping\n", 5);
		close(p1[1]);
		
		// Parent reading from pipe2
		close(p2[1]);
		
		close(0);
		dup(p2[0]);
		close(p2[0]);
		
		read(0, buffer, sizeof(buffer));
		printf("%d: received %s", getpid(), buffer);
		wait(0);
		exit(0);
	}

}
