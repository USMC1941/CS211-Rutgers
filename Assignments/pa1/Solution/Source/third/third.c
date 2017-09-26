#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

#define BUCKETSIZE 10000

typedef struct Node {
	int data;
	struct Node* next;
} Node;

typedef Node* LinkedList;

typedef Node* HashTable;


int hash (int key);
int insertToHashTable (HashTable* pHashTable, Node* pNodeToInsert);
int searchFromHashTable (HashTable* pHashTable, int inputData);
void freeHashTable (HashTable*  hashTable);
void freeLinkedList(LinkedList inputLinkedList);


int main (int argc, char** argv) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("error\n");
		return 0;
	};

	Node** theHashTable = (Node**) malloc(BUCKETSIZE * sizeof(Node*));
	
	int i;
	for (i=0; i < BUCKETSIZE; i++) {
		*(theHashTable + i) = NULL;
	}
	
	//File
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)  {
        printf ("error\n");
		return 0;
    }
	else {
		int IS_EMPTY_FILE = TRUE;
		//
		char c;
		int i;
		//
		int r = fscanf(fp, "%c %d\n", &c, &i);
        while (r != EOF) {
            if (r == 2) {
				if (c == 'i') {
					Node* pNodeToInsert = (Node*) malloc(sizeof(Node));
					pNodeToInsert->data = i;
					pNodeToInsert->next = NULL;
					//
					int isInserted = insertToHashTable(theHashTable, pNodeToInsert);
					//
					if (isInserted == TRUE) {
						printf("inserted\n");
						IS_EMPTY_FILE = FALSE;
					}
					else { 
						free(pNodeToInsert);
						printf("duplicate\n");
						IS_EMPTY_FILE = FALSE;
					}
				}
				else if (c == 's') {
					int isFound = searchFromHashTable(theHashTable, i);
					//
					if (isFound) {
						printf("present\n");
						IS_EMPTY_FILE = FALSE;
					}
					else {
						printf("absent\n");
						IS_EMPTY_FILE = FALSE;
					}
				}
				else {
					printf("error\n");
						IS_EMPTY_FILE = FALSE;
				}
			}
			else {
				printf("error\n");
				IS_EMPTY_FILE = FALSE;
			}
			//
            r = fscanf(fp, "%c %d\n", &c, &i);
		}
		//
		fclose(fp);
		//
		if ( IS_EMPTY_FILE == TRUE) {
				printf("\n");
		}
    }
	
	// Free HashTable
	freeHashTable (theHashTable);
	
	return 0;
}



int hash (int key) {
   int ret = key % BUCKETSIZE;

   if (ret < 0) {
	   ret += BUCKETSIZE;
   }
	
   return ret;	
}



int insertToHashTable (HashTable* pHashTable, Node* pNodeToInsert) {
	int isInserted = FALSE;
	//
	int hashIndex = hash(pNodeToInsert->data);
	Node* pHeadOfLinkedList = (*(pHashTable+hashIndex));
	//
	if (pHeadOfLinkedList == NULL) {
		(*(pHashTable+hashIndex)) = pNodeToInsert;
		isInserted = TRUE;
	}
	else {
		int isDuplicate = FALSE;
		//
		Node* pOneNodeInList = pHeadOfLinkedList;
		while (pOneNodeInList) {
			if (pOneNodeInList->data == pNodeToInsert->data) {
				isDuplicate = TRUE;
				break;
			}
			//
			pOneNodeInList = pOneNodeInList->next;
		}
		//
		if (isDuplicate) {
			//do not insert
		}
		else {																						// Insert before the head
			pNodeToInsert->next = pHeadOfLinkedList;
			(*(pHashTable+hashIndex)) = pNodeToInsert;
			isInserted = TRUE;			
		}
	}
	//
	return isInserted;
}


int searchFromHashTable (HashTable* pHashTable, int inputData) {
	Node* nodeFound = NULL;

	int hashIndex = hash(inputData);
	Node* pHeadOfLinkedList = (*(pHashTable+hashIndex));
	
	if (pHeadOfLinkedList != NULL) {										// List not empty
		Node* oneNodeInList = pHeadOfLinkedList;						//the head
		//
		while (oneNodeInList) {
			if (oneNodeInList->data == inputData) {
				nodeFound = oneNodeInList;
				break;
			}
			//
			oneNodeInList = oneNodeInList->next;			
		}
	}
	else {																		//List is empty
	
	}
	//
	return nodeFound != NULL;
}



void freeHashTable (HashTable*  hashTable) {
	if (*hashTable) {
		int i;
		for (i=0; i < BUCKETSIZE; i++) {
			LinkedList one = *(hashTable+i);
			freeLinkedList(one);
		}
		//
		free(hashTable);
	}
}


void freeLinkedList(LinkedList inputLinkedList) {
	if (inputLinkedList) {
		Node* pOneNodeToFree =  inputLinkedList;
		while (pOneNodeToFree) {
			Node* temp = pOneNodeToFree;
			//
			pOneNodeToFree = pOneNodeToFree->next;
			//
			free(temp);
		}
	}
	else {
	}
}
