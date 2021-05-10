#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
	pid_t hijo;
	char * command; 
	int estado;

	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	command = argv[1];
	hijo = fork();

	if(hijo == 0){
		execl("/bin/bash", "bash","-c", command, (char *) NULL);

		return 1;
	
	}else{
		if(hijo == -1)
			return -1;
		
		while(waitpid(hijo, &estado, 0) == -1){		
			if(EINTR != errno){
					estado = -1;
					break;
			}		
		}
	}


	return estado;
}

