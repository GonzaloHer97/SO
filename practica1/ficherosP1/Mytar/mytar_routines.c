#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mytar.h"

extern char *use;

/** Copy nBytes bytes from the origin file to the destination file.
 *
 * origin: pointer to the FILE descriptor associated with the origin file
 * destination:  pointer to the FILE descriptor associated with the destination file
 * nBytes: number of bytes to copy
 *
 * Returns the number of bytes actually copied or -1 if an error occured.
 */
int
copynFile(FILE * origin, FILE * destination, int nBytes)
{
	int aux;
	int cont = 0;
	if(origin == NULL || destination == NULL)
		return -1;
	else{
		while(aux != EOF && cont < nBytes){
			aux = getc(origin);
			if(aux!= EOF){
				putc(aux, destination);
				cont++;
			}
		}
		return cont;
	}
}

/** Loads a string from a file.
 *
 * file: pointer to the FILE descriptor 
 * 
 * The loadstr() function must allocate memory from the heap to store 
 * the contents of the string read from the FILE. 
 * Once the string has been properly built in memory, the function returns
 * the starting address of the string (pointer returned by malloc()) 
 * 
 * Returns: !=NULL if success, NULL if error
 */
char*
loadstr(FILE * file)
{
	char aux;
	long int tam = 0;
	do
	{
		aux = getc(file);
		tam++;
	} while (aux != '\0' && aux != EOF);

	if(aux == EOF || tam == 1)
		return NULL;
	
	char* result = malloc(tam);

	fseek(file, -tam, SEEK_CUR);

	for(long int i = 0; i < tam;i++){
		result[i] = getc(file);
	}
	
	
	return result;
}

/** Read tarball header and store it in memory.
 *
 * tarFile: pointer to the tarball's FILE descriptor 
 * nFiles: output parameter. Used to return the number 
 * of files stored in the tarball archive (first 4 bytes of the header).
 *
 * On success it returns the starting memory address of an array that stores 
 * the (name,size) pairs read from the tar file. Upon failure, the function returns NULL.
 */
stHeaderEntry*
readHeader(FILE * tarFile, int *nFiles)
{
	fread(nFiles,sizeof(int),1,tarFile);
	stHeaderEntry *header = malloc(sizeof(stHeaderEntry) * (*nFiles));
	for(int i =0; i < *nFiles;i++){
		header[i].name = loadstr(tarFile);
		if(header[i].name == NULL)
			return NULL;
		fread(&header[i].size, sizeof(int), 1, tarFile);
	}

	return header;
}

/** Creates a tarball archive 
 *
 * nfiles: number of files to be stored in the tarball
 * filenames: array with the path names of the files to be included in the tarball
 * tarname: name of the tarball archive
 * 
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First reserve room in the file to store the tarball header.
 * Move the file's position indicator to the data section (skip the header)
 * and dump the contents of the source files (one by one) in the tarball archive. 
 * At the same time, build the representation of the tarball header in memory.
 * Finally, rewind the file's position indicator, write the number of files as well as 
 * the (file name,file size) pairs in the tar archive.
 *
 * Important reminder: to calculate the room needed for the header, a simple sizeof 
 * of stHeaderEntry will not work. Bear in mind that, on disk, file names found in (name,size) 
 * pairs occupy strlen(name)+1 bytes.
 *
 */
int
createTar(int nFiles, char *fileNames[], char tarName[])
{
	FILE *mTar = fopen(tarName, "w");
	stHeaderEntry *header = malloc(sizeof(stHeaderEntry)*nFiles);

	char aux = sizeof(int) + (nFiles * sizeof(unsigned int));
	for(int i = 0; i < nFiles;i++){
		aux  += strlen(fileNames[i]) + 1;
	}

	fseek(mTar, aux, SEEK_SET);

	for(int i = 0; i < nFiles; i++){
		FILE *f = fopen(fileNames[i], "r");

		header[i].name = malloc(strlen(fileNames[i]) + 1);
		strcpy(header[i].name, fileNames[i]);

		int num = copynFile(f, mTar, INT_MAX);

		if(num == -1){
			printf("Error: el arcihvo %s no encontrado. \n", fileNames[i]);
			fclose(f);
			return EXIT_FAILURE;
		}
		header[i].size = num;
		fclose(f);
	}

	fseek(mTar, 0, SEEK_SET);

	fwrite(&nFiles, sizeof(int), 1, mTar);
	for(int i = 0; i < nFiles;i++){
		fwrite(header[i].name, strlen(header[i].name),1, mTar);
		fwrite("\0", sizeof(char),1, mTar);
		fwrite(&header[i].size, sizeof(int),1, mTar);
	}


	for(int i =0 ;i < nFiles;i++)
		free(header[i].name);

	free(header);

	fclose(mTar);
	printf("Archivo %s generado con éxito \n", tarName);

	return EXIT_SUCCESS;
}

/** Extract files stored in a tarball archive
 *
 * tarName: tarball's pathname
 *
 * On success, it returns EXIT_SUCCESS; upon error it returns EXIT_FAILURE. 
 * (macros defined in stdlib.h).
 *
 * HINTS: First load the tarball's header into memory.
 * After reading the header, the file position indicator will be located at the 
 * tarball's data section. By using information from the 
 * header --number of files and (file name, file size) pairs--, extract files 
 * stored in the data section of the tarball.
 *
 */
int
extractTar(char tarName[])
{
	FILE* mtar = fopen(tarName,"r");

	if(mtar == NULL){
		printf("El archivo %s no se ha encontrado \n",tarName );
		return EXIT_FAILURE;
	}


	int* nFiles = malloc(sizeof(int));
	stHeaderEntry* header = readHeader(mtar, nFiles);
	

	if(header == NULL){
		printf("La cabecera del archivo %s no se ha podido leer correctamente \n",tarName );
		return EXIT_FAILURE;
	}
	
	for(int i = 0; i < *nFiles; i++){
		FILE* aux = fopen(header[i].name, "w");
		unsigned long int nB = header[i].size;
		char b = copynFile(mtar, aux, nB);
		fclose(aux);
		if(b == -1){
			printf("Error al extraer el fichero %s \n",header[i].name );
			return EXIT_FAILURE;
		}
		
	}

	printf("%d ficheros extraidos con éxito del archivo %s\n", *nFiles, tarName);
	fclose(mtar);

	return EXIT_SUCCESS;
}
