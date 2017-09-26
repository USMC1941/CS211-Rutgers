#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, const char* argv[]) {
	/*No input in the command line*/
	if (argc < 2) {
		printf("error\n");
		return 0;
	}	
	
	char* output = (char*) malloc ((argc) * sizeof(char));
	
	int i;
	for (i = 0; i < (argc-1); i++) {
		const char* str = argv[i + 1];
		int len = strlen(str);
		*(output + i) = *(str + len - 1);
	}
	//
	*(output + argc - 1) = '\0';

	printf("%s\n", output);

	free(output);
	
	return 0;
}
