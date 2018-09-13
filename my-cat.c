#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	char buffer[600];
	int index =1;

	if(argv[1]==NULL)
		return(0);
	while(index<argc){

		FILE *fp = fopen(argv[index], "r");
		if (fp == NULL) {
    			printf("my-cat: cannot open file\n");
    			exit(1);
		}else{
			while(!feof(fp)){
				if( fgets (buffer, 600, fp)!=NULL ) {
					printf("%s",buffer);
   				}
			}
		}
		index++;
		fclose(fp);
	}
   //	fclose(fp);
   	return(0);
}
