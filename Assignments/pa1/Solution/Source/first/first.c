#include <stdio.h>
#include <stdlib.h>


#define IS_PRIME 1
#define NOT_PRIME 0



/*Checks whether a number is prime or not*/
void checkIfPrime (int input);


int main (int argc, const char* argv[]) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("error\n");
		return 0;
	}	
	
	int input;
	input = atoi(argv[1]);
	checkIfPrime(input);
	
	return 0;
}




void checkIfPrime (int input) {
	int itIsPrime;
	
	if (input <= 0) {
		itIsPrime = NOT_PRIME;
	}
	else if (input == 1) {
		itIsPrime = NOT_PRIME;													// 1 is excluded as a prime number, for reasons explained WIKI. 
	} 
	else if (input == 2) {															// 2 is a prime number
		itIsPrime = IS_PRIME;
	} 
	else {																					// >2
		itIsPrime = IS_PRIME;
		//
		int theDivisor;
		for (theDivisor = 2; theDivisor < input; theDivisor++) {
			if (input % theDivisor == 0) {
				itIsPrime = NOT_PRIME;
				break;
			}
		}
	}
	
	if (itIsPrime == NOT_PRIME) {
		printf("no\n");
	}
	else {
		printf("yes\n");
	}
}
