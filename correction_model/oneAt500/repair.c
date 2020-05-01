/*
	=============================================================================
	Name: repair.c
	Author: Ondrej Bily
	Description: Program pre opravenie dat tak, aby pri 500 nm bola 1
	Date of latest update: 14.2.2019
	=============================================================================
	Run:		./repair <filename>

	Compile: 	gcc repair.c -o repair -lm
	=============================================================================
*/

// system header files
#include 	<math.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<unistd.h>

// recognise OS
#if _WIN32
  	// OS = Windows
	const int os = 1;
#else
#ifdef __linux__
	// OS = Linux
   	const int os = 2;
//#else
//#ifdef (__APPLE__) && defined(__MACH__)
//	// OS = Apple
//  const int os = 3;
#else
	// unknown OS
   	const int os = 0;
//#endif
#endif
#endif

int main(int argc, char *argv[]) {

	double lambda, intensity, res;
	char fileName[100], str[100];
	strcpy(fileName, argv[1]);
	FILE  *fr, *fw; // pointer to a file with output
	printf("\n%s\n", fileName);

	// I have opened file that I need to repair
	if ((fr = fopen(fileName, "r")) == NULL) {
		printf("Unable to open the file: test.txt\n");
		return 1;
	}

	// find intensity at 500 nm
	while (fgets(str, 100, fr) != NULL) {
		//printf("%s\n", str);
		sscanf(str, "%lf\t%lf", &lambda, &intensity);

		//printf("%f\n", lambda);
		if (lambda == 500.) {
			res = intensity;
			break;
		}
	}

	if (fclose(fr) == EOF) {
		printf("Unable to close the file: data.txt\n");
		return 1;
	}

	// repair the file

	// I have opened file that I need to repair
	if ((fr = fopen(fileName, "r")) == NULL) {
		printf("Unable to open the file: test.txt\n");
		return 1;
	}
	// open new file
	//char newFileName[200];
	strcat(fileName, "_cor");
	//strcpy("cor_" + fileName, newFileName);
	if ((fw = fopen(fileName, "w")) == NULL) {
		printf("Unable to open the file: newtest.txt\n");
		return 1;
	}

	// repair all numbers
	while (fgets(str, 100, fr) != NULL) {
		//printf("%s\n", str);
		sscanf(str, "%lf\t%lf", &lambda, &intensity);

		fprintf(fw, "%f\t%f\n", lambda, intensity/res);
	}

	if (fclose(fr) == EOF) {
		printf("Unable to close the file: test.txt\n");
		return 1;
	}
	if (fclose(fw) == EOF) {
		printf("Unable to close the file: test.txt\n");
		return 1;
	}

	//printf("%f\n", res);
	return 0;
}
