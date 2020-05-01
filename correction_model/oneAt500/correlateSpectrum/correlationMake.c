/*
	=============================================================================
	Name: correlationMake.c
	Author: Ondrej Bily
	Description: Program pre korelaciu spektra ziarenia
	Date of latest update: 14.2.2019
	=============================================================================
	Run:		correlationMake.exe OR ./correlationMake

	Compile: 	gcc correlationMake.c -o correlationMake -lm
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

// defining constants
#define GAMMA 0.577
#define LAMBDA_0 0.5 // micrometres
#define TAU_A0 0.1 // from 0.1 to 0.4
#define ALPHA 1.
#define A 2.
#define B 20. // (micrometres)^-1
#define D 35000. // in metres
#define Z_DEG 70. // in degrees
#define MIN_LAMBDA 0.300
#define MAX_LAMBDA 0.800
#define Q_0 100000.

// used functions
double elipticValue(double x);
double calcXi(double lambda);
double calcg(double lambda);
double calcG(double lambda);
double calcTauA(double lambda);
double calcTauR(double lambda);
double calcRes(double lambda, double z);

int main(int argc, char *argv[]) {
	double lambda, intensity, res, z = Z_DEG*(M_PI/180.);
	char fileName[100], str[100];
	strcpy(fileName, argv[1]);
	FILE  *fr, *fw; // pointer to a file with output
	printf("\n%s\n", fileName);

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

		res = calcRes(lambda/1000., z);
		fprintf(fw, "%f\t%f\n", lambda, intensity*res);
		printf("%f\t%f\n", lambda, res);
	}

	if (fclose(fr) == EOF) {
		printf("Unable to close the file: test.txt\n");
		return 1;
	}
	if (fclose(fw) == EOF) {
		printf("Unable to close the file: test.txt\n");
		return 1;
	}

  return 0;
}

// Definitions of Fuctions used in main
double elipticValue(double x) {
  return 1.57-0.32*x-0.07*x*x-0.03*x*x*x-0.016*x*x*x*x;
}

double calcXi(double lambda) {
	return (4*calcg(lambda))/pow(1+calcg(lambda), 2);
}

double calcg(double lambda) {
  return (cos(calcG(lambda))*cos(calcG(lambda)))/(1+calcG(lambda)*calcG(lambda));
}

double calcG(double lambda) {
  return ((10.0*GAMMA*GAMMA+lambda*B)/(8*M_PI))*(8/(10+5*A*pow(GAMMA*M_PI, -2)));
}

double calcTauA(double lambda) {
  return TAU_A0*pow(lambda/LAMBDA_0, -ALPHA);
}

double calcTauR(double lambda) {
  return 0.00879*pow(lambda, -4.09);
}

double calcRes(double lambda, double z) {
  double 	res,
					coef1, part1, part2, part3, part21, part22, part31, part32,
					coef2, part4;

	// Aerosol's part of intensity: Ia
	coef1 = (1/cos(z))*(1/D)*(calcTauA(lambda)/(4*M_PI))*(1/(1-calcg(lambda)));
	part1 = elipticValue(calcXi(lambda));
	part21 = sqrt(1+calcg(lambda)*calcg(lambda))+calcg(lambda)-1;
	part22 = (0.5*M_PI-z)*(calcg(lambda)*sqrt(1+calcg(lambda)*calcg(lambda)))
						/pow(1-calcg(lambda),2);
	part2 = (0.5*M_PI-z)*(1/(2*(1+calcg(lambda))))*(part21+part22);
	part31 = sin(z)/sqrt(1+calcg(lambda)*calcg(lambda)+2*calcg(lambda)*cos(z));
	part32 = cos(z)/sqrt(1+calcg(lambda)*calcg(lambda)-2*calcg(lambda)*sin(z));
	part3 = (part31-part32)*(2*calcg(lambda))/(1+calcg(lambda));

	// Rayeligh's part of intensity: Ir
	coef2 = (3*calcTauR(lambda))/(D*16*M_PI*cos(z));
	part4 = 0.25*M_PI+0.5*z+0.25*sin(2*z);

	// Total intesity: I = Ia + Ir

	// together
	res = Q_0*coef1*(part1+part2+part3) + Q_0*coef2*part4;
	// rayleigh
    //res = Q_0*coef2*part4;
	// aerosol
    //res = Q_0*coef1*(part1+part2+part3);

  return res;
}
