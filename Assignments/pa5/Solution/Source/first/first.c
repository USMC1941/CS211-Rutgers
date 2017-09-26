#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first.h"



int main(int argc, char** argv) {
	if (argc<3) {
		fprintf(stderr,"ERROR: Input does not have the right number of inputs.\n");
		exit(1);
	}
	//
	char* circuit_file_name 	= argv[1];
	char* input_file_name 	= argv[2];
	//
	char	cctPoint[52+1];														// Alphabet. Upper and Lower case. 
	cctPoint[52] = 0;
	//
	Input in;
	in.isValid		= FALSE;
	in.line			= NULL;
	in.count		= 0;
	in.value			= NULL;
	//
	Output out;
	out.isValid		= FALSE;
	out.line			= NULL;
	out.count		= 0;
	out.value		= NULL;
	//
	Circuit cct;
	cct.listGate	= NULL;
	cct.count		= 0;
	//
	readCircuit(circuit_file_name, &in, &out, &cct);
	//
	if (!in.isValid) {
		printf("Input is not valid\n");
	}
	else if (!out.isValid) {
		printf("Output is not valid\n");
	}
	else {
		if (cct.count<=0) {
			printf("Circuit count is 0\n");
		}
		else {
			int i;
			for (i=0; i<cct.count; i++) {
				Gate* pGate = cct.listGate + i;
				if (!pGate->isValid) {
					printf("Gate %d is not valid\n", i);
				}
			}
		}
	}
	//
    FILE *file = fopen(input_file_name, "r");
	//
	if (file == NULL) {
		fprintf(stderr, "ERROR: Cannot open input file.\n");
	}
	else {
		char line[128];
		//
		while (fgets(line, 128, file) != NULL) {
			int i;
			for (i = 0; i < 52; i++) {
				cctPoint[i] = '?';
			}
			//
			for (i=0; i<cct.count; i++) {
				Gate* pGate = cct.listGate + i;
				pGate->resolved = FALSE;
			}
			//
			getCircuitOutput(line, &in, &out, &cct, cctPoint);
			//
			for (i=0; i<out.count; i++) {
					char a = *(out.value + 2 * i);
					int ia = getValue(a, cctPoint);
					if (i == 0) {
						printf("%d", ia);
					}
					else {
						printf(" %d", ia);
					}
			}
			//
			printf("\n");
		}
 	}
	//
	if (file) {
		fclose(file);
	}
	//
	free(in.line);
	free(out.line);
	//
	int i;
	for (i=0; i<cct.count; i++) {
		Gate* pGate = cct.listGate + i;
		free(pGate->line);
	}
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



int toThePower(int x, int n)  {
    int i;
    int number = 1;
	//
    for (i = 0; i < n; ++i) {
        number *= x;
	}
	//
    return number;
}



int getCctPointIndex(char input) {
	if (input >= 'A' && input <= 'Z') {							//0-25: A-Z
		return input - 'A';
	}
	else if (input >= 'a' && input <= 'z') {					//26-51: a-z
		return input - 'a' + 26;
	}
	else {
		return -1;
	}
}



int getValue(char input, char* cctPoint) {
	char out;
	if (input=='0' || input =='1') {
		out = input;
	}
	else {
		int index = getCctPointIndex(input);
		if (index>=0) {
			out = *(cctPoint + index);
		}
		else {
			out = '?';
		}
	}
	//
	if (out=='0')  {
		return 0;
	}
	else if (out=='1')  {
		return 1;
	}
	else {
		return -1;
	}
} 



/*
 * This function converts a reflected binary
 * Gray code number to a binary number.
 * Each Gray code bit is exclusive-ored with all
 * more significant bits.
 */
unsigned int grayToBinary(unsigned int num) {
    unsigned int mask;
    for (mask = num >> 1; mask != 0; mask = mask >> 1) {
        num = num ^ mask;
    }
	//
    return num;
}



unsigned int getDecFromGrayCodeString(char* input, int length, char* pCctPoint) {
	unsigned int output = 0;
	int i;
	for (i=0; i<length; i++) {
		char a = *(input + 2 * i);																		//2 * i : there is a space between chars
		int ia = getValue(a, pCctPoint);
		//
		if (ia>=0) {
			//output = output + ia * toThePower(2, i);
			output = output + ia * toThePower(2, length - i - 1);
		}
		else {
			output = -1;
			break;
		}
	}
	//
	if (output>=0) {
		output = grayToBinary(output);
	}
	//
	return output;
}




// Read Circuit
void readCircuit(char* circuit_file_name, Input* pIn, Output* pOut, Circuit* pCircuit) {
    FILE *file = fopen(circuit_file_name, "r");
	//
	if (file == NULL) {
		fprintf(stderr, "ERROR: Cannot open circuit file.\n");
	}
	else {
		char line[128];
		//
		while (fgets(line, 128, file) != NULL) {
			char* oneline = strdup(line);																			//strdup will allocate memory. Need to free at the end.
			//
			if (strstr(oneline, "INPUTVAR") != NULL) {
				char* pCount = strstr(oneline, "INPUTVAR") + strlen("INPUTVAR") + 1;		// + 1 for space
				//
				if (sscanf(pCount, "%d", &(pIn->count))==1) {
					pIn->isValid = TRUE;
					//
					if (pIn->count < 10) {																			//Example: 8 A
						pIn->value = pCount + 2;																	//length(count) + length(" ") = 2
					}
					else {																									//Example: 12 A
						pIn->value = pCount + 3;																	//length(count) + length(" ") = 3
					}
					//
					pIn->line  = oneline;
				} 
				else {
					pIn->isValid = FALSE;
				}
			}
			else if (strstr(oneline, "OUTPUTVAR") != NULL) {
				char* pCount = strstr(oneline, "OUTPUTVAR") + strlen("OUTPUTVAR") + 1;		// + 1 for space
				//
				if (sscanf(pCount, "%d", &(pOut->count))==1) {
					pOut->isValid = TRUE;
					//
					if (pOut->count < 10) {
						pOut->value = pCount + 2;
					}
					else {
						pOut->value = pCount + 3;
					}
					//
					pOut->line  = oneline;
				} 
				else {
					pOut->isValid = FALSE;
				}
			}
			else {
				Gate* pGate;
				if (pCircuit->count==0) {											// 1st gate
					pCircuit->count = 1;
					pCircuit->listGate = malloc(sizeof(Gate));
					pGate = pCircuit->listGate;
				} 
				else {
					pCircuit->count++;
					pCircuit->listGate = realloc(pCircuit->listGate, sizeof(Gate)*pCircuit->count);
					pGate = pCircuit->listGate + pCircuit->count - 1;
				}
				//
				pGate->resolved 	= FALSE;
				pGate->isValid		= FALSE;
				pGate->type			= GATE_INVALID;
				pGate->line			= oneline;
				//
				pGate->cnt_in		= 0;
				pGate->input		= NULL;
				//
				pGate->cnt_out	= 0;
				pGate->output		= NULL;
				//
				pGate->cnt_sgn	= 0;
				pGate->signal		= NULL;
				//
				//
				if (strstr(oneline, "AND") != NULL) {
					pGate->type		= GATE_AND;
					pGate->isValid	= TRUE;
					pGate->input	= strstr(oneline, "AND") + strlen("AND") + 1;
					pGate->output	= pGate->input + 4;
				}
				else if (strstr(oneline, "OR")  != NULL) {
					pGate->type		= GATE_OR;
					pGate->isValid	= TRUE;
					pGate->input	= strstr(oneline, "OR") + strlen("OR") + 1;
					pGate->output	= pGate->input + 4;
				}
				else if (strstr(oneline, "NOT")  != NULL) {
					pGate->type		= GATE_NOT;
					pGate->isValid	= TRUE;
					pGate->input	= strstr(oneline, "NOT") + strlen("NOT") + 1;
					pGate->output	= pGate->input + 2;
				}
				else if (strstr(oneline, "DECODER") != NULL) {
					pGate->type		= GATE_DECODER;
					//
					char* pCount = strstr(oneline, "DECODER") + strlen("DECODER") + 1;		// + 1 for space
					//
					if (sscanf(pCount, "%d", &(pGate->cnt_in))==1) {
						pGate->isValid = TRUE;
						//
						pGate->cnt_out	= toThePower(2, pGate->cnt_in);
						//
						if (pGate->cnt_in < 10) {
							pGate->input 	= pCount + 2;
						}
						else {
							pGate->input 	= pCount + 3;
						}
						//
						pGate->output	= pGate->input + 2 * (pGate->cnt_in);
					} 
					else {
						pGate->isValid = FALSE;
					}
					//
					continue;
				}
				else if (strstr(oneline, "MULTIPLEXER") != NULL) {
					pGate->type		= GATE_MULTIPLEXER;
					//
					char* pCount = strstr(oneline, "MULTIPLEXER") + strlen("MULTIPLEXER") + 1;		// + 1 for space
					//
					if (sscanf(pCount, "%d", &(pGate->cnt_in))==1) {
						pGate->isValid = TRUE;
						//
						pGate->cnt_sgn= logTwo(pGate->cnt_in);
						pGate->cnt_out	= 1;
						//
						if (pGate->cnt_in < 10) {
							 pGate->input	= pCount + 2;
						}
						else {
							pGate->input 	= pCount + 3;
						}
						//
						pGate->signal	= pGate->input  + 2 * (pGate->cnt_in);
						//
						pGate->output	= pGate->signal + 2 * (pGate->cnt_sgn) ;
					} 
					else {
						pGate->isValid = FALSE;
					}
				}
				else {
						//do nothing
				}
			}
		}
 	}
	//
	if (file) {
		fclose(file);
	}
}



void getCircuitOutput(char* line, Input* pIn, Output* pOut, Circuit* pCircuit, char* pCctPoint) {	
	int i;
	for (i=0; i < pIn->count; i++) {
		char charInLine 		= *(line + 2 * i);
		char charInInput	= *(pIn->value + 2 * i);
		//
		int index = getCctPointIndex(charInInput);
		//
		if (index>=0) {
			*(pCctPoint + index) = charInLine;
		}
	}
	//
	//printf("After Input: %s\n", pCctPoint);
	//
	int doMore;
	do {
		doMore = FALSE;
		//
		for (i=0; i<pCircuit->count; i++) {
			Gate* pGate = pCircuit->listGate + i;
			if (pGate->resolved) {
				continue;
			}
			else {
				if (pGate->type == GATE_AND) {
					char ca = *(pGate->input);
					char cb = *(pGate->input + 2);
					//
					int ia = getValue(ca, pCctPoint);
					int ib = getValue(cb, pCctPoint);
					//
					if (ia>=0 && ib>=0) {
						pGate->resolved = TRUE;
						doMore = TRUE;
						//
						int ic = ia * ib;
						//
						int index = getCctPointIndex(*(pGate->output));
						if (index>=0) {
							*(pCctPoint + index) =  '0' + ic; 
						}
						//
						//printf("After AND Gate: %s\n", pCctPoint);
					}
				}
				else if (pGate->type == GATE_OR) {
					char ca = *(pGate->input);
					char cb = *(pGate->input + 2);
					//
					int ia = getValue(ca, pCctPoint);
					int ib = getValue(cb, pCctPoint);
					//
					if (ia>=0 && ib>=0) {
						pGate->resolved = TRUE;
						doMore = TRUE;
						//
						int ic = ia + ib;
						if (ic > 1) {
							ic = 1;
						}
						//
						int index = getCctPointIndex(*(pGate->output));
						if (index>=0) {
							*(pCctPoint + index) =  '0' + ic; 
						}
						//
						//printf("After OR Gate: %s\n", pCctPoint);
					}
				}
				else if (pGate->type == GATE_NOT) {
					char ca = *(pGate->input);
					//
					int ia = getValue(ca, pCctPoint);
					//
					if (ia>=0) {
						pGate->resolved = TRUE;
						doMore = TRUE;
						//
						int ic;
						if (ia==0) {
							ic = 1;
						}
						else {
							ic = 0;
						}
						//
						int index = getCctPointIndex(*(pGate->output));
						if (index>=0) {
							*(pCctPoint + index) =  '0' + ic; 
						}
						//
						//printf("After NOT Gate: %s\n", pCctPoint);
					}
				}
				else if (pGate->type == GATE_DECODER) {
					int in_code = getDecFromGrayCodeString(pGate->input, pGate->cnt_in, pCctPoint);
					//
					if (in_code>=0) {
						pGate->resolved = TRUE;
						doMore = TRUE;
						//
						int i;
						for (i=0; i<pGate->cnt_out; i++) {
							char c = *(pGate->output + i*2);
							int index = getCctPointIndex(c);
							//
							if (i==in_code) {
								*(pCctPoint + index) =  '1'; 
							}
							else {
								*(pCctPoint + index) =  '0'; 
							}
						}
						//
						//printf("After DECODER Gate: %s\n", pCctPoint);
					}
				}
				else if (pGate->type == GATE_MULTIPLEXER) {
					int in_code = getDecFromGrayCodeString(pGate->signal, pGate->cnt_sgn, pCctPoint);
					//
					int ia;
					if (in_code>=0) {
						char ca = *(pGate->input + 2 * in_code);
						ia = getValue(ca, pCctPoint);
					}
					//
					if (in_code>=0 && ia>=0) {															//both signal and input present
						pGate->resolved = TRUE;
						doMore = TRUE;
						//
						char c = *(pGate->output);
						int index = getCctPointIndex(c);
						//
						if (ia>0) {
							*(pCctPoint + index) =  '1'; 
						}
						else {
							*(pCctPoint + index) =  '0'; 
						}
						//
						//printf("After MULTIPLEXER Gate: %s\n", pCctPoint);
					}
				}
			}
		}
	} while (doMore);
}
