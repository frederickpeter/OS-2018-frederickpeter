#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include<pthread.h>

char *binParams[20];
char *usrParams[20];
char *allArguments[10];
char *parallelArrayBin[20];
char *parallelArrayUsr[20];

char bin[600] = "";
char usr[600] = "";
char redirection[600] = "";
char filename[600] = "";
char parallelCommand[600] = "";

pthread_t myThread[10];
int k=0;

void callError(){
	char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}


void parseBatch(char * command, char *params[], char *binParams[], char *usrParams[]){
	char bin2[600] = "/bin/";
	char usr2[600] = "/usr/bin/";
	binParams[0] = bin2;
	usrParams[0] = usr2;

    //For breaking the input and adding it to the array
	for(int i =0; i<10; i++){
		params[i] = strsep(&command, " ");
		params[i] = strtok(params[i], "\n");
		params[i] = strtok(params[i], "\r");

		if(params[i] == NULL) break;
	}
	//concatenate the path with the input
	strcat(bin2, params[0]);
    strcat(usr2, params[0]);
    //for the path with /bin/
	for(int i =1; i<10; i++){
		binParams[i] = params[i];
		if(binParams[i] == NULL) break;	
	}
	//for the path with usr/bin/
	for(int i =1; i<10; i++){
		usrParams[i] = params[i];
		if(binParams[i] == NULL) break;
		
	}
}



void parseCommand(char * command, char *params[], char *binParams[], char *usrParams[]){

    //For breaking the input and adding it to the array
	for(int i =0; i<10; i++){
		params[i] = strsep(&command, " ");
		params[i] = strtok(params[i], "\n");
		params[i] = strtok(params[i], "\r");

		allArguments[i] = params[i];

		if(params[i] == NULL) break;
		
	}

	//check if it has a redirect sign and set variable to 'redirect'
	for(int i=0; i<10; i++){
		if(params[i]==NULL)break;

		if(strcmp(params[i], ">")==0){
			strcpy(redirection, "redirect");
			strcpy(filename, params[i+1]);
			params[i] = NULL;
			params[i+1] = NULL;
		}
		else{
			strcpy(redirection, "");
		}

	}

	//check if it has ampersand and set variable to 'parallel'
	for(int i=0; i<10; i++){

		if(params[i]==NULL)break;

		if(strcmp(params[i], "&") ==0){
			strcpy(parallelCommand, "parallel");
			break;
		}
		else{
			strcpy(parallelCommand, "");
		}
	}

	// //concatenate the path with the input
	if(strcmp(params[0], "path")!=0){
		if(strcmp(bin, "/bin/")==0){
			strcat(bin, params[0]);
			binParams[0] = bin;
		}
		else if(strcmp(bin, "")==0){
			strcat(bin, params[0]);
			binParams[0] = bin;
		}
		else{
			if(strstr(bin, "/bin/")){
				strcpy(bin, "");
				strcpy(bin, "/bin/");
				strcat(bin, params[0]);
				binParams[0] = bin;
			}
		}

		if(strcmp(usr, "/usr/bin/")==0){
			strcat(usr, params[0]);
			usrParams[0] = usr;
		}
		else if(strcmp(usr, "")==0){
			strcat(usr, params[0]);
			usrParams[0] = usr;
		}
		else{
			if(strstr(usr, "/usr/bin/")){
				strcpy(usr, "");
				strcpy(usr, "/usr/bin/");
				strcat(usr, params[0]);
				usrParams[0] = usr;
			}
		}
		
 	}


    //for the path with /bin/
	for(int i =1; i<10; i++){
		//printf("%s", binParams[0]);
		binParams[i] = params[i];
		if(binParams[i] == NULL) break;
		
	}

	//for the path with usr/bin/
	for(int i =1; i<10; i++){
		usrParams[i] = params[i];
		if(usrParams[i] == NULL) break;
		
	}


}

int executeCmd(){

	pid_t pid = fork();
	// Child process
	if (pid == 0) {

		if(strcmp(redirection, "redirect")==0){
			FILE* file = fopen(filename, "w+");
			dup2(fileno(file), fileno(stdout));
			dup2(fileno(file), fileno(stderr));

			fclose(file);
		}

		// Execute command
		if(!execv(binParams[0], binParams))
			printf("%s\n", "failed");
		else
			execv(usrParams[0], binParams);

		// Error occurred
		callError();

		return 0;
	}

	// Parent process
    else {
        // Wait for child process to finish
        int childStatus;
        waitpid(pid, &childStatus, 0);
        return 1;
    }

}


void* executeParallel(){
	pid_t pid = fork();
	// Child process
	if (pid == 0) {

		// Execute command
		if(!execv(parallelArrayUsr[0], parallelArrayUsr))
			printf("%s\n", "failed");
		else
			execv(parallelArrayBin[0], parallelArrayBin);

		callError();

		return 0;
	}
	// Parent process
	 else {
	// // Wait for child process to finish
	 	int childStatus;
	 	waitpid(pid, &childStatus, 0);
	 }
}


int main(int argc, char *argv[]){
	size_t buffer_size = 600;
	char buffer[buffer_size];
	char buff[100];
	char *params[10];
	int fd[2];
	char *buffer2 = malloc(buffer_size * sizeof(char));


	if(argc==1){
		while(1){
			printf("%s", "wish> ");

			if(fgets(buffer, buffer_size, stdin) ==NULL)
				return(0);

			parseCommand(buffer, params, binParams, usrParams);

			if(strcmp(params[0], "exit")==0){
				exit(0);
			}

			else if(strcmp(params[0], "cd")==0){
				if(params[2]==NULL){
            		char *c = strtok(params[1], "\n");
            		if(chdir(c)==-1)
            			callError();
            		else{
            			printf("%s\n","successfully changed directory");
            		}
            	}
            	else
            		callError();	
			}

			else if(strcmp(params[0], "path")==0){
				if(params[1]==NULL){
					strcpy(bin, "");
					strcpy(usr, "");
				}
				else if(params[2]==NULL){

					if(strcmp(params[1], "/bin/")==0){
						strcpy(bin, "");
						strcpy(bin, "/bin/");
					}

					if(strcmp(params[1], "/usr/bin/")==0){
						strcpy(usr, "");
						strcpy(usr, "/usr/bin/");
					}
				}

				else if(params[3]==NULL){

					if(strcmp(params[2], "/usr/bin/")==0){
						strcpy(usr, "");
						strcpy(usr, "/usr/bin/");
					}

					if(strcmp(params[2], "/bin/")==0){
						strcpy(bin, "");
						strcpy(bin, "/bin/");
					}
				}
				
			}

			else if(strcmp(parallelCommand, "parallel")==0){

				int j =0;
				k =0;
				char bintest[600] = "";
				char usrtest[600] = "";

				for(int i =0; i<10; i++){

					if(allArguments[i]==NULL){
						pthread_create(&myThread[i], NULL, executeParallel, NULL);
						pthread_join(myThread[i], NULL);
						break;
					}

					if(strcmp(allArguments[i], "&")!=0){

						if(j ==0){

						 strcpy(bintest, "");
						 strcpy(bintest, "/bin/");
						 parallelArrayBin[j] = allArguments[i];
						 strcat(bintest, parallelArrayBin[0]);
						 parallelArrayBin[0] = bintest;

						 strcpy(usrtest, "");
						 strcpy(usrtest, "/usr/bin/");
						 parallelArrayUsr[j] = allArguments[i];
						 strcat(usrtest, parallelArrayUsr[0]);
						 parallelArrayUsr[0] = usrtest;

						}
						else{

						parallelArrayBin[j] = allArguments[i];
						parallelArrayUsr[j] = allArguments[i];

					}
						j++;
					}

					else{

						j=0;

						pthread_create(&myThread[i], NULL, executeParallel, NULL);
						pthread_join(myThread[i], NULL);

						memset(parallelArrayBin, 0, sizeof parallelArrayBin);
						memset(parallelArrayUsr, 0, sizeof parallelArrayUsr);	
					}
				}

			}

			else if(executeCmd() ==0){
				exit(0);
			}
		}
	}else if(argc==2){
		
		FILE *fp = fopen(argv[1], "r");
		if (fp == NULL) {
    			callError();
    			exit(1);
		}else{
			while(-1 != getline(&buffer2, &buffer_size, fp)){

				parseBatch(buffer2, params, binParams, usrParams);
			
				 if(strcmp(params[0], "cd")==0){
					if(params[2]==NULL){
	            		char *c = strtok(params[1], "\n");
	            		if(chdir(c)==-1)
	            			callError();
	            		else{
	            			callError();
	            		}
	            	}
	            	else
	            		callError();
				}

				else if(executeCmd() ==0){
				exit(0);
				}
			}
		}
		fclose(fp);
	}
	else{
		exit(1);

	}
	return(0);

}



