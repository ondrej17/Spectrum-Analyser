/*
	=============================================================================
	Name: correlationCalc.c
	Author: Ondrej Bily
	Description: Program pre korelaciu spektra ziarenia
	Date of latest update: 14.2.2019
	=============================================================================
	Run:		./correlationCalc

	Compile: 	gcc correlationCalc.c -o correlationCalc -lm
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
#define TAU_A0 0.1
#define ALPHA 1.
#define A 2.
#define B 20. // (micrometres)^-1
#define D 8000. // in metres
#define MIN_LAMBDA 0.300
#define MAX_LAMBDA 0.800
#define Q_0 100000.

// used functions
double elipticValue(double fi, double xsi);
double f(double theta,double xsi);
double calcXi(double lambda);
double calcg(double lambda);
double calcG(double lambda);
double calcTauA(double lambda);
double calcTauR(double lambda);
double calcRes(double lambda, double z);

int main(int argc, char *argv[]) {

  	FILE  *fw; // pointer to a file with output
	double  lambda, res, z_deg, z;

	// open output file
	if ((fw = fopen("data.txt", "w")) == NULL) {
		printf("Unable to open the file: data.txt\n");
		return 1;
	}
	
	fprintf(fw, "0\t");	
	for (z_deg = 60.; z_deg <= 60.; z_deg += 1.) {
		fprintf(fw, "%f\t", z_deg);
	}
	fprintf(fw, "\n");
	
  	// START main loop that calculates correlation
  	for (lambda = MIN_LAMBDA; lambda <= MAX_LAMBDA; lambda += 0.001) {

		fprintf(fw, "%f\t", lambda);
				
		for (z_deg = 60.; z_deg <= 60.; z_deg += 1.) {
		
			z = z_deg*(M_PI/180.);

			res = calcRes(lambda, z);

			fprintf(fw, "%f\t", res);
		}

		fprintf(fw, "\n");
  	}

  	if (fclose(fw) == EOF) {
		printf("Unable to close the file: data.txt\n");
		return 1;
	}

  	return 0;
}

// Definitions of Fuctions used in main

double f(double theta, double xsi)
{
        return sqrt(1-xsi*sin(theta)*sin(theta));
}

double elipticValue(double fi, double xsi) {
    double a,b,h,vysledok1,vysledok2;
    long n,i;
    int presnost_dosiahnuta;

    //Vypocet adaptivnou Simpsonovou metodou
    a=fi; b=M_PI/4+fi; //hranice integrovania
    n=10; //pociatocny pocet intervalov
    vysledok1 = 0;
    presnost_dosiahnuta = 0;
    while(presnost_dosiahnuta == 0)
    {
        if((n%2)==1) n++;
        h = (b-a)/n;
        vysledok2 = 0;
        for(i=0;i<=n;i++)
        {
            if((i==0)||(i==n))
                vysledok2 += f(a+i*h,xsi);
            else
                if((i%2)==1)
                    vysledok2 += 4*f(a+i*h,xsi);
                else
                    vysledok2 += 2*f(a+i*h,xsi);
        }
        vysledok2 *= h/3;
        if(fabs((vysledok2-vysledok1)/vysledok1)<1e-6)
        {
            presnost_dosiahnuta = 1;
        }
        vysledok1 = vysledok2;
        n = 2*n;
    }



return vysledok2;
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
					coef1, part1,  part3, part31, part32,
					coef2, part4;

	// Aerosol's part of intensity: Ia
	coef1 = (1/cos(z))*(1/D)*(calcTauA(lambda)/(4*M_PI))*(1/(1-calcg(lambda)));
	part1 = elipticValue(z/2,calcXi(lambda));

	part31 = sin(z)/sqrt(1+calcg(lambda)*calcg(lambda)+2*calcg(lambda)*cos(z));
	part32 = cos(z)/sqrt(1+calcg(lambda)*calcg(lambda)-2*calcg(lambda)*sin(z));
	part3 = (part31-part32)*(2*calcg(lambda))/(1+calcg(lambda));

	// Rayeligh's part of intensity: Ir
	coef2 = (3*calcTauR(lambda))/(D*16*M_PI*cos(z));
	part4 = 0.25*M_PI+0.5*z+0.25*sin(2*z);

	// Total intesity: I = Ia + Ir

	// together
	res = Q_0*coef1*(part1+part3) + Q_0*coef2*part4;
	//res = (Q_0*coef1*(part1+part3)) / (Q_0*coef2*part4); //pomer na testovanie
	// rayleigh
	//res = Q_0*coef2*part4;
	// aerosol
	//res = Q_0*coef1*(part1+part3);

  return res;
}
