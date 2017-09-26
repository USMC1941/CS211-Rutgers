#include <stdio.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1


typedef struct Node {
	int data;
	struct Node* left;
	struct Node* right;
} Node;

typedef Node* BST;


int insertNodeIntoBST(BST* pRoot, Node* nodeToInsert);
Node* deleteNodeFromBST(BST* pRoot, int data, Node** ppDeleted);
int searchInBST(BST* pRoot, int data);
Node* findMinFromRight(Node* pRoot);
void freeBST(BST* pRoot);


int main (int argc, char** argv) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("error\n");
		return 0;
	}

	BST theBST = NULL;
	
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
					pNodeToInsert->left = NULL;
					pNodeToInsert->right = NULL;
					//
					int isInserted = insertNodeIntoBST(&theBST, pNodeToInsert);
					//
					if (isInserted < 0) {
						free(pNodeToInsert);
						printf("duplicate\n");
					}
					else {
						printf("inserted %d\n", isInserted);
					}
				}
				else if (c == 's') {
					int found = searchInBST(&theBST, i);
					//
					if (found < 0) {
						printf("absent\n");
					}
					else {
						printf("present %d\n", found);
					}
				}
				else if (c == 'd') {
					Node* pDeleted = NULL;
					theBST = deleteNodeFromBST(&theBST, i, &pDeleted);
					//
					if (pDeleted == NULL) {
						printf("fail\n");
					}
					else {
						printf("success\n");
						free(pDeleted);
					}
				}
				else {
					printf ("error\n");
				}
			}
			else {
				printf ("error\n");
			}
			//
            r = fscanf(fp, "%c %d\n", &c, &i);
		}
		//
		fclose(fp);
    }
	
	freeBST(&theBST);
	
	return 0;
}





int insertNodeIntoBST(BST* pRoot, Node* nodeToInsert) {
	int depth = 1;
	
	int data = nodeToInsert->data;
	//
	if (*pRoot == NULL) {
		*pRoot = nodeToInsert;
		return depth;
	}
	else {
		Node* current = (*pRoot);
		Node* parent  = NULL;
		while (TRUE) {
			depth++;
			parent = current;
			if (data < current->data) {								//< go to left
				current = current->left;
				if (current == NULL) {
					parent->left = nodeToInsert;
					return depth;
				}
			}
			else if (data > current->data) {						//> go to right
				current = current->right;
				if (current == NULL) {
					parent->right = nodeToInsert;
					return depth;
				}
			}
			else {
				return -1;													//= do not insert
			}
		}
	}
	//
	return -1;																//never here
}


int searchInBST(BST* pRoot, int data) {
	int depth = 1;
	
	Node* current = *pRoot;
	
	while (current!=NULL) {
		if (current->data == data) {									//=
			return depth;
		}
		else if (current->data > data) {							//< go to left
			depth++;
			current = current->left;
		}
		else {																	//> right
			depth++;
			current = current->right;
		}
	}
	
	return -1;																//not found
}


Node* deleteNodeFromBST(BST* pRoot, int data, Node** ppDeleted) {
    if ((*pRoot) == NULL) {
        return (*pRoot);
    }
	//
    if ((*pRoot)->data > data) {																					//< go to left
        (*pRoot)->left = deleteNodeFromBST(&((*pRoot)->left), data, ppDeleted);
    }
	else if ((*pRoot)->data < data) {																			//> go to right
        (*pRoot)->right = deleteNodeFromBST(&((*pRoot)->right), data, ppDeleted);
    }
	else {																														//= delete it
        if ((*pRoot)->left == NULL && (*pRoot)->right == NULL) {								//no child
			*ppDeleted = (*pRoot);
            *pRoot = NULL;
        }
		else if ((*pRoot)->right == NULL) {																	//only left child
			*ppDeleted = (*pRoot);
            *pRoot = (*pRoot)->left;
        }
		else if ((*pRoot)->left == NULL) {																		//only right child
			*ppDeleted = (*pRoot);
            *pRoot = (*pRoot)->right;
        }
		else {																													//two children
            Node* temp  = findMinFromRight((*pRoot)->right);
            (*pRoot)->data = temp->data;
            (*pRoot)->right = deleteNodeFromBST(&((*pRoot)->right), temp->data, ppDeleted);
        }
    }
    return (*pRoot);
}
Node* findMinFromRight(Node* pRoot) {
	Node* pNode = pRoot;
    while (pNode->left != NULL) {
        pNode = pNode->left;
    }
    return pNode;
}



void freeBST(BST* pRoot) {
    if ((*pRoot) == NULL) {
    }
	else {
		freeBST(&((*pRoot)->left));
	 
		freeBST(&((*pRoot)->right));

		free(*pRoot);
	}
}
