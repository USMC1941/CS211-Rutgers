#define FALSE 0
#define TRUE 1

// Cache Block
typedef struct  cache_block {
	unsigned long 	tag;
	//char*				pData;		//2^len_offset bytes of data.
	int					valid;		//Is theBlock valid
	//int					dirty;		//For write_back. Since we only use write_through. not needed.
} cache_block;


typedef struct cache_set {
	cache_block* 	pCacheBlock;						//array of CacheBlock
	int					index_to_be_reused;			//For Replacement algorithm: First In First Out (FIFO) 
} cache_set;


typedef struct counter {
	unsigned int		hit;
	unsigned int		miss;
	unsigned int		read;
	unsigned int		write;
} counter;


// Specifications of the cache
typedef struct Cache_Spec {
	int		cache_size;				//how many bytes in total of all blocks
	int		cache_setsize;			//how many blocks in one set
	int		block_size;				//how many bytes in one block
	int 		len_address;				//48 bits in this situation
	//
	int		number_of_blocks;	//# of blocks
	int		number_of_sets;		//# of sets
	//
	int		len_offset;					//from block_size
	int		len_index;					//from cache_setsize
	int		len_tag;					//len_address - len_offset - len_index
	//
	int		start_offset;				//always 0
	int		start_index;				//=len_offset
	int		start_tag;					//=len_address - len_offset - len_index
} Cache_Spec;


// log 2 function
int logTwo(int input);

// Parsing address into unsigned long. Example 0x87987654
unsigned long hexToLong(char* input);

// Abstract bits
unsigned long getPartFromLong(unsigned long input, int start, int length);

//Output
void printCounter(char* title, counter* pcounter);

void printCacheSpec(char* title, Cache_Spec* pcs);

//Simulate read or write one byte from memory via Cache
void readOrWriteOneByte(int isRead, Cache_Spec* pcs, cache_set* pcache, unsigned long address, counter* pcounter);

int initializeCacheSpec(Cache_Spec* pcs, char* szCacheSize, char* associativity, char* szBlockSize, int addressWidth, int normalLayout);

//Initialize each block
void initializeBuffer(Cache_Spec* pcs, cache_block* pcacheData, cache_set* pcache);

void doSimulation(char* trace_file_name, Cache_Spec* pcs, cache_set* pcache, counter* pcounter);