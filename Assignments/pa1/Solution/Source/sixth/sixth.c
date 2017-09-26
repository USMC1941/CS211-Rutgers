#include <stdio.h>
#include <stdlib.h>


int main (int argc, const char* argv[]) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("error\n");
		return 0;
	}	
	
	char* output = (char*) malloc ((argc) * sizeof(char));		//argc-1 words + 1 for \0 terminate
	
	int i;
	for (i = 0; i < (argc-1); i++) {
		*(output + i) = *argv[i + 1];
	}
	//
	*(output + argc - 1) = '\0';

	printf("%s\n", output);

	free(output);
	
	return 0;
}
