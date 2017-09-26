#define FALSE 0
#define TRUE 1



#define GATE_INVALID			0
#define GATE_AND				1
#define GATE_OR					2
#define GATE_NOT				3
#define GATE_DECODER		4
#define GATE_MULTIPLEXER	5



typedef struct Input {
	int		isValid;
	char*	line;
	//
	int		count;
	char* 	value;
} Input;



typedef struct Output {
	int		isValid;
	char* 	line;
	//
	int		count;
	char* 	value;
} Output;



typedef struct Gate {
	int		resolved;							//to be initialized to FALSE for every run of input
	int		isValid;
	int		type;
	char * 	line;
	//
	int		cnt_in;
	char*	input;
	//
	int		cnt_out;
	char*	output;
	//
	int		cnt_sgn;
	char*	signal;
} Gate;



typedef struct Circuit {
	Gate* 	listGate;						//Array of Gate, length = count
	int		count;
} Circuit;






// log 2 function
int logTwo(int input) ;

int toThePower(int x, int n) ;

int getCctPointIndex(char input);

int getValue(char input, char* cctPoint);

/*
 * This function converts a reflected binary
 * Gray code number to a binary number.
 * Each Gray code bit is exclusive-ored with all
 * more significant bits.
 */
unsigned int grayToBinary(unsigned int num);

unsigned int getDecFromGrayCodeString(char* input, int length, char* pCctPoint);

// Read Circuit
void readCircuit(char* circuit_file_name, Input* pIn, Output* pOut, Circuit* pCircuit);

void getCircuitOutput(char* line, Input* pIn, Output* pOut, Circuit* pCircuit, char* pCctPoint);

