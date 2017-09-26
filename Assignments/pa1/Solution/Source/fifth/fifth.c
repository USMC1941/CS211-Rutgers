#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1


void multiplyMatrix (int* a, int* b, int* c, int max_row_a, int max_column_a, int max_row_b, int max_column_b);
int getColumn (int index, int max_row, int max_column);
int getRow (int index, int max_row, int max_column);
int getIndex (int row, int column, int max_row, int max_column);
void printMatrix (int* a, int max_row, int max_column);
void freeMatrixs (int* a, int* b, int* c) ;

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
		int max_row1;
		int max_column1;
		//
		{
			int r = fscanf(fp, "%d %d\n", &max_row1, &max_column1);

			if (r == 2) {
				a = (int*) malloc((max_row1 * max_column1)*sizeof(int));
			}
			else {
				printf ("error\n");
				return 0;
			}
		}
		
		/*First Matrix*/
		int i;
		for (i=0; i < (max_row1 * max_column1 ); i++) {
			int column = getColumn(i, max_row1, max_column1);
			//
			int r;
			int iTemp;
			if (column + 1 == max_column1) {				// Last one in the row
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
				freeMatrixs (a, b, c);
				return 0;
			}
		}
		
		int max_row2;
		int max_column2;
		{
			int r = fscanf(fp, "\n\n%d %d\n", &max_row2, &max_column2);
			//
			if (r == 2) {
				if (max_column1 != max_row2) {
					printf ("error\n");
					freeMatrixs (a, b, c);
					return 0;
				}
				else {
					b = (int*) malloc((max_row2 * max_column2)*sizeof(int));
				}
			}
			else {
				printf ("error\n");
				freeMatrixs (a, b, c);
				return 0;
			}
		}
		
		/*Second Matrix*/
		for (i=0; i < (max_row2 * max_column2 ); i++) {
			int column = getColumn(i, max_row2, max_column2);
			//
			int r;
			int iTemp;
			if (column + 1 == max_column2) {				// Last one in the row
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
				freeMatrixs (a, b, c);
				return 0;
			}
		}

		fclose(fp);
		
		c = (int*) malloc((max_row1 * max_column2)*sizeof(int));

		multiplyMatrix (a, b, c, max_row1, max_column1, max_row2, max_column2);
		
		printMatrix (c, max_row1, max_column2);
		
		freeMatrixs (a, b, c);
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


void multiplyMatrix (int* a, int* b, int* c, int max_row_a, int max_column_a, int max_row_b, int max_column_b) {
	int i;
	int j;
	int k;
	for (i = 0; i < max_row_a; i++) {
		for (j = 0; j < max_column_b; j++) {
			int sum = 0;
			for (k = 0; k < max_column_a; k++) {											// max_column_a = int max_row_b
				int index_a = getIndex(i, k, max_row_a, max_column_a);
				int index_b = getIndex(k, j, max_row_b, max_column_b);
				sum += (*(a + index_a)) * (*(b + index_b));
			}
			int index_c = getIndex(i, j, max_row_a, max_column_b);	
			*(c + index_c) = sum;
		}
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
			if (j != max_column - 1) {
				printf("\t");
			}
			else {
				printf("\n");
			}
		}
	}
}


void freeMatrixs (int* a, int* b, int* c) {
	if (a) {free(a);}
	if (b) {free(b);}
	if (c) {free(c);}
}
