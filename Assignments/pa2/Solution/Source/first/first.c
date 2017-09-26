#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "trie.h"
#include "first.h"


char oneString[1024];

		
LetterNodes** ppNodes 	= NULL;													// Current letter
LetterNodes** ppRoot 	= NULL;													// Root of trie. 
int maxTrieDepth = 0;
char* pWord = NULL;
FILE** pFp_Out = NULL;




/* Parameters: Mapping file name */
int main (int argc, char** argv) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("invalid input\n");
		return 0;
	};
	
	//File
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)  {
        printf ("invalid input\n");
		return 0;
    }
	else {
		/* Filenames for dict_file and data_file */
		char str1[256];
		char str2[256];
		//
		/* Sequence of file -> needed in creating output file name */
		int i = 0;
		//
		int r = fscanf(fp, "%s %s", str1, str2);
		while (r != EOF) {
			if (r == 2) {
				FILE *fp_dict   = fopen(str1, "r");
				FILE *fp_data  = fopen(str2, "r");
				//
				if (fp_dict == NULL || fp_data == NULL) {
					//do nothing
				}
				else {
					ppNodes = NULL;
					ppRoot = NULL;
					maxTrieDepth = 0;
					pWord = NULL;
					pFp_Out = NULL;
					//
					LetterNodes* root = NULL;
					ppRoot = &root;
					//
					readDict(fp_dict);
					//
					while (fscanf(fp_data, "%1023s", oneString) == 1) {
						matchStr(oneString);
					}
					//
					i++;
					//
					char str[64];
					char buf[8];
					strcpy(str, "out");					
					sprintf(buf, "%d", i);
					strcat(str, buf);
					strcat(str, ".txt");
					//
					FILE *fp_out = fopen(str, "w");
					if (fp_out==NULL) {
						printf ("invalid input\n");
					}
					//
					pFp_Out = &fp_out;
					//
					pWord = (char*)calloc(1, (maxTrieDepth + 1) * sizeof(char));
					printResult();
					//
					if (fp_out != NULL) {
						fclose(fp_out);
					}
					//
					free(pWord);
					freeMemory(ppRoot);
				}
				//
				if (fp_dict != NULL) {
					fclose(fp_dict);
				}
				//
				if (fp_data != NULL) {
					fclose(fp_data);
				}
			}
			//
			/* Loop through map file */
			r = fscanf(fp, "%s %s", str1, str2);
		}
		//
		fclose(fp);
	}
	//
	return 0;
}

/*
The function takes a file pointer to the dictionary file and
reads the unique words from the dictionary file and stores them in an appropriate data structure.
*/
void readDict(FILE *dict_file) {
    if (dict_file == NULL)  {
    }
	else {
		int currentDepth = 0;
		int oneChar = fgetc(dict_file);
		int nextChar;
		if (oneChar != EOF) {
			oneChar = tolower(oneChar);
			nextChar = fgetc(dict_file);
			if (nextChar != EOF) {
				ppNodes = ppRoot;
				while (nextChar != EOF) {
					nextChar = tolower(nextChar);
					//
					if (isalpha(oneChar)) {
						int hasMore = FALSE;
						if (isalpha(nextChar)) {
							hasMore = TRUE;
						}
						//
						ppNodes = insertLetter(ppNodes, &maxTrieDepth, &currentDepth, oneChar, hasMore);
						//
						if (ppNodes == NULL) {
							ppNodes = ppRoot;
							currentDepth = 0;
						}
					}
					else {
						ppNodes = ppRoot;
						currentDepth = 0;
					}
					//
					oneChar = nextChar;
					nextChar = fgetc(dict_file);
				}
				//
				if (isalpha(oneChar)) {
					ppNodes = insertLetter(ppNodes, &maxTrieDepth, &currentDepth, oneChar, FALSE);
				}
				//doing
			}
			else {
				/* Only 1 char in the dict_file */
				ppNodes = ppRoot;
				//
				if (isalpha(oneChar)) {
					ppNodes = insertLetter(ppNodes, &maxTrieDepth, &currentDepth, oneChar, FALSE);
					//
					if (ppNodes == NULL) {
						ppNodes = ppRoot;
						currentDepth = 0;
					}
				}
				else {
					ppNodes = ppRoot;
					currentDepth = 0;
				}
			}
		}
		else {
			/* Nothing in dict_file */
		}
    }
}


/*
This function will produce the output of the program
*/
void printResult() {
	printOutput(pFp_Out, ppRoot, 0, pWord) ;
}

/*
This function will take a word and search the data structure that
holds the unique dictionary words in order to find matches and to update the various counts. This
function should be used while scanning the data file for occurrences of dictionary words and their
prefixes and superwords
*/
void matchStr(char* str) {
	int len = strlen(str);
	int oneChar;
	int nextChar;
	//
	ppNodes = ppRoot;
	//
	int i;
	for (i=0; i < (len - 1); i++) {
		oneChar = *(str + i);
		nextChar = *(str + i + 1);
		//
		oneChar = tolower(oneChar);
		nextChar = tolower(nextChar);
		//
		if (isalpha(oneChar)) {
			int hasMore = FALSE;
			if (isalpha(nextChar)) {
				hasMore = TRUE;
			}
			//
			if (ppNodes) {
				ppNodes = checkLetterForWordAndPrefix(ppNodes, oneChar, hasMore);
			}
		}
		else {
			ppNodes = ppRoot;
		}
	}
	//
	if (ppNodes) {
		if (isalpha(tolower(*(str + strlen(str) - 1)))) {
			ppNodes = checkLetterForWordAndPrefix(ppNodes, tolower(*(str + strlen(str) - 1)), FALSE);
		}
	}
}