#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "first.h"

// Populate Cache_Spec
int main(int argc, char** argv) {
	if (argc<5) {
		fprintf(stderr,"ERROR: Input does not have the right number of inputs.\n");
		exit(1);
	}
	//
	//
	Cache_Spec cs_A;
	Cache_Spec cs_B;
	char* trace_file_name;
	//
	int retA = initializeCacheSpec(&cs_A, argv[1], argv[2], argv[3], 48, TRUE);
	//printCacheSpec("A", &cs_A);
	if (retA==FALSE) {
		fprintf(stderr,"ERROR: Could not initialize cache specification for test A.\n");
		exit(1);
	}
	//
	int retB = initializeCacheSpec(&cs_B, argv[1], argv[2], argv[3], 48, FALSE);
	//printCacheSpec("B", &cs_B);
	if (retB==FALSE) {
		fprintf(stderr,"ERROR: Could not initialize cache specification for test B.\n");
		exit(1);
	}
	//
	trace_file_name = argv[4];
	//
	//
	counter ct_a;
	ct_a.hit		= 0;
	ct_a.miss	= 0;
	ct_a.read	= 0;
	ct_a.write	= 0;
	//
	counter ct_b;
	ct_b.hit		= 0;
	ct_b.miss	= 0;
	ct_b.read	= 0;
	ct_b.write	= 0;
	//
	//
	cache_block* 	pcacheData 	= malloc(sizeof(cache_block) * cs_A.number_of_blocks);										// Using spec_a since A and B the same
	cache_set* 		pcache 			= malloc(sizeof(cache_set) * cs_A.number_of_sets);												// Using spec_a since A and B the same
	//
	// A
	//printf("Initialize Buffer A");
	initializeBuffer(&cs_A, pcacheData, pcache);
	//printf("Run Simulate A");
	doSimulation(trace_file_name, &cs_A, pcache, &ct_a);
	//printf("Complete A");
	//
	// B
	//printf("Initialize Buffer B");
	initializeBuffer(&cs_B, pcacheData, pcache);
	//printf("Run Simulate B");
	doSimulation(trace_file_name, &cs_B, pcache, &ct_b);
	//printf("Complete B");
	//
	printCounter("A", &ct_a);
	printCounter("B", &ct_b);
	//
	free(pcacheData);
	free(pcache);
	//
	return 0;
}



// log 2 function
int logTwo(int input) {
	int index = 0;
	for (index = 0; index < 31; index++) {
		if ((input >> index) == 1)	{
			break;
		}
	}
	//
	return index;
}



// Parsing address into unsigned long. Example 0x87987654
unsigned long hexToLong(char* input) {
	unsigned long output = 0;
	int index = 0;
	//
	if (*(input + index) == '0' && (*(input + index + 1) == 'x' || *(input + index + 1) == 'X') ) {
		index = index + 2;
	}
	//
	while (*(input + index) != 0) {
		char oneChar = *(input + index);
		output = output * 16;
		//
		if (oneChar > '0' && oneChar <= '9') {						//number
			output = output + (oneChar - '0');
		}
		else if (oneChar >= 'a' && oneChar <= 'f') {				//Letters
			output = output + (oneChar - 'a') + 10;
		}
		else if (oneChar >= 'A' && oneChar <= 'F') {
			output = output + (oneChar - 'A') + 10;
		}
		else {
			//do nothing
		}
		//
		index++;
	}
	//
	return output;
}


// Abstract bits
unsigned long getPartFromLong(unsigned long input, int start, int length) {
	unsigned long output;
	//
	if (length == 0) {
		output = 0;
	}
	else {
		output  = input << (64  - length - start);
		output  = output >> (64  - length);					//for unsigned type, the shift is logical
	}
	//
	return output;
}


//Output
void printCounter(char* title, counter* pcounter) {
	printf("cache %s\n", title);
	//
	printf("Memory reads: %d\n", 	pcounter->read);
	printf("Memory writes: %d\n", 	pcounter->write);
	printf("Cache hits: %d\n", 			pcounter->hit);
	printf("Cache misses: %d\n", 	pcounter->miss);
}

/*
void printCacheSpec(char* title, Cache_Spec* pcs) {
	printf("Cache Specification %s\n", title);
	//
	printf("Cache size: %d\n", pcs->cache_size);
	printf("Cache set size: %d\n", pcs->cache_setsize);
	printf("Block size: %d\n", pcs->block_size);
	printf("Length of address: %d\n", pcs->len_address);
	printf("# of blocks: %d\n", pcs->number_of_blocks);
	printf("# of sets: %d\n", pcs->number_of_sets);
	printf("Length of offset: %d\n", pcs->len_offset);
	printf("Length of index: %d\n", pcs->len_index);
	printf("Length of tag: %d\n", pcs->len_tag);
	printf("Start of offset: %d\n", pcs->start_offset);
	printf("Start of index: %d\n", pcs->start_index);
	printf("Start of tag: %d\n", pcs->start_tag);
}
*/


//Simulate read or write one byte from memory via Cache
void readOrWriteOneByte(int isRead, Cache_Spec* pcs, cache_set* pcache, unsigned long address, counter* pcounter) {
	unsigned long 	tag 		= getPartFromLong(address, pcs->start_tag, pcs->len_tag);
	int  					index 	=  (int)getPartFromLong(address, pcs->start_index, pcs->len_index);
	//int 					offset 	=  (int)getPartFromLong(address, pcs->start_offset, pcs->len_offset);
	//
	cache_set* pTheSet = pcache + index;
	//
	int found = FALSE;
	// Hit
	int i;
	for (i=0; i < pcs->cache_setsize; i++) {
		cache_block* pOneBlock = (pTheSet->pCacheBlock + i);
		if (pOneBlock->tag==tag && pOneBlock->valid==TRUE) {
			if (isRead) {
				//output = pOneBlock->data[offset];
				pcounter->hit++;
			}
			else {
				//pOneBlock->data[offset] = input;
				pcounter->hit++;
				//write the whole block to memory as write through
				pcounter->write++;
			}
			//
			found = TRUE;
			//
			break;
		}
	}
	//
	// Miss
	if (found==FALSE) {
		cache_block* pOneBlock = (pTheSet->pCacheBlock + pTheSet->index_to_be_reused);
		//
		// Prepare for next miss. Reuse this block
		pTheSet->index_to_be_reused++;
		pTheSet->index_to_be_reused = pTheSet->index_to_be_reused % pcs->cache_setsize;
		//
		//Read the whole block from memory
		pcounter->read++;
		//
		/*
		 For a write-through cache, there is the question of what should happen in case of a write miss. In this
		assignment, the assumption is that the block is first read from memory (one read memory), and then
		followed by a memory write.
		*/
		//
		pOneBlock->tag = tag;
		pOneBlock->valid = TRUE;
		//
		if (isRead) {
			//output = pOneBlock->data[offset];
			pcounter->miss++;
		}
		else {
			//pOneBlock->data[offset] = input;
			pcounter->miss++;
			//write the whole block to memory as write through
			pcounter->write++;
		}
	}
}


int initializeCacheSpec(Cache_Spec* pcs, char* szCacheSize, char* associativity, char* szBlockSize, int addressWidth, int normalLayout) {	// normalLayout is TRUE for test A, FALSE for test B
	int ret;
	//
	pcs->cache_size		= atoi(szCacheSize);
	pcs->block_size			= atoi(szBlockSize);
	//
	pcs->number_of_blocks = pcs->cache_size / pcs->block_size;
	//
	//printf("%s %s %s\n", szCacheSize, associativity, szBlockSize);
	//printf("%d %d\n", pcs->cache_size, pcs->block_size);
	//
	if (strcmp(associativity,"direct")==0) {
		pcs->cache_setsize	= 1;
		ret = TRUE;
	}
	else if (strcmp(associativity, "assoc")==0) {
		pcs->cache_setsize	= pcs->number_of_blocks;
		ret = TRUE;
	}
	else {
		char* st = strstr (associativity, "assoc:");
		if (st != NULL) {
			pcs->cache_setsize	= atoi(associativity+strlen("assoc:") );
			ret = TRUE;
		}
		else {
			ret = FALSE;
		}
	}
	//
	pcs->number_of_sets = pcs->number_of_blocks / pcs->cache_setsize;
	//
	pcs->len_address = addressWidth;
	//
	pcs->len_offset	= logTwo(pcs->block_size);															//from block_size
	pcs->len_index	= logTwo(pcs->number_of_sets);													//from number_of_sets
	pcs->len_tag		= pcs->len_address - pcs->len_offset - pcs->len_index;					//len_address - len_offset - len_index
	//
	if (normalLayout) {
		// Type A (Normal)
		pcs->start_offset	= 0;
		pcs->start_index	= pcs->len_offset;
		pcs->start_tag		= pcs->len_offset + pcs->len_index;
	}
	else {
		// Type B (Abnormal)
		pcs->start_offset	= 0;
		pcs->start_tag		= pcs->len_offset;
		pcs->start_index	= pcs->len_offset + pcs->len_tag;
	}
	//
	if (ret==TRUE) {
		if (pcs->cache_size <= 0) {
			ret = FALSE;
		}
		else if (pcs->block_size <= 0) {
			ret = FALSE;
		}
		else if (pcs->cache_setsize <= 0) {
			ret = FALSE;
		}
		else if (pcs->number_of_blocks <= 0) {
			ret = FALSE;
		}
		else if (pcs->number_of_sets <= 0) {
			ret = FALSE;
		}
		else if (pcs->cache_size != pcs->block_size * pcs->number_of_blocks) {
			ret = FALSE;
		}
		else if (pcs->cache_size != pcs->block_size * pcs->cache_setsize * pcs->number_of_sets) {
			ret = FALSE;
		}
		else {
			// Do Nothing
		}
	}
	//
	return ret;
}



//Initialize each block
void initializeBuffer(Cache_Spec* pcs, cache_block* pcacheData, cache_set* pcache) {
	int i;
	//fprintf(stderr,"Start initialize block.\n");
	for (i=0; i< pcs->number_of_blocks; i++) {
		//fprintf(stderr,"Block %d.\n", i);
		cache_block* pOneCacheBlock = pcacheData + i;
		pOneCacheBlock->tag	= 0;
		pOneCacheBlock->valid	= FALSE;
	}
	// First In First Out (FIFO)
	//fprintf(stderr,"Start initialize set.\n");
	for (i=0; i < pcs->number_of_sets; i++) {
		//fprintf(stderr,"Set %d.\n", i);
		cache_set* pOneSet = pcache + i;
		pOneSet->pCacheBlock	= pcacheData + i * pcs->cache_setsize;
		pOneSet->index_to_be_reused	= 0;
	}
}


// Run simulation
void doSimulation(char* trace_file_name, Cache_Spec* pcs, cache_set* pcache, counter* pcounter) {
    FILE *file = fopen(trace_file_name, "r");
	//
	if (file == NULL) {
		fprintf(stderr, "ERROR: Cannot open file.\n");
	}
	else {
		// Filename
		char line[128];
		char program_counter[32];
		char mode;
		char address[32];
		
		while (fgets(line, 128, file) != NULL) {
			int iCount = sscanf(line, "%s %c %s", program_counter, &mode, address);
			if (iCount==3) {
				if (mode=='R' || mode=='r') {
					readOrWriteOneByte(TRUE, pcs, pcache, hexToLong(address), pcounter);
				}
				else if (mode=='W' || mode=='w') {
					readOrWriteOneByte(FALSE, pcs, pcache, hexToLong(address), pcounter);
				}
				else {																																		//any other
					//do nothing but continue
				}
			}
			else if (iCount==1) {
				if (strcmp(program_counter, "#eof")==0) {
					break;
				}
				else {
					//do nothing but continue
				}
			}
			else {																																			//iCount=0, 2
				//do nothing but continue
			}
		}
 	}
	//
	if (file) {
		fclose(file);
	}
}
