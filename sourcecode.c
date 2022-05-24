#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LIMIT 100






//this function parses the piped input as left side and right side 

void parsepiped(char** array ,int size ,char** l, char** r){

	
	int pipe = 0;
	
	int rightsize = 0;
	for (int i = 0; i < size-1; i++){
		
		
		if (strcmp(array[i],"|") == 0){
			pipe = 1;
			l[i] = NULL;
			
		}
		if (pipe){
			r[rightsize] = array[i+1];
			rightsize++;		
		}
		else {
			l[i] = array[i];
		}
	}
	r[rightsize] = NULL ;
	
	
	
}
//this function takes 2 parameter one of them  the is the strings at the left of the | symbol and the other one is the strings at the right of | symbol.
//child process execute the left side command and write it to the write end 
//parent process waits the child process and reads the left commands output from read end .

void execArgsPiped(char** parsed, char** parsedpipe)
{
    // 0 is read end, 1 is write end
    int pipefd[2]; 
    pid_t p1,p2 ;
  
    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }
  
    if (p1 == 0) {
        
        // It only needs to write at the write end
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
        
  
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute left command ..");
            exit(0);
        }
        
    } 
    else {
      	    p2 = fork();
      	    
      	    if (p2 < 0){
      	    	printf("\nCould not fork");
      	    	return;
      	    }	
      	    if (p2 == 0){
            	dup2(pipefd[0], STDIN_FILENO);
            	close(pipefd[0]);
            	close(pipefd[1]);
            	if (execvp(parsedpipe[0], parsedpipe) < 0) {
                	printf("\nCould not execute right command ..");
                	exit(0);
            	}
            	
        	
     		} 
            else{
            		close(pipefd[0]);
            		close(pipefd[1]);
     			wait(NULL);
            		wait(NULL);
     		}   
	}
 }	
  //this function check the given array contain "|" character
  
int checkforpipe(char** array , int arraysize ){
	for (int i = 0 ; i < arraysize ; i++){
		if (strcmp(array[i],"|") == 0 ){
			
			return 1;
		}
	}
	return 0;
}




int main(){

	int historyCount = 0;
	
	char * history[10];
	
	while(1){
		
		char * array[50];
		
		char str[MAX_LIMIT];	
		
		printf("myshell>");
		
		int len;
			
		fgets (str,MAX_LIMIT,stdin);
		
		len = strlen(str);
		
		//if there is a '\n' at the end of the input string ,I remove it.
		if ( str[len-1] == '\n'){
			str[len-1] = '\0';
		}

		//adding the input string to the history 
		if (historyCount == 10){
		
			for (int i = 1 ; i < 10 ; i++){
				history[i-1] = history[i] ;
		
			}
			history[9] = strdup(str);
		}
		else {
		
			history[historyCount] = strdup(str);
			historyCount++;

		}
		//here I parse the string that I get with fgets function and storing them in array. 
		char * token = strtok(str, " ");

		int arraysize = 0 ;
		
		while ( token != NULL ) {
			
			array[arraysize++] = token;
		
			token = strtok(NULL, " ");
			
		}

		array[arraysize] = NULL;

		int background = (strcmp(array[arraysize-1],"&") == 0);
		
		
		if (!checkforpipe(array,arraysize)){
		
			char s[100];
			
			if (strcmp(array[0],"cd") == 0 ){ //cd command
			
				if (array[2] == NULL){
					char * path = array[1] ;
			
					int ch = chdir(path) ;
			
					if (ch<0){
						perror("");
						char * home = getenv("HOME");
						chdir(home);
					}
					else{
						setenv("PWD", path, 1);
					}
				}
				else {
					printf("too much arguments for command: cd\n");
				}	
			}
			else if (strcmp(array[0],"dir") == 0 ){ // dir command
			
				if (getcwd(s, sizeof(s)) != NULL) {
					printf("Current working dir: %s\n", s);
				}
			
			}
			else if (strcmp(array[0],"history") == 0 ){ // history command
				printf("ı am history\n");
			
				for (int i = 0; i < historyCount ; i++){

					printf("[%d] %s\n",i,history[i] );
				}
			}

			else if (strcmp(array[0],"bye") == 0 ){ // bye command
				exit(0);
			}
			else{
			
				pid_t pid;
				pid = fork();

				if (pid < 0){
					fprintf(stderr, "Fork Failed");
					}
		 			//child process  
				if (pid == 0) {
					
				
					if (execvp(array[0], array) < 0) {
						printf(" command not found\n");
						
					}
					exit(0);
				} 
				else{
					wait(NULL);
				}
				
				
		
			}
		}
	else{
		char * left[5];
		char * right[5];


		parsepiped(array,arraysize, left,right);
		
		execArgsPiped(left,right); 
	}
	
	

	

	
	
}
	return 0;
}


 	
		
	
	
		
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

