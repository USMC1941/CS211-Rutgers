#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"



/*
This function processes 1 letter from the dictionary file.
Input:
	ppNodes: The nodes structure (for 26 letters) the current letter would be inserted into
	pMaxDepth: Pointer to the max depth of the trie (after reading the dict_file, it would be the max word length)
	pCurrentDepth: Current depth of the trie
	input: Letter currently being processed.
	hasMore: Indicate whether there are more letters for the current word.
Output:
	The node structure for the next letter (if any) of the current word. Will return NULL if input is the last letter of the current word
Note:
	Dictionary File: Input is lowercase
*/
LetterNodes** insertLetter(LetterNodes** ppNodes, int* pMaxDepth, int *pCurrentDepth, int input, int hasMore) {
	/* Find the maximum length of words */
	(*pCurrentDepth)++;
	if (*pCurrentDepth > *pMaxDepth) {
		*pMaxDepth = *pCurrentDepth;
	}
	//
	LetterNodes** ppNext = NULL;														// The nodes for the next letter in the word
	//
	int isNewLetterNodes = 0;																// boolean 
	//
	if  ((*ppNodes) == NULL) {
		/* If there is no trie nodes, create one. If there is a child for this letter, allocate memory to hold it too*/
		(*ppNodes) = (LetterNodes*)calloc(1, sizeof(LetterNodes) + hasMore * sizeof(LetterNodes*));
		isNewLetterNodes = 1;
		//
		/* Initialize letterLeafIndex */
		memset((*ppNodes)->letterLeafIndex, LEAF_PROPERTY_NOT_PRESENT, ALPHABET_COUNT);
	}
	//
	int index = input - 'a';
	//
	/* Letter never present in trie branch before */
	if ((*ppNodes)->letterChildIndex[index] == CHILD_PROPERTY_NOT_PRESENT) {
		if (hasMore) {																																					// More letters for this word.
			(*ppNodes)->childCount++;
			(*ppNodes)->letterChildIndex[index] = (*ppNodes)->childCount;																	// Index from 1-26
			//
			(*ppNodes)->letterLeafIndex[index] = LEAF_PROPERTY_PRESENT_NOT_LEAF;													// Since there are more letters in this word, it is not a leaf
			//
			if (isNewLetterNodes) {
				// No need to realloc as the calloc already account for the space for the new pointer for the child
				ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) + (*ppNodes)->letterChildIndex[index];
			}
			else {
				/* Need to add space for the pointer to the children. Size of LetterNodes and size of all pointers to their children */
				(*ppNodes) = (LetterNodes*)realloc((*ppNodes), sizeof(LetterNodes) + (*ppNodes)->childCount * sizeof(LetterNodes*));
				//
				ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) + (*ppNodes)->letterChildIndex[index];
				*ppNext = NULL;
			}
		}
		else {
			//(*ppNodes)->childCount;																														// Child count does not change
			(*ppNodes)->letterChildIndex[index] = CHILD_PROPERTY_PRESENT_WITHOUT_CHILD;									// For that letter, it is present but no child
			//
			/* If no child, it will be a leaf */
			(*ppNodes)->letterLeafIndex[index] = (*ppNodes)->leafCount;
			(*ppNodes)->leafCount++;
		}
	}
	/* Letter present in trie branch but has no child */
	else if ((*ppNodes)->letterChildIndex[index] == CHILD_PROPERTY_PRESENT_WITHOUT_CHILD) {
		if (hasMore) {
			(*ppNodes)->childCount++;
			(*ppNodes)->letterChildIndex[index] = (*ppNodes)->childCount;
			//
			//(*ppNodes)->letterLeafIndex[index];
			//(*ppNodes)->leafCount;
			//
			/* Allocate memory for the additional child */
			(*ppNodes) = (LetterNodes*)realloc((*ppNodes), sizeof(LetterNodes) + (*ppNodes)->childCount * sizeof(LetterNodes*));
			//
			ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) + (*ppNodes)->letterChildIndex[index];
			*ppNext = NULL;
		}
		else {
			//(*ppNodes)->childCount;
			//(*ppNodes)->letterChildIndex[index];
			//
			if ((*ppNodes)->letterLeafIndex[index] >= ALPHABET_COUNT) {																// Not a leaf yet
				(*ppNodes)->letterLeafIndex[index] = (*ppNodes)->leafCount;															// Mark it as a leaf
				(*ppNodes)->leafCount++;
			}
		}
	}
	/* Letter is in trie branch and has children (1 or more) */
	else 	{																																					// CHILD_PROPERTY_PRESENT_WITH_CHILD
		/* Child is already there */
		if (hasMore) {																																	// Both existing and input have child
			//(*ppNodes)->childCount;																											
			//(*ppNodes)->letterChildIndex[index];
			//
			//(*ppNodes)->letterLeafIndex[index] ;
			//(*ppNodes)->leafCount;
			//
			ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) + (*ppNodes)->letterChildIndex[index];
		}
		else {
			//(*ppNodes)->childCount;
			//(*ppNodes)->letterChildIndex[index];
			//
			if ((*ppNodes)->letterLeafIndex[index] >= ALPHABET_COUNT) {																// Not a leaf yet
				(*ppNodes)->letterLeafIndex[index] = (*ppNodes)->leafCount;															// Mark it as a leaf
				(*ppNodes)->leafCount++;
			}
		}
	}
	//
	return ppNext;
}




/*
This function search 1 letter from the data file
Input:
	ppNodes: The nodes structure (for 26 letters) the current letter would be compared into
	input: letter from the data file currently being searched
	hasMore: Indicate whether there are more letters for the current word.
Output:
	The node structure for the next letter (if any) of the current word. Will return NULL if input is the last letter of the current word
Note:
	Data File: Input is lowercase
*/
LetterNodes** checkLetterForWordAndReversePrefix(LetterNodes** ppNodes, int input, int hasMore) {
	LetterNodes** ppNext = NULL;
	//
	int index = input - 'a';
	//
	if ((*ppNodes)->letterChildIndex[index] == CHILD_PROPERTY_NOT_PRESENT) {											// Not present
		// do nothing
	}
	else if ((*ppNodes)->letterChildIndex[index] == CHILD_PROPERTY_PRESENT_WITHOUT_CHILD) {				// No child
		if (hasMore) {																																		// Input has child
		}
		else {																																					// Input has no child
			if ((*ppNodes)->letterLeafIndex[index] < ALPHABET_COUNT) {															// This is a leaf. No more letters for that word
				if ((*ppNodes)->pCounters == NULL ) {
					/* No memory for the counters yet. Need to allocate memory. */
					(*ppNodes)->pCounters = (int*)calloc(1, (*ppNodes)->leafCount * 2 * sizeof(int));
				}
				//
				(*((*ppNodes)->pCounters + 2 * (*ppNodes)->letterLeafIndex[index] ))++;									// Word count + 1
			}
			else {
				// This is not a leaf
			}
		}
	}
	else 	{																																						// CHILD_PROPERTY_PRESENT_WITH_CHILD
		if (hasMore) {
			ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) +(*ppNodes)->letterChildIndex[index];				// For next letter
		}
		else {
			if ((*ppNodes)->letterLeafIndex[index] < ALPHABET_COUNT) {															// This is a leaf
				if ((*ppNodes)->pCounters == NULL ) {
					/* No memory for the counters yet. Need to allocate memory. */
					(*ppNodes)->pCounters = (int*)calloc(1, (*ppNodes)->leafCount * 2 * sizeof(int));
				}
				//
				(*((*ppNodes)->pCounters + 2 * (*ppNodes)->letterLeafIndex[index] ))++;									// Word count + 1
			}
			else {
				// This is not a leaf
			}
			//
			/* This is a word from the data file. Count it and propogate downstream as prefix for other dictionary words */
			(*ppNodes)->dataWordCount[index]++;	
		}
	}
	//
	return ppNext;
}


/*
This function prints the output by going through the trie recursively.
Input:
	pFileOut: Pointer to the output file.
	ppNodes: The nodes structure (for 26 letters)
	pCurrentDepth: Current depth of the trie
	pStr: Hold the words by combining all the letters from different levels of the trie
Output:
	void
*/
void printOutput(FILE** pFileOut, LetterNodes** ppNodes, int currentDepth, char* pStr) {
	if ((*ppNodes) != NULL) {
		//
		int index;
		for (index=0; index < ALPHABET_COUNT; index++) {
			*(pStr + currentDepth) = ((char)index) + 'a';
			//
			if ((*ppNodes)->letterLeafIndex[index] < ALPHABET_COUNT) {												// This is a leaf. Need to print.
				/* No counter for all 26 letters */
				if ((*ppNodes)->pCounters==NULL) {
					fprintf(*pFileOut, "%s %d %d\n", pStr, 0, 0);
				}
				else {
					if (*pFileOut != NULL) {
						fprintf(*pFileOut, "%s %d %d\n",
								pStr,
								(*((*ppNodes)->pCounters + 2 * (*ppNodes)->letterLeafIndex[index] )), 			// Word count
								(*((*ppNodes)->pCounters + 2 * (*ppNodes)->letterLeafIndex[index] + 1))		// Prefix count + 1 (next)
						);
					}
				}
			}
			//
			/* Go to the letter's children */
			if ((*ppNodes)->letterChildIndex[index] >= 1 && (*ppNodes)->letterChildIndex[index] <= ALPHABET_COUNT) {		// Has children
				LetterNodes** ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) + (*ppNodes)->letterChildIndex[index];
				//
				/* Continue to next child */
				printOutput(pFileOut, ppNext, currentDepth + 1, pStr);
			}
		}
		//
		*(pStr + currentDepth) = 0;
	}
}


/*
This function recursively increases the reverse prefix count for every dictionary word.
Input:
	ppNodes: The nodes structure (for 26 letters)
	currentCount: The data file word count at the current level to be propogating down the trie to be the prefix of all the downstream dictionary words.
Output:
	void
*/
void increaseReversePrefix(LetterNodes** ppNodes, int currentCount) {
	if ((*ppNodes) != NULL) {
		int index;
		for (index=0; index < ALPHABET_COUNT; index++) {
			if ((*ppNodes)->letterLeafIndex[index] < ALPHABET_COUNT) {																			// This is a leaf
				if ((*ppNodes)->pCounters == NULL ) {
					(*ppNodes)->pCounters = (int*)calloc(1, (*ppNodes)->leafCount * 2 * sizeof(int));
				}
				//
				(*((*ppNodes)->pCounters + 2 * (*ppNodes)->letterLeafIndex[index] + 1)) = currentCount;							// Data Word count
			}
			//
			/* Go to the letter's children */
			if ((*ppNodes)->letterChildIndex[index] >= 1 && (*ppNodes)->letterChildIndex[index] <= ALPHABET_COUNT) {		// Has children
				LetterNodes** ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) + (*ppNodes)->letterChildIndex[index];
				//
				/* Continue to next child */
				increaseReversePrefix(ppNext, currentCount + (*ppNodes)->dataWordCount[index]);
			}
		}
	}
}

/*
This function frees all the memory by going through the trie recursively.
Input:
	ppNodes: The nodes structure (for 26 letters)
Output:
	void
*/
void freeMemory(LetterNodes** ppNodes) {
	if ((*ppNodes) != NULL) {
		//
		int index;
		for (index=0; index < ALPHABET_COUNT; index++) {
			/* Go to the letter's children */
			if ((*ppNodes)->letterChildIndex[index] >= 1 && (*ppNodes)->letterChildIndex[index] <= ALPHABET_COUNT) {		// Has children
				LetterNodes** ppNext = ((LetterNodes**) &(*ppNodes)->pCounters) + (*ppNodes)->letterChildIndex[index];
				//
				/* Continue to next child */
				freeMemory(ppNext);
			}
		}
		//
		if ((*ppNodes)->pCounters) {
			free((*ppNodes)->pCounters);
		}
		//
		free(*ppNodes);
	}
}