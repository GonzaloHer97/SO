#include <stdio.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	FILE* file=NULL;
	int bufferSize = 1;
	//int c,ret;
	int size;
	
	if (argc<2) {
		fprintf(stderr,"Usage: %s <file_name>\n",argv[0]);
		exit(1);
	}

	/* Open file */
	if ((file = fopen(argv[1], "r")) == NULL)
		err(2,"The input file %s could not be opened",argv[1]);
	/*size of bytes in the loop*/
	if (argc>2){
		//bufferSize = atoi(argv[2]);
		if ((bufferSize = atoi(argv[2])) < 1){
			bufferSize = 1;
		}
	}

	printf("BufferSize: %d bytes\n", bufferSize);
	char buffer[bufferSize];
	/* Read file byte by byte */
	while ((size = fread(buffer,sizeof(char), bufferSize,file))>0){
			
        	if (fwrite( buffer, sizeof(char),size,stdout) < size)  {
            		perror("write");
            		fclose(file);			
            		exit(-1);
       	 	}	
	}
	// while ((c = getc(file)) != EOF) {
	// 	/* Print byte to stdout */
		
	// 	ret=putc((unsigned char) c, stdout);

	// 	if (ret==EOF){
	// 		fclose(file);
	// 		err(3,"putc() failed!!");
	// 	}
	// }

	fclose(file);
	return 0;
}
