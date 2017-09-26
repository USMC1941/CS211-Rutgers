#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1


void addMatrix (int* a, int* b, int* c, int max_row, int max_column);
int getColumn (int index, int max_row, int max_column);
int getRow (int index, int max_row, int max_column);
int getIndex (int row, int column, int max_row, int max_column);
void printMatrix (int* a, int max_row, int max_column);
void freeMatrices (int* a, int* b, int* c) ;


int main (int argc, char** argv) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("error\n");
		return 0;
	};

	int *a = NULL;
	int *b = NULL;
	int *c = NULL;
	
	//File
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)  {
        printf ("error\n");
		return 0;
    }
	else {
		int max_row;
		int max_column;
		//
		int r = fscanf(fp, "%d %d\n", &max_row, &max_column);

		if (r == 2) {
			a = (int*) malloc((max_row * max_column)*sizeof(int));
			b = (int*) malloc((max_row * max_column)*sizeof(int));
			c = (int*) malloc((max_row * max_column)*sizeof(int));
		}
		else {
			printf ("error\n");
			return 0;
		}
		
		/*First Matrix*/
		int i;
		for (i=0; i < (max_row * max_column ); i++) {
			int column = getColumn(i, max_row, max_column);
			//
			int r;
			int iTemp;
			if (column + 1 == max_column) {				// Last one in the row
				r = fscanf(fp, "%d\n", &iTemp);
			}
			else {
				r = fscanf(fp, "%d", &iTemp);
			}
			//
			if (r==1) {
				*(a+i) = iTemp;
			}
			else {
				printf ("error\n");
				freeMatrices (a, b, c) ;
				return 0;
			}
		}
		
		/*Second Matrix*/
		for (i=0; i < (max_row * max_column ); i++) {
			int column = getColumn(i, max_row, max_column);
			//
			int r;
			int iTemp;
			if (i == 0) {
				if (max_column == 1) {									//first of the matrix
					r = fscanf(fp, "\n%d\n", &iTemp);				
				}
				else {
					r = fscanf(fp, "\n%d", &iTemp);				
				}
			}
			else if (column + 1 == max_column) {				// Last one in the row
				r = fscanf(fp, "%d\n", &iTemp);				
			}
			else {
				r = fscanf(fp, "%d", &iTemp);
			}
			//
			if (r == 1) {
				*(b+i) = iTemp;
			}
			else {
				printf ("error\n");
				freeMatrices (a, b, c) ;
				return 0;
			}
		}

		fclose(fp);

		addMatrix (a, b, c, max_row, max_column);
		
		printMatrix (c, max_row, max_column);
		
		freeMatrices (a, b, c) ;
    }
	
	return 0;
}


//index <-> col, row
int getIndex (int row, int column, int max_row, int max_column) {
	return column + row * max_column;
}

int getRow (int index, int max_row, int max_column) {
	return index / max_column;
}

int getColumn (int index, int max_row, int max_column) {
	return index % max_column;
}


void addMatrix (int* a, int* b, int* c, int max_row, int max_column) {
	int i;
	for (i=0; i < max_row * max_column; i++) {
		*(c + i) = *(a + i) + *(b+i);
	}
}


void printMatrix (int* a, int max_row, int max_column) {
	int i;
	int j;

	for (i=0; i < max_row; i++) {
		for (j=0; j < max_column; j++) {
			int iOne = *(a + getIndex(i, j, max_row, max_column));
			printf("%d", iOne);
			//
			if (j != max_column - 1) {					//not last column in a row
				printf("\t");
			}
			else {
				printf("\n");
			}
		}
	}
}


void freeMatrices (int* a, int* b, int* c) {
	if (a) {free(a);}
	if (b) {free(b);}
	if (c) {free(c);}
}
