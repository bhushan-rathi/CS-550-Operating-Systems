#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main ( int argc, char *argv[] )
{
		int pid, ppid, H;

		   
 	if ( argc != 3 ) // argc should be 3 for correct execution
    	{
        	printf( "Please enter 3 argument: programs .o file, Height of the tree and number of childrens you want to create from every single process upto heightyou want\n" );
		exit(1);
    	}
	else
	{	
		if ( atoi(argv[1]) == 1){//place to introduce logic if we want excute additional thing at Height 1
		}
		H = atoi(argv[1]);
		char converter[25];
		sprintf(converter, "%d", H-1); 
		char *argvNew[] = {argv[0], converter, argv[2], 0};		
		printf("(%d): process starting\n", getpid());
		printf("(%d): Parent's id = (%d)\n", getpid(), getppid() ); 
		printf("(%d): Height in the tree = (%d)\n", getpid(), atoi(argv[1]));
		printf("(%d): Creating %d children at height %d\n", getpid(), atoi(argv[2]), H-1);

		if(H>1){//if Height is greate than 1 then only need to call fork() and exec()
			int i;
			for(i = 0; i < atoi(argv[2]); i++){
				pid = fork();
				if(pid == 0){
					//pid will return 0 which means the process is child process and it will create new process with execv() fuction call
					int j; 
					j =  execvp(argv[0], argvNew);
					if(j==-1){
						exit(1);					
					}

				}
				if(pid>0){
					// to catch parent process and wrting logic if we want to assign some task to parent process
				}
				if(pid == -1){
					//program will exit if process creation failed or tried to goto infinite loop as we are decreasing the height	
					exit(1);
				}
			}
			while(wait(NULL)<0);//terminating all the processes
		}
				
	}
	printf("(%d): Terminating at height %d\n", getpid(), H);
		
	return 0;
}
