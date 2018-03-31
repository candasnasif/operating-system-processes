#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void matrix_multiplication(int **a, int size);
int size_of_matrix();
void create_matrix(int **matrix, int size);
void print_matrix(int **matrix, int size, int fileNumber, int pid);
int main ( int argc, char *argv[] )
{
    int f; 
	int i;
    int size = size_of_matrix();
    int **fd = (int**)malloc(atoi(argv[1])*sizeof(int*));
    for (i = 0; i < atoi(argv[1]); ++i)/*memory allocation for pipes*/
    {
   		fd[i] = (int*)malloc(2*sizeof(int));
   		pipe(fd[i]);/*create pipes*/
    }
    int **matrix = (int**)malloc(size*sizeof(int*));
    for ( i = 0; i < size; ++i)/*memory allocation for matrix*/
    {
        matrix[i] = (int*)malloc(size*sizeof(int));
    }
    create_matrix(matrix,size);/*fill the matrix*/
    for(i = 0; i < atoi(argv[1]); i++)/*for loop for n child processes for matrix multiplication*/ 
    {
        f = fork();/*create child process*/
        if(f == 0)
        {
        	/*child process*/	
            read(fd[i][0], matrix, size*size*sizeof(int));/*read matrix from the parent process*/
            matrix_multiplication(matrix, size);/*multiply matrix with itself*/
            print_matrix(matrix, size, i+1, getpid());/*print screen and the file matrix*/
            write(fd[i+1][1], matrix, size*size*sizeof(int));/*write new matrix in the next pipe for next child process*/                      
            close(fd[i][0]);/*close read*/
            close(fd[i+1][1]);/*close write*/

        }
        else if(f > 0)
        {
        	/*parent process*/
        	if(i == 0)
        	{
            	close(fd[0][0]);/*close read because we do not use read operation*/
            	write(fd[0][1], matrix, size*size*sizeof(int));  /*write the matrix in pipe for child processes*/
            }
            else
            {
            	close(fd[0][0]);
            }
            close(fd[0][1]);
            waitpid(-1, NULL, 0); /*wait for child processes*/
            exit(0);
        }
        else 
        {
            exit(0);
 	    }
    }
	free(fd);
	free(matrix);
	return 0;
}
/*print_matrix will prints process's informations and matrix on screen and the file*/
void print_matrix(int **matrix, int size, int fileNumber, int pid){
	FILE *mFile;
	char fileName[6];
	sprintf(fileName, "%d.txt", fileNumber);
	mFile = fopen(fileName, "w+");
	int i,j;
	printf("Process-%d %d\n", fileNumber, pid );
	fprintf(mFile,"Process-%d %d\n", fileNumber, pid );

	for (i = 0; i < size; ++i)
    {
        for (j = 0; j < size; ++j)
        {
           fprintf(mFile, "%d ", matrix[i][j]);
           printf("%d ", matrix[i][j]);
           
        }
        fprintf(mFile, "\n");
        printf("\n");
    }
    fprintf(mFile, "\n");
	printf("\n");
	fclose(mFile);
}
/*matrix_multiplication function takes a matrix and multiply with itself*/
void matrix_multiplication(int **a, int size){
    int i, j , k ,sum;
    int **c=(int**)malloc(size*sizeof(int*));
    for ( i = 0; i < size; ++i)
    {
        c[i] = (int*)malloc(size*sizeof(int));
    }
    for(i = 0; i < size; i++)
    { 
      for(j = 0; j < size; j++)
      {
           sum=0;
           for(k = 0; k < size; k++)
               sum = sum + a[i][k] * a[k][j];
           c[i][j] = sum;
      }
    }
      for(i = 0; i < size; i++)
      { 
        for(j = 0; j < size; j++)
        {
            a[i][j] = c[i][j];/*first matrix show us to multiplication's result*/
        }
      }
     free(c);
}
/*size_of_matrix reads the first line of matrix.txt and determines the size of matrix and return it */
int size_of_matrix(){
    char const* const fileName = "matrix.txt"; 
    FILE* file = fopen(fileName, "r+"); 
    char line[256];
    int size;
    while (fgets(line, sizeof(line), file)) 
    {
        size=atoi(line);
        break;
    }
    fclose(file);
    return size;
    


}
/*create_matrix function reads the matrix.txt and fill the matrix with integers*/
void create_matrix(int **matrix, int size){
 	char const* const fileName = "matrix.txt"; 
    FILE* file = fopen(fileName, "r+"); 
    char line[256];
    int i = 0;
    int j = 0;
    char *pch;
    int control = 0;
    while (fgets(line, sizeof(line), file)) 
    {
        if(control > 0)
        {
        	pch = strtok (line," \t\n\r,");
        	j=0;
        	while (pch != NULL)
        	{
            	matrix[i][j] = atoi(pch);
            	j++;
            	pch = strtok (NULL, " \t\n\r,");
        	}
        	i++;
       }
       control++; 
    }
	fclose(file);
}