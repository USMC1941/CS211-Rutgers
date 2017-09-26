#include <stdio.h>
#include <stdlib.h>


#define FALSE 0
#define TRUE 1


typedef struct Node {
	int data;
	struct Node* next;
} Node;

typedef Node* LinkedList;


int insertNodeIntoLinkedList(LinkedList* pInputLinkedList, Node* nodeToInsert);
Node* deleteNodeFromLinkedList(LinkedList* pInputLinkedList, int inputData);
void printLinkedList(LinkedList inputLinkedList);
void freeLinkedList(LinkedList inputLinkedList);


int main (int argc, char** argv) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("error\n");
		return 0;
	}	

	LinkedList theList = NULL;
	
	//File
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL)  {
        printf ("error\n");
		return 0;
    }
	else {
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
					int isInserted = insertNodeIntoLinkedList(&theList, pNodeToInsert);
					//
					if (isInserted == FALSE) {
						free(pNodeToInsert);
					}					
				}
				else if (c == 'd') {
					Node* pNodeDeleted = deleteNodeFromLinkedList(&theList, i);
					//
					if (pNodeDeleted) {
						free(pNodeDeleted);
					}
				}
				else {
					printf ("error\n");
					freeLinkedList(theList);
					return 0;
				}
			}
			else {
				printf ("error\n");
				freeLinkedList(theList);
				return 0;
			}
			//
            r = fscanf(fp, "%c %d\n", &c, &i);
		}
		//
		fclose(fp);
    }
	
	// Print list
	printLinkedList(theList);
	
	// Free list
	freeLinkedList(theList);
	return 0;
}






// Boolean inserted or not
int insertNodeIntoLinkedList(LinkedList* pInputLinkedList, Node* nodeToInsert) {
	// Find the place to insert
	Node* previous = NULL;
	Node* after = NULL;
	int isDuplicate = FALSE;
	//
	if (pInputLinkedList == NULL) {											//No list (null pointer)
	}
	else if (*pInputLinkedList == NULL) {									//List is empty
	}
	else {																					// List not empty
		Node* previousNodeInLoop = NULL;
		Node* oneNodeInList = *pInputLinkedList;						//the head
		//
		while (oneNodeInList) {
			if (oneNodeInList->data == nodeToInsert->data) {
				isDuplicate = TRUE;
				//duplicate
				break;
			}
			else {
				if (previousNodeInLoop) {											// Current node is not the head of the list
					if (previousNodeInLoop->data < nodeToInsert->data && oneNodeInList->data > nodeToInsert->data) {
						previous = previousNodeInLoop;
						after = oneNodeInList;
						break;
					}
					else {
					}
				}
				else {																	// previousNodeInLoop is NULL; Current node is the head of the list
					if (oneNodeInList->data > nodeToInsert->data) {
						previous = NULL;
						after = oneNodeInList;
						break;
					}
					else {
					}
				}
			}
			//
			previousNodeInLoop = oneNodeInList;
			oneNodeInList = oneNodeInList->next;			
		}
	}
	
	// Insert node
	if (isDuplicate) {
	}
	else {
		if (after) {
			if (previous) {															// Node to insert before is not head
				nodeToInsert->next = after;
				previous->next = nodeToInsert;
			}
			else {																		//insert before head
				nodeToInsert->next = (*pInputLinkedList);
				(*pInputLinkedList) = nodeToInsert;
			}
		}
		else {
			if (*pInputLinkedList == NULL) {								// List is empty
				*pInputLinkedList = nodeToInsert;
			}
			else {																		// Insert into tail
				Node* pOneInList =  *pInputLinkedList;
				while (pOneInList->next != NULL) {
					pOneInList = pOneInList->next;
				}
				pOneInList->next = nodeToInsert;				
			}
		}
	}
	
	return isDuplicate == FALSE;											//is inserted
}



Node* deleteNodeFromLinkedList(LinkedList* pInputLinkedList, int inputData) {
	// Find the Node and its previous
	Node* previous = NULL;
	Node* nodeFound = NULL;
	//
	if (pInputLinkedList == NULL) {														//No list (null pointer)
	}
	else if (*pInputLinkedList == NULL) {												//List is empty
	}
	else {																								// List not empty
		Node* previousNodeInLoop = NULL;
		Node* oneNodeInList = *pInputLinkedList;									//the head
		//
		while (oneNodeInList) {
			if (oneNodeInList->data == inputData) {
				previous = previousNodeInLoop;
				nodeFound = oneNodeInList;
				break;
			}
			//
			previousNodeInLoop = oneNodeInList;
			oneNodeInList = oneNodeInList->next;			
		}
	}
	
	// Delete nodeFound
	if (nodeFound) {														// If found
		if (previous) {														// If nodeFound is not first
			previous->next = nodeFound->next;
			nodeFound->next = NULL;
		}
		else {																// If nodeFound is first
			*pInputLinkedList = nodeFound->next;
			nodeFound->next = NULL;
		}
	}
	else {																	// If not found
		// Do nothing
	}
	
	return nodeFound;	
}



void printLinkedList(LinkedList inputLinkedList) {
	if (inputLinkedList) {
		Node* pOneToPrint =  inputLinkedList;
		while (pOneToPrint) {
			printf("%d",  pOneToPrint->data);
			pOneToPrint = pOneToPrint->next;
			//
			if (pOneToPrint) {
				printf("\t");
			}
		}
		printf("\n");
	}
	else {
		printf("\n");
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
