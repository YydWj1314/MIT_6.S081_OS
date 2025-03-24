#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int max_prime = 35;

int sieve(int fd_read){

	int p;	
	if(read(fd_read, &p, sizeof(p))  == 0){
		close(fd_read);
		exit(0);
	}
	
	printf("%d\n", p);
	
	int pip_next[2];
	pipe(pip_next);
	
	int pid = fork();
	if(pid == 0){
		close(pip_next[1]);
		close(fd_read);
		sieve(pip_next[0]);
		close(pip_next[0]);
		exit(0);

	}else{
		close(pip_next[0]);

		int n;
		while(read(fd_read, &n, sizeof(p)) > 0){
			if(n % p != 0){
				write(pip_next[1], &n, sizeof(n));
			}
		}
		close(fd_read);
		close(pip_next[1]);
		wait((int*)0);
		exit(0);
	}

}

int main(){
	int pip[2];
	pipe(pip);

	// Child process 
	if(fork() == 0){
		close(pip[1]);
		sieve(pip[0]);
		close(pip[0]);
		exit(0);	
	}
	// Parent process 
	else{
		close(pip[0]);
		// Writing 2~35 to pip
		for(int i = 2; i <= max_prime; i++){
			write(pip[1], &i, sizeof(i));
		}
		close(pip[1]);
		wait((int*)0);
		exit(0);
	}
}
