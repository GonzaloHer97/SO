#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    FILE* file = NULL;
    int c,ret;

    if(argc != 2){
        fprintf(stderr, "Usage: %s <file_name> \n", argv[0]);
        exit(1);
    }


    if((file =fopen(argv[1],"r" )) == NULL){
        err(2,"THe input file %s could not be opened",argv[1]);
    }

    int ret2 = fread(&c,sizeof(int),1,file);
    while( ret2 == 1 ){
        ret = fwrite(&c,sizeof(int),1,stdout);
        if(ret != 1){
            fclose(file);
            err(3, "fwrite() failed!!!");
        }
        ret2 = fread(&c,sizeof(int),1,file);
    }
    fclose(file);
    return 0;
}