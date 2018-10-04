#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

char *binParams[20];
char *usrParams[20];

char bin[600] = "tes";
char usr[600] = "tes";



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
		//printf("%s", binParams[0]);
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
		if(params[i] == NULL) break;
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
			strcpy(bin, "/bin/");
			strcat(bin, params[0]);
			binParams[0] = bin;
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
			strcpy(usr, "/usr/bin/");
			strcat(usr, params[0]);
			usrParams[0] = usr;
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

    // Fork process
    pid_t pid = fork();



    // Child process
    if (pid == 0) {

        // Execute command
        if(!execv(binParams[0], binParams))
        	printf("%s\n", "failed");
        else
        	execv(usrParams[0], binParams);


        // Error occurred
         char* error = strerror(errno);
    	 printf("\n%s\n", error);
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
            			printf("%s\n", "Please, the directory does not exist");
            		else{
            			printf("%s\n","successfully changed directory");
            		}
            	}
            	else
            		printf("%s\n", "Please, cd takes only one parameter");	
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
						printf("%s\n", bin);
					}

					if(strcmp(params[1], "/usr/bin/")==0){
						strcpy(usr, "");
						strcpy(usr, "/usr/bin/");
						printf("%s\n", usr);
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

			else if(executeCmd() ==0){
				exit(0);
			}
		}
	}else if(argc==2){
		

		FILE *fp = fopen("batch.txt", "r");
		if (fp == NULL) {
    			printf("cannot open file\n");
    			exit(1);
		}else{
			while(-1 != getline(&buffer2, &buffer_size, fp)){

				parseBatch(buffer2, params, binParams, usrParams);
			
				 if(strcmp(params[0], "cd")==0){
					if(params[2]==NULL){
	            		char *c = strtok(params[1], "\n");
	            		if(chdir(c)==-1)
	            			printf("%s\n", "Please, the directory does not exist");
	            		else{
	            			printf("%s\n","successfully changed directory");
	            		}
	            	}
	            	else
	            		printf("%s\n", "Please, cd takes only one parameter");	
				}

				else if(executeCmd() ==0){
				exit(0);
				}
			}
		}
		fclose(fp);
	}
	

	return(0);

}



