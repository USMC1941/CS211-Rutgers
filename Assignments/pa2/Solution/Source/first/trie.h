#define ALPHABET_COUNT 											26

#define CHILD_PROPERTY_NOT_PRESENT 						0b00000000
#define CHILD_PROPERTY_PRESENT_WITHOUT_CHILD		0b10000000
//#define CHILD_PROPERTY_PRESENT_WITH_CHILD 		from 1 to 26

#define LEAF_PROPERTY_NOT_PRESENT 							0b10000000
#define LEAF_PROPERTY_PRESENT_NOT_LEAF					0b01000000
//#define LEAF_PROPERTY_PRESENT_WITH_LEAF 			from 0 to 25


#define TRUE 														1
#define FALSE 														0


typedef struct LetterNodes {
	unsigned char 	letterChildIndex		[ALPHABET_COUNT];
	unsigned char 	childCount;
	unsigned char 	letterLeafIndex		[ALPHABET_COUNT];
	unsigned char 	leafCount;
	int* 	pCounters;
} LetterNodes;

LetterNodes** insertLetter(LetterNodes** ppNodes, int* pMaxDepth, int *pCurrentDepth, int input, int hasMore);

LetterNodes** checkLetterForWordAndPrefix(LetterNodes** ppNodes, int input, int hasChild);

void printOutput(FILE** pFileOut, LetterNodes** ppNodes, int currentDepth, char* pStr);

void freeMemory(LetterNodes** ppNodes);