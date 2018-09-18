#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char buffer[600];
	int index =2;
	size_t buffer_size =600;
	char *buffer2 = malloc(buffer_size * sizeof(char));

	if(argc==1){
		printf("my-grep:searchterm [file...]\n");
		exit(1);
	}
	if(argc==2){
		while(fgets(buffer, 600, stdin)){
			if (strstr(buffer, argv[1]) != NULL) {
   				 printf("%s", buffer);
			}
			return(0);
		}
	}
	while(argc>index){

		FILE *fp = fopen(argv[index], "r");
		if (fp == NULL) {
    			printf("my-grep: cannot open file\n");
    			exit(1);
		}else{
			while(-1 != getline(&buffer2, &buffer_size, fp)){
				if(strstr(buffer2, argv[1])!=NULL){
					if(argc==3){
						printf("%s", buffer2);
					}
					else
						printf("%s: %s", argv[index], buffer2);
				}
			}
		}
		index++;
		fclose(fp);
	}
   	return(0);
}
