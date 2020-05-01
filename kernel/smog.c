/*
	==============================================================================================
	Name: smog.c
	Author: Ondrej Bily
	Description: SMOG calculates percentage of different light sources in given spectrum.
	Date of latest update: 25.6.2019
	==============================================================================================


	------------------------- PROGRAM SMOG -------------------------------------------------------
	Compile:	gcc -Wall smog.c -o smog -lm
	Run:		./smog <min_lambda> <max_lambda> <delta_lambda>     <file_name>       <spectrum>
	Example:	./smog 	 330           770            23		list_of_lights.txt	experiment.txt


	==============================================================================================
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
#else
#ifdef (__APPLE__) && defined(__MACH__)
	// OS = Apple
	const int os = 3;
#else
	// unknown OS
   	const int os = 0;
#endif
#endif
#endif

// local definitions of constants
#define		MAX				150
#define		LAMBDA_0		500.

// DEBUGGING MODE - printing useless files and other
// To access debugging mode, uncommennt this:
//#define DEBUGGER_MODE

// definitions of static global variables
double	*matrix_x;						// matrix X
double	*matrix_orig;					// matrix ORIG
double	*vector_y;						// matrix Y - measured spectrum
double	*vector_orig;
double	*matrix_x_t;					// matrix X transpose
double	*matrix_x_x_t;					// matrix X times X transpose
double	*matrix_x_x_t_1;				// inverted matrix X times matrix transpose
double	*matrix_m;						// matrix M
double	*vector_c;						// vector C - coeficients of lights
double	*vector_xc;						// matrix X times vector C - calculated spectrum
double	*vector_orig_c;
double	*vector_sigma;					// vector SIGMA - deviation of each coeficient
struct {								// structure - temporary contains iformation about light
	char 	name[100];					// name of light without spaces and same as the name of file with spectrum of light
	char	fullName[150];				// full-name of file with itegrated spectrum of light
	char	fullNameOrig[150];
	int		partic;						// participation of light into calculation (1 or 0)
	double 	lambda;						// wavelength
	double 	intensity;					// signal at particuliar wavelegth
} light;
struct {								// structure - used during saving measured and calculated spectra to png
	double	lambda;						// wavelength
	double	experiment;					// measured value
	double	calculation;				// calculated value
	double	lambda_light;
} graph;

// function prototype of all used functions
void 	print_matrix(double *matrix, int rows, int cols, char file[30]);
void 	input_to_matrix(double *matrix, int rows, int cols, int row_act, int col_act, double input);
double 	element_of_matrix(double *matrix, int rows, int cols, int row_act, int col_act);
double 	*init(int rows, int cols);
void	transpose(double *matrix_origin, double *matrix_transponed, int rows, int cols);
double 	invert_matrix(double *matrix, double *matrix_i, int size);
int 	setNumN(char adress[200]);
void	change_participation(char name[100]);

// MAIN() function with parameters
int 	main(int argc, char *argv[])
{
	// checking for correct command from terminal
	if(argc != 6)
    {
    	printf("--------------------------------------------------------------------------------\n");
    	printf("                                Program SMOG\n");
    	printf("--------------------------------------------------------------------------------\n");
    	printf("Run:	 ./smog <min_lambda> <max_lambda> <delta_lambda> <file_name> <spectrum>\n");
     	printf("Example: ./smog 330 770 23 list_of_lights.txt experiment.txt\n\n");
     	return 1;
    }

    // LOG FILE
	FILE	*logFile;

	// OPEN LOG FILE
	if ((logFile = fopen("logFileKernel.txt", "w")) == NULL) {
		#ifdef DEBUGGER_MODE
		printf("LOG FILE COULD NOT BE OPENED !\n");
		#endif
		return 1;
	}
	else {
		fprintf(logFile, "KERNEL:\n");
		#ifdef DEBUGGER_MODE
		printf("LOG FILE CREATED\n");
		#endif
		setbuf(logFile, NULL);
	}

	// light databse - must be in interval <min_lambda, max_lambda>
	char cutCommand[100];
	if (os == 1) {
		sprintf(cutCommand, "cut_spectra %f %f", atof(argv[1]), atof(argv[2]));
		system(cutCommand);
	}
	else if (os == 2) {
		sprintf(cutCommand, "./cut_spectra %f %f", atof(argv[1]), atof(argv[2]));
		system(cutCommand);
	}
	else if (os == 3) { }
	else {  }

	// light databse - must have integral equal to 1
	if (os == 1) 		system("scale_to_one");
	else if (os == 2) 	system("./scale_to_one");
	else if (os == 3) {	/* do something*/ }
	else { /* I don't know what to do here*/ }

	// variables used in program
	FILE	*fr1, *fwr1, *fr2, *fr3, *fw4, *fr4, *fr5, *fr6, *fw7, *fr7, *fw8, *fw9; 	// pointers for used files
	int		a, b, bb, i, j, k, l, n, n_orig;
	double	sum, pom, sigma2, sigma, sumOfCoeficients = 0.;
	char	filename[200], str[1000], command1[1000], command2[1000], command3[1000], str1[1000], str2[1000];

	// variables loaded from start-command
	double	min_lambda = atof(argv[1]);		// minimal wavelength (start of integration)
	double	max_lambda = atof(argv[2]);		// maximal wavelength (end of integration)
	//double	ang_exp = atof(argv[3]);		// Angstrom exponent
	double	delta_lambda = atof(argv[3]);	// step of integration of spectra
	char	config_file[100];
	strcpy(config_file, argv[4]);			// name of configuration file with lights
	char	spectrum_file[100];
	strcpy(spectrum_file, argv[5]);
	#ifdef DEBUGGER_MODE
	printf("%s\n", spectrum_file);
	#endif

	char 	spectrum[200];
	sprintf(spectrum, "input_data/%s", spectrum_file);

	// LOADING OF USED LIGHTS IN CALCULATION AND INTEGRATION OF THEIR SPECTRA

	// make path to configuration file
	char	config_file_path[200];
	sprintf(config_file_path, "input_data/%s", config_file);

	if ((fr1 = fopen(config_file_path, "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: %s\n", config_file_path);
		return 1;
	}
	if ((fwr1 = fopen("results/list_of_used_lights.txt", "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"results/list_of_used_lights.txt\"\n");
		return 1;
	}

	// loading used lights
	// creating a new file with only used lights
	// starting program spectrumavg.exe
	k = 0;
	// read all lines in file
	while (fgets(str, MAX, fr1) != NULL)
	{
		// ignore lines with '#' at the beginning
		if ((str[0] != '#'))
		{
			// load name of light and participation to the structure 'light'
		 	sscanf(str, "%s\t%d", light.name, &light.partic);

		 	// if partic = 1, then start spectrumavg.exe, which create a new file with integrated spectrum with same name, but .dat extension
			if (light.partic == 1)
		 	{
		 		// make string (command) to start spectrumavg.exe
		 		if (os == 1) {
					sprintf(command1, "SPECTRUMAVG %f %f %f spectra_used/%s.txt spectra_integrated/%s.dat", delta_lambda, min_lambda, max_lambda, light.name, light.name);
		 			system(command1);
				}
				else if (os == 2) {
					sprintf(command1, "./spectrumavg %f %f %f spectra_used/%s.txt spectra_integrated/%s.dat", delta_lambda, min_lambda, max_lambda, light.name, light.name);
		 			system(command1);
				}
				else if (os == 3) {
					// do something
				}
				else {
					// I don't know what to do here
				}

				// find the number of rows with wavelength in integrated spectrum
				sprintf(light.fullName, "spectra_integrated/%s.dat", light.name);
				sprintf(light.fullNameOrig, "spectra_used/%s.txt", light.name);
				n = setNumN(light.fullName);
				n_orig = setNumN(light.fullNameOrig);

				// name a participation of light write in separated file
				fprintf(fwr1, "%s\t%d\n", light.name, light.partic);

				 // number k means how many light we shall use
				k++;
			}
		}
	}

	if (fclose(fr1) == EOF)
	{
		fprintf(logFile, "Unable to close the file: %s\n", config_file_path);
		return 1;
	}

	if (fclose(fwr1) == EOF)
	{
		fprintf(logFile, "Unable to close the file: results/list_of_used_lights.txt\n");
		return 1;
	}

	int n_orig_exp = setNumN(spectrum);
	#ifdef DEBUGGER_MODE
	printf("n = %d\n", n);
	printf("n_orig = %d\n", n_orig);
	printf("n_orig_exp = %d\n", n_orig_exp);
	#endif

	// INITIALIZATION OF ALL MATRICES AND VECTORS

	if ((matrix_x = init(n, k)) == NULL)
	{
		fprintf(logFile, "\nMemory shortage - matrix_x!\n");
		return 1;
	}
	if ((matrix_orig = init(n_orig, k)) == NULL)
	{
		fprintf(logFile, "\nMemory shortage - matrix_orig!\n");
		return 1;
	}
	if ((matrix_x_t = init(k, n)) == NULL)
	{
		fprintf(logFile, "Memory shortage - matrix_x_t!\n");
		return 1;
	}
	if ((matrix_x_x_t = init(k, k)) == NULL)
	{
		fprintf(logFile, "Memory shortage - matrix_x_t!\n");
		return 1;
	}
	if ((matrix_x_x_t_1 = init(k, k)) == NULL)
	{
		fprintf(logFile, "Memory shortage - matrix_x_x_t_1!\n");
		return 1;
	}
	if ((matrix_m = init(k, n)) == NULL)
	{
		fprintf(logFile, "Memory shortage - matrix_m!\n");
		return 1;
	}
	if ((vector_c = init(k, 1)) == NULL)
	{
		fprintf(logFile, "Memory shortage - vector_c!\n");
		return 1;
	}
	if ((vector_sigma = init(k, 1)) == NULL)
	{
		fprintf(logFile, "Memory shortage - vector_sigma!\n");
		return 1;
	}
	if ((vector_y = init(n, 1)) == NULL)
	{
		fprintf(logFile, "Memory shortage - vector_y!\n");
		return 1;
	}
	if ((vector_orig = init(n_orig_exp, 1)) == NULL)
	{
		fprintf(logFile, "Memory shortage - vector_orig!\n");
		return 1;
	}
	if ((vector_orig_c = init(n_orig, 1)) == NULL)
	{
		fprintf(logFile, "Memory shortage - vector_orig_c!\n");
		return 1;
	}
	if ((vector_xc = init(n, 1)) == NULL)
	{
		fprintf(logFile, "Memory shortage - vector_xc!\n");
		return 1;
	}

	// MEASURED SPECTRUM - must be in interval <min_lambda, max_lambda>
	if (os == 1) {
		sprintf(cutCommand, "cut_exp_spectrum %f %f %s", atof(argv[1]), atof(argv[2]), spectrum_file);
		system(cutCommand);
	}
	else if (os == 2) {
		sprintf(cutCommand, "./cut_exp_spectrum %f %f %s", atof(argv[1]), atof(argv[2]), spectrum_file);
		system(cutCommand);
	}
	else if (os == 3) {	 }
	else { }

	// INTEGRATION OF CORELATED MEASURED SPECTRUM
	// start spectrumavg.exe to corelated spectrum
	if (os == 1) {
		sprintf(command1, "SPECTRUMAVG %.2f %.2f %.2f spectra_cut/%s spectra_integrated/experiment_data.dat", delta_lambda, min_lambda, max_lambda, spectrum_file);
		system(command1);
	}
	else if (os == 2) {
		sprintf(command1, "./spectrumavg %.2f %.2f %.2f spectra_cut/%s spectra_integrated/experiment_data.dat", delta_lambda, min_lambda, max_lambda, spectrum_file);
		system(command1);
	}
	else if (os == 3) {
		// do something
	}
	else {
		// I don't know what to do here
	}


	// INTEGRATED MEASURED SPECTRUM DATA-LOADING INTO VECTOR Y
	// PRINTING WAVELENGTHS INTO SEPARATED FILE (LATER USE)

	if ((fr2 = fopen("spectra_integrated/experiment_data.dat", "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"spectra_integrated/experiment_data.dat\"\n");
		return 1;
	}
	if ((fw4 = fopen("running_files/wavelengths.txt", "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"running_files/wavelengths.txt\"\n");
		return 1;
	}

	// number b means row-number of vector Y
	b = 0;

	// read all lines
	while (fgets(str, MAX, fr2) != NULL)
	{
		// ignore lines with '#' at the beginning
		if (str[0] != '#')
		{
			// load wavelength and intensity to the structure 'light'
			sscanf(str, "%lf%lf", &light.lambda, &light.intensity);

			// write intensity into vector Y
			input_to_matrix(vector_y, n, 1, b, 0, light.intensity);

			// write wavelength in separated file
			fprintf(fw4, "%f\n", light.lambda);

			// increase row-number
			b++;
		}
	}

	// close used files
	if (fclose(fr2) == EOF)
	{
		fprintf(logFile, "Unable to close the file: spectra_integrated/experiment_data.dat\n");
		return 1;
 	}
 	if (fclose(fw4) == EOF)
	{
		fprintf(logFile, "Unable to close the file: running_files/wavelengths.txt\n");
		return 1;
 	}

	// PRINT VECTOR Y
	print_matrix(vector_y, n, 1, "running_files/vector_y.txt");


	// LOAD ORIGINAL EXPERIMENT DATA INTO VECTOR ORIG

	if ((fr2 = fopen(spectrum, "r")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"%s\"\n", spectrum);
	  return 1;
	}
	if ((fw4 = fopen("running_files/wavelengths_orig.txt", "w")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"running_files/wavelengths_orig.txt\"\n");
	  return 1;
	}

	// number b means row-number of vector Y
	bb = 0;

	// read all lines
	while (fgets(str, MAX, fr2) != NULL)
	{
	  // ignore lines with '#' at the beginning
	  if ((str[0] != '#') && (str[0] != 'W'))
	  {
	    // load wavelength and intensity to the structure 'light'
	    sscanf(str, "%lf\t%lf", &light.lambda, &light.intensity);

	    // write intensity into vector Y
	    input_to_matrix(vector_orig, n_orig_exp, 1, bb, 0, light.intensity);

	    // write wavelength in separated file
	    fprintf(fw4, "%f\n", light.lambda);

	    // increase row-number
	    bb++;
	  }
	}

	// close used files
	if (fclose(fr2) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: %s\n", spectrum);
	  return 1;
	}
	if (fclose(fw4) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: running_files/wavelengths_orig.txt\n");
	  return 1;
	}
	// PRINT VECTOR ORIG
	#ifdef DEBUGGER_MODE
	print_matrix(vector_orig, n_orig_exp, 1, "running_files/vector_orig.txt");
	#endif

	// INTEGRATED SPECTRA DATA-LOADING INTO MATRIX X

	if ((fwr1 = fopen("results/list_of_used_lights.txt", "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"results/list_of_used_lights.txt\"\n");
		return 1;
	}

	// column number
	a = 0;

	// read all lines in file
	while (fgets(str, MAX, fwr1) != NULL)
	{
		// ignore lines with '#' at the beginning
		if (str[0] != '#')
		{
			// load name of light and participation to the structure 'light'
		 	sscanf(str, "%s\t%d", light.name, &light.partic);

		 	// create string - full name of file that we are going to open
		 	sprintf(filename, "spectra_integrated/%s.dat", light.name);

		 	// open the file
		 	if ((fr3 = fopen(filename, "r")) == NULL)
			{
				fprintf(logFile, "Unable to open the file: \"%s\"\n", filename);
				return 1;
			}
			// number b means row-number of matrix X
			b = 0;

			// load all lines
		 	while (fgets(str, MAX, fr3) != NULL)
		 	{
		 		// ignore lines with '#' and 'W' at the beginning
				if ((str[0] != '#') && (str[0] != 'W'))
				{
					// load wavelength and intensity of light in the structure light
					sscanf(str, "%lf\t%lf", &light.lambda, &light.intensity);

					// write intensity in the matrix X (size: N x K)
					input_to_matrix(matrix_x, n, k, b, a, light.intensity);

					// increase row-number
					b++;
				}
			}

			// close read file
			if (fclose(fr3) == EOF)
			{
				fprintf(logFile, "Unable to close the file: \"%s\"\n", filename);
				return 1;
			}

			// increase column-number
			a++;
		}
	}

	// close used file
	if (fclose(fwr1) == EOF)
	{
		fprintf(logFile, "Unable to close the file: results/list_of_used_lights.txt\n");
		return 1;
	}

	// PRINT MATRIX X
	#ifdef DEBUGGER_MODE
	print_matrix(matrix_x, n, k, "running_files/matrix_x.txt");
	#endif

	// LOAD ORIGINAL SPECTRA INTO MATRIX ORIG
	if ((fwr1 = fopen("results/list_of_used_lights.txt", "r")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"results/list_of_used_lights.txt\"\n");
	  return 1;
	}

	// column number
	a = 0;

	// read all lines in file
	while (fgets(str, MAX, fwr1) != NULL)
	{
	  // ignore lines with '#' at the beginning
	  if (str[0] != '#')
	  {
	    // load name of light and participation to the structure 'light'
	    sscanf(str, "%s\t%d", light.name, &light.partic);

	    // create string - full name of file that we are going to open
	    sprintf(filename, "spectra_used/%s.txt", light.name);
	    //printf("column = %d\n", a);
	    //printf("file = %s\n", filename);

	    // open the file
	    if ((fr3 = fopen(filename, "r")) == NULL)
	    {
	      fprintf(logFile, "Unable to open the file: \"%s\"\n", filename);
	      return 1;
	    }
		if ((fw4 = fopen("running_files/wavelengths_light.txt", "w")) == NULL)
		{
			fprintf(logFile, "Unable to open the file: \"running_files/wavelengths_light.txt\"\n");
			return 1;
		}
	    // number b means row-number of matrix X
	    bb = 0;

	    // load all lines
	    while (fgets(str, MAX, fr3) != NULL)
	    {
	      // ignore lines with '#' at the beginning
	      if ((str[0] != '#') && (str[0] != 'W'))
	      {
	        // load wavelength and intensity of light in the structure light
	        sscanf(str, "%lf\t%lf", &light.lambda, &light.intensity);

	        // write intensity in the matrix X (size: N x K)
	        input_to_matrix(matrix_orig, n_orig, k, bb, a, light.intensity);
	        //printf("[%d:%d] .... Y[0] = %.8f\n", a, bb, vector_y[0]);

			// write wavelength in separated file
		    fprintf(fw4, "%f\n", light.lambda);

	        // increase row-number
	        bb++;
	      }
	    }

	    // close read file
	    if (fclose(fr3) == EOF)
	    {
	      fprintf(logFile, "Unable to close the file: \"%s\"\n", filename);
	      return 1;
	    }
		if (fclose(fw4) == EOF)
		{
			fprintf(logFile, "Unable to close the file: running_files/wavelengths_light.txt\n");
			return 1;
		}

	    // increase column-number
	    a++;
	  }
	}

	// close used file
	if (fclose(fwr1) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: results/list_of_used_lights.txt\n");
	  return 1;
	}

	// PRINT MATRIX ORIG
	#ifdef DEBUGGER_MODE
	print_matrix(matrix_orig, n_orig, k, "running_files/matrix_orig.txt");
	#endif

	// TRANSPOSE OF MATRIX X AND PUT IT INTO MATRIX X_T

	transpose(matrix_x, matrix_x_t, n, k);

	// PRINT TRANSPOSE OF MATRIX X
	#ifdef DEBUGGER_MODE
	print_matrix(matrix_x_t, k, n, "running_files/matrix_x_t.txt");
	#endif

	// MULTIPLY MATRIX X & TRANSPOSE OF MATRIX X, AND PUT IT INTO MATRIX X_X_T
	for(i = 0; i < k; i++)
	{
		for(j = 0; j < k; j++)
		{
			sum = 0.;
			for(l = 0; l < n; l++)
            {
            	sum += element_of_matrix(matrix_x_t, k, n, i, l)*element_of_matrix(matrix_x, n, k, l, j);
            }
            input_to_matrix(matrix_x_x_t, k, k, i, j, sum);
		}
	}
	#ifdef DEBUGGER_MODE
	fprintf(logFile, "Bez chyb\n");
	#endif

	// PRINT MATRIX X_X_T
	#ifdef DEBUGGER_MODE
	print_matrix(matrix_x_x_t, k, k, "running_files/matrix_x_x_t.txt");
	#endif

	// INVERSION OF MATRIX X_X_T AND PUT IT INTO MATRIX X_X_T_1
	// checking for zero determinant

	double det = invert_matrix(matrix_x_x_t, matrix_x_x_t_1, k);

	#ifdef DEBUGGER_MODE
	printf("Determinant = %.15f\n", det);
	#endif

	// PRINT MATRIX X_X_T_1
	#ifdef DEBUGGER_MODE
	print_matrix(matrix_x_x_t_1, k, k, "running_files/matrix_x_x_t_1.txt");
	#endif

	// MULTIPLY MATRICES X_X_T_1 AND X_T, AND PUT IT INTO MATRIX M
	for(i = 0; i < k; i++)
	{
		for(j = 0; j < n; j++)
		{
			sum = 0.;
			for(l = 0; l < k; l++)
            {
            	sum += element_of_matrix(matrix_x_x_t_1, k, k, i, l)*element_of_matrix(matrix_x_t, k, n, l, j);
            }
            input_to_matrix(matrix_m, k, n, i, j, sum);
		}
	}

	// PRINT MATRIX M
	#ifdef DEBUGGER_MODE
	print_matrix(matrix_m, k, n, "running_files/matrix_m.txt");
	#endif

	// MULTIPLY MATRIX M AND VECTOR Y, AND PUT IT INTO VECTOR C
	for (i = 0; i < k; i++)
	{
		sum = 0.;
		for (j = 0; j < n; j++)
		{
			sum += element_of_matrix(matrix_m, k, n, i, j)*element_of_matrix(vector_y, n, 1, j, 0);

		}
		input_to_matrix(vector_c, k, 1, i, 0, sum);
	}

	#ifdef DEBUGGER_MODE
	fprintf(logFile, "Bez chyb\n");
	#endif

	// PRINT VECTOR C
	#ifdef DEBUGGER_MODE
	print_matrix(vector_c, k, 1, "running_files/vector_c.txt");
	#endif

	// MULTIPLY MATRIX X AND VECTOR C, AND PUT IT INTO VECTOR XC
	for (i = 0; i < n; i++)
	{
		sum = 0.;
		for (j = 0; j < k; j++)
		{
			sum += element_of_matrix(matrix_x, n, k, i, j)*element_of_matrix(vector_c, k, 1, j, 0);
		}
		input_to_matrix(vector_xc, n, 1, i, 0, sum);
	}

	// PRINT VECTOR XC
	print_matrix(vector_xc, n, 1, "running_files/vector_xc.txt");


	// MULTIPLY MATRIX ORIG AND VECTOR C, AND PUT IT INTO VECTOR ORIG_C

	for (i = 0; i < n_orig; i++)
	{
		sum = 0.;
		for (j = 0; j < k; j++)
		{
			sum += element_of_matrix(matrix_orig, n_orig, k, i, j)*element_of_matrix(vector_c, k, 1, j, 0);
		}
		input_to_matrix(vector_orig_c, n_orig, 1, i, 0, sum);
	}

	// PRINT VECTOR ORIG_C
	print_matrix(vector_orig_c, n_orig, 1, "running_files/vector_orig_c.txt");

	// CREATE VECTOR Y-XC
	#ifdef DEBUGGER_MODE
	FILE 	*fw10;
	char	fileName[100];
	sprintf(fileName, "running_files/vectorY-XC_%.2f.txt", delta_lambda);
	printf("%s\n", fileName);

	if ((fw10 = fopen(fileName, "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"%s\"\n", fileName);
		return 1;
	}


	for (b = 0; b < n; b++)
	{
		fprintf(fw10, "%.8f\n", element_of_matrix(vector_y, n, 1, b, 0) - element_of_matrix(vector_xc, n, 1, b, 0));
		printf("%.8f\t%.8f\n", element_of_matrix(vector_y, n, 1, b, 0),
		element_of_matrix(vector_xc, n, 1, b, 0));

	}

	if (fclose(fw10) == EOF)
	{
		fprintf(logFile, "Unable to close the file: \"%s\"\n", fileName);
		return 1;
 	}
	#endif

	// CALCULATING OF DEVIATION FOR EACH COEFICIENT
	// PUT IT INTO VECTOR SIGMA

	sigma2 = 0.;
	sigma = 0.;
	sum = 0.;
	for (i = 0; i < n; i++)
	{
		pom = 0.;
		for (j = 0; j < k; j++)
		{
			pom += element_of_matrix(matrix_x, n, k, i, j)*element_of_matrix(vector_c, k, 1, j, 0);
		}

		sum += pow(element_of_matrix(vector_y, n, 1, i, 0) - pom, 2) /*vahy tu maju byt*/;
	}
	sigma2 = sum/(n-k);
	sigma = sqrt(sigma2);

	for (i = 0; i < k; i++)
	{
		input_to_matrix(vector_sigma, k, 1, i, 0, sqrt(fabs(sigma2*element_of_matrix(matrix_x_x_t_1, k, k, i, i))));
	}

	// PRINT VECTOR SIGMA
	#ifdef DEBUGGER_MODE
	print_matrix(vector_sigma, k, 1, "running_files/vector_odchylok.txt");
	#endif

	// CALCULATE AIC COEFICIENT and WRITE IT INTO FILE
	// from Akaike information criterion
	double aic = n*log(sigma2) + 2*k;

	if ((fw9 = fopen("running_files/aic.txt", "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: running_files/aic.txt\n");
		return 1;
	}

	fprintf(fw9, "%f", aic);

	if (fclose(fw9) == EOF)
	{
		fprintf(logFile, "Unable to close the file: running_files/aic.txt\n");
		return 1;
	}


	// CREATING A FILE WITH COEFICIENTS (AND DEVIATONS) FOR EACH USED LIGHT

	sprintf(filename, "results/coeficients.txt");
	if ((fr1 = fopen(filename, "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"%s\"\n", filename);
		return 1;
	}

	if ((fwr1 = fopen("results/list_of_used_lights.txt", "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"results/list_of_used_lights.txt\"\n");
		return 1;
	}

	a = 0;
	// read all lines
	while (fgets(str, MAX, fwr1) != NULL)
	{
		// ignore lines with '#' at the beginning
		if (str[0] != '#')
		{
			// load name and participation of light into structure 'light'
		 	sscanf(str, "%s\t%d", light.name, &light.partic);

		 	// write name of light, coeficient and deviation
		 	fprintf(fr1, "%s\t%.11f\t%.11f\n", light.name, element_of_matrix(vector_c, k, 1, a, 0), element_of_matrix(vector_sigma, k, 1, a, 0));

		 	// sum up all the coeficients
		 	sumOfCoeficients += element_of_matrix(vector_c, k, 1, a, 0);
		}
		a++;
	}

	// close used files
	if (fclose(fr1) == EOF)
	{
		fprintf(logFile, "Unable to close the file: results/coeficients.txt\n");
		return 1;
	}

	if (fclose(fwr1) == EOF)
	{
		fprintf(logFile, "Unable to close the file: results/list_of_used_lights.txt\n");
		return 1;
	}

	// CREATING A FILE WITH PERCENTAGE (AND DEVIATONS) FOR EACH USED LIGHT

	sprintf(filename, "results/percentage.txt");
	if ((fr1 = fopen(filename, "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"%s\"\n", filename);
		return 1;
	}

	if ((fwr1 = fopen("results/list_of_used_lights.txt", "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"results/list_of_used_lights.txt\"\n");
		return 1;
	}

	a = 0;
	// read all lines
	while (fgets(str, MAX, fwr1) != NULL)
	{
		// ignore lines with '#' at the beginning
		if (str[0] != '#')
		{
			// load name and participation of light into structure 'light'
		 	sscanf(str, "%s\t%d", light.name, &light.partic);

		 	double minimum = 100*element_of_matrix(vector_c, k, 1, a, 0)/sumOfCoeficients - 100*element_of_matrix(vector_sigma, k, 1, a, 0)/sumOfCoeficients;
		 	double maximum = 100*element_of_matrix(vector_c, k, 1, a, 0)/sumOfCoeficients + 100*element_of_matrix(vector_sigma, k, 1, a, 0)/sumOfCoeficients;
		 	double middle = 100*element_of_matrix(vector_c, k, 1, a, 0)/sumOfCoeficients;
		 	// write name of light, minimum, middle and maximum precentage
		 	fprintf(fr1, "%s\t%.3f\t%.3f\t%.3f\n", light.name, minimum, middle, maximum);
		}
		a++;
	}

	// close used file
	if (fclose(fr1) == EOF)
	{
		fprintf(logFile, "Unable to close the file: %s\n", filename);
		return 1;
	}

	if (fclose(fwr1) == EOF)
	{
		fprintf(logFile, "Unable to close the file: results/list_of_used_lights.txt\n");
		return 1;
	}

	// PRINT DELTA_LAMBDA & SIGMA IN LOG FILE
	fprintf(logFile, "Database:\t%s\n", config_file);
	fprintf(logFile, "Determinant:\t%.15f\n", det);
	fprintf(logFile, "Lambda Min:\t%.0f\n", min_lambda);
	fprintf(logFile, "Lambda Max:\t%.0f\n", max_lambda);
	fprintf(logFile, "Delta Lambda:\t%.1f\n", delta_lambda);
	fprintf(logFile, "Std. Deviation:\t%.8f\n", sigma);
	fprintf(logFile, "Akaike Coef:\t%.8f\n", aic);

	// PRINT DELTA_LAMBDA and SIGMA OF CALCULATION INTO file: sigma.txt
	if ((fw8 = fopen("running_files/sigma.txt", "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"running_files/sigma.txt\"\n");
		return 1;
	}

	fprintf(fw8, "%f\t%f", delta_lambda, sigma);

	if (fclose(fw8) == EOF)
	{
		fprintf(logFile, "Unable to close the file: running_files/sigma.txt\n");
		return 1;
 	}

	// CREATE A FILE WITH THREE COLUMN
	// WAVELENGTH	|	MEASURED VALUE	|	CALCULATED VALUE

	if ((fr4 = fopen("running_files/wavelengths.txt", "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"running_files/wavelengths.txt\"\n");
		return 1;
	}
	if ((fr5 = fopen("running_files/vector_y.txt", "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"running_files/vector_y.txt\"\n");
		return 1;
	}
	if ((fr6 = fopen("running_files/vector_xc.txt", "r")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"running_files/vector_xc.txt\"\n");
		return 1;
	}
	if ((fw7 = fopen("running_files/comparison.txt", "w")) == NULL)
	{
		fprintf(logFile, "Unable to open the file: \"running_files/comparison.txt\"\n");
		return 1;
	}

	// write data into comaprison.txt
	// comaprison.txt will be used for plotting graph
	//fprintf(fw7, "lambda\texperiment\tcalculation\n");
	for (i = 0; i < b; i++) {
		fscanf(fr4, "%lf\n", &graph.lambda);
		fscanf(fr5, "%lf\n", &graph.experiment);
		fscanf(fr6, "%lf\n", &graph.calculation);
		fprintf(fw7, "%f\t%f\t%f\n", graph.lambda, graph.experiment, graph.calculation);
		#ifdef DEBUGGER_MODE
		printf("%f\t%f\t%f\n", graph.lambda, graph.experiment, graph.calculation);
		#endif
	}

	if (fclose(fr4) == EOF)
	{
		fprintf(logFile, "Unable to close the file: running_files/wavelengths.txt\n");
		return 1;
 	}
 	if (fclose(fr5) == EOF)
	{
		fprintf(logFile, "Unable to close the file: running_files/vector_y.txt\n");
		return 1;
 	}
 	if (fclose(fr6) == EOF)
	{
		fprintf(logFile, "Unable to close the file: running_files/vector_xc.txt\n");
		return 1;
 	}
 	if (fclose(fw7) == EOF)
	{
		fprintf(logFile, "Unable to close the file: running_files/comparison.txt\n");
		return 1;
 	}

	// CREATE A FILE WITH four COLUMN WITH ORIGINAL DATA
	// WAVELENGTH_exp	|	MEASURED VALUE	| WAVELENGTH_light |	CALCULATED VALUE

	if ((fr5 = fopen(spectrum, "r")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"%s\"\n", spectrum);
	  return 1;
	}
	if ((fr6 = fopen("running_files/vector_orig_c.txt", "r")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"running_files/vector_orig_c.txt\"\n");
	  return 1;
	}
	if ((fr7 = fopen("running_files/wavelengths_light.txt", "r")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"running_files/wavelengths_light.txt\"\n");
	  return 1;
	}
	if ((fw7 = fopen("running_files/plot_orig_exp.txt", "w")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"running_files/plot_orig_exp.txt\"\n");
	  return 1;
	}
	if ((fw8 = fopen("running_files/plot_orig_calc.txt", "w")) == NULL)
	{
	  fprintf(logFile, "Unable to open the file: \"running_files/plot_orig_calc.txt\"\n");
	  return 1;
	}

	// write data into comaprison_orig.txt
	// comaprison_orig.txt will be used for plotting graph
	// make file with original experiment data

	while (fgets(str, MAX, fr5) != NULL)
	{
		// ignore lines with '#' at the beginning
		if ((str[0] != '#') || (str[0] != 'W'))
		{
			// load name and participation of light into structure 'light'
		 	sscanf(str, "%lf\t%lf\n", &graph.lambda, &graph.experiment);
		 	fprintf(fw7, "%.6f\t%.6f\n", graph.lambda, graph.experiment);
			//printf("%.6f\t%.6f\n", graph.lambda, graph.experiment);
		}
	}

	for (i = 0; i < bb; i++) {
		fgets(str1, MAX, fr7);
		fgets(str2, MAX, fr6);
		sscanf(str1, "%lf\n", &graph.lambda_light);
	  sscanf(str2, "%lf\n", &graph.calculation);
		fprintf(fw8, "%f\t%f\n", graph.lambda_light, graph.calculation);
	}


	if (fclose(fr5) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: %s\n", spectrum);
	  return 1;
	}
	if (fclose(fr6) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: running_files/vector_orig_c.txt\n");
	  return 1;
	}
	if (fclose(fr7) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: running_files/wavelengths_light.txt\n");
	  return 1;
	}
	if (fclose(fw7) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: running_files/plot_orig_exp.txt\n");
	  return 1;
	}
	if (fclose(fw8) == EOF)
	{
	  fprintf(logFile, "Unable to close the file: running_files/plot_orig_calc.txt\n");
	  return 1;
	}


 	// find absolute path to this program
 	char cwd[100000];
 	char path_name[100000];
	char path_name_orig_exp[100000];
	char path_name_orig_calc[100000];
	getcwd(cwd, sizeof(cwd));
	sprintf(path_name, "%s/running_files/comparison.txt", cwd);
	sprintf(path_name_orig_exp, "%s/running_files/plot_orig_exp.txt", cwd);
	sprintf(path_name_orig_calc, "%s/running_files/plot_orig_calc.txt", cwd);

	// fprintf(logFile, "Comparison: %s\n", path_name);

 	// SAVE GRAPH TO PNG FILE
 	if (os == 1) {
		sprintf(command2,
	 		" gnuplot -persist -e "
			" \"set terminal postscript enhanced;"
			" set xlabel 'wavelength [nm]'; "
	 		" set key at graph 0.955,0.955; "
	 		" set label 1 'step = %.1f nm' at graph 0.055,0.95; "
	 		" set label 2 '{sigma} = %.6f' at graph 0.055,0.91; "
			" set grid; "
			" set terminal png size 683,768; "
	 		" set output 'results/graph.png'; "
			" set title 'Comparison of original and calculated spectra (integrated)'; "
			" set xrange[%f:%f]; "
			" plot"
			" '%s'"
			" using 1:3 title 'calculation' with boxes fs solid 0.10 lc 8, "
	 		" '%s'"
			" using 1:2 title 'experiment' pt 11 ps 1 lc 8\"", delta_lambda, sigma, min_lambda, max_lambda, path_name, path_name);
		system(command2);
	}
	else if (os == 2) {
		sprintf(command2,
	 		" gnuplot -persist -e "
			" \"set terminal postscript enhanced; "
			" set xlabel '{/Symbol l} [nm]' enhanced; "
	 		" set key at graph 0.955,0.955; "
	 		" set label 1 '{/Symbol D}{/Symbol l} = %.1f nm' at graph 0.055,0.95 enhanced; "
	 		" set label 2 '{/Symbol s} = %.6f' at graph 0.055,0.91 enhanced; "
			" set grid; "
			" set terminal png size 683,768; "
	 		" set output 'results/graph.png'; "
			" set title 'Comparison of original and calculated spectra (integrated)'; "
			" set xrange[%f:%f]; "
			" plot"
			" '%s'"
			" using 1:3 title 'calculation' with boxes fs solid 0.10 lc 8, "
	 		" '%s'"
			" using 1:2 title 'experiment' pt 11 ps 1 lc 8\"", delta_lambda, sigma, min_lambda, max_lambda, path_name, path_name);
		system(command2);
	}
	else if (os == 3) {
		// do something
	}
	else {
		// unknown
	}

	// SAVE GRAPH of ORIGINAL DATA TO PNG FILE
 	if (os == 1) {
		sprintf(command3,
	 		" gnuplot -persist -e "
			" \"set terminal postscript enhanced;"
			" set xlabel 'wavelength [nm]'; "
	 		" set key at graph 0.955,0.955; "
	 		" set label 1 'step = %.1f nm' at graph 0.055,0.95; "
	 		" set label 2 '{sigma} = %.6f' at graph 0.055,0.91; "
			" set grid; "
			" set terminal png size 683,768; "
	 		" set output 'results/graph_original.png'; "
			" set title 'Comparison of original and calculated spectra (NON-integrated)'; "
			" set xrange[%f:%f]; "
			" plot"
			" '%s'"
			" using 1:2 title 'calculation' with lines lt -1 lc 8, "
	 		" '%s'"
			" using 1:2 title 'experiment' pt 7 ps 1 lc 8\"",
			delta_lambda, sigma, min_lambda, max_lambda, path_name_orig_calc, path_name_orig_exp);
		system(command3);
	}
	else if (os == 2) {
		sprintf(command3,
	 		" gnuplot -persist -e "
			" \"set terminal postscript enhanced; "
			" set xlabel '{/Symbol l} [nm]' enhanced; "
	 		" set key at graph 0.955,0.955; "
	 		" set label 1 '{/Symbol D}{/Symbol l} = %.1f nm' at graph 0.055,0.95 enhanced; "
	 		" set label 2 '{/Symbol s} = %.6f' at graph 0.055,0.91 enhanced; "
			" set grid; "
			" set terminal png size 683,768; "
	 		" set output 'results/graph_original.png'; "
			" set title 'Comparison of original and calculated spectra (NON-integrated)'; "
			" set xrange[%f:%f]; "
			" plot"
			" '%s'"
			" using 1:2 title 'calculation' with lines lt -1 lc 8, "
	 		" '%s'"
			" using 1:2 title 'experiment' pt 7 ps 1 lc 8\"",
			delta_lambda, sigma, min_lambda, max_lambda, path_name_orig_calc, path_name_orig_exp);
		system(command3);
	}
	else if (os == 3) {
		// do something
	}
	else {
		// unknown
	}


	// CLOSE LOG FILE
	fprintf(logFile, "*****************************************\n");

	if (fclose(logFile) == EOF)
	{
		#ifdef DEBUGGER_MODE
		printf("LOG FILE COULD NOT BE CLOSED !");
		#endif

		return 1;
	}
	else
	{
		#ifdef DEBUGGER_MODE
		printf("LOG FILE CLOSED\n");
		#endif
	}

	//system("Pause");
	return 0;
}

// definitions of global functions

///////////////////////////////////////////////////////////////////////////////////////////////
void 	print_matrix(double *matrix, int rows, int cols, char file[30])
{
	/*
	 * This function prints matrix (size: rows x cols) in 'file'
	 * 		-> *matrix is pointer on the beginning of the matrix
	 */


	FILE	*fw;
	int		i, j;

	if ((fw = fopen(file, "w")) == NULL)
	{
		printf("\nUnable to open the file: \"%s\"\n", file);
	}

	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			fprintf(fw, "%f\t", element_of_matrix(matrix, rows, cols, i, j));
		}
		fputc('\n', fw);
	}

	if (fclose(fw) == EOF)
	{
		printf("\nUnable to close the file: \"%s\"\n", file);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void 	input_to_matrix(double *matrix, int rows, int cols, int row_act, int col_act, double input)
{
	/*
	 * This function puts a number 'input' in matrix (size: rows x cols) at position (row_act x col_act)
	 * 		-> *matrix is pointer on the beginning of the matrix
	 */

	*(matrix + cols*row_act + col_act) = input;
}

///////////////////////////////////////////////////////////////////////////////////////////////
double 	element_of_matrix(double *matrix, int rows, int cols, int row_act, int col_act)
{
	/*
	 * This function returns element of the matrix (size: rows x cols) at position (row_act x col_act)
	 * 		-> *matrix is pointer on the beginning of the matrix
	 */

	return (*(matrix + cols*row_act + col_act));
}

///////////////////////////////////////////////////////////////////////////////////////////////
double 	*init(int rows, int cols)
{
	/*
	 * This function returns pointer at the beginning of area in memory (size: rows x cols)
	 */

	//return ((double*) malloc((rows+1)*(cols+1)*sizeof(double)));
	return ((double*) malloc((rows)*(cols)*sizeof(double)));
}

///////////////////////////////////////////////////////////////////////////////////////////////
void	transpose(double *matrix_origin, double *matrix_transponed, int rows, int cols)
{
	/*
	 * This function transposes matrix (size: rows x cols)
	 * 		to another matrix (size: cols x rows).
	 *
	 *		-> *matrix_origin is pointer on the beginning of the original matrix
	 *		-> *matrix_transponed is pointer on the beginning of the new matrix
	 */

	int	i, j;

	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			input_to_matrix(matrix_transponed, cols, rows, j, i, element_of_matrix(matrix_origin, rows, cols, i, j));
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
double 	invert_matrix(double *matrix, double *matrix_i, int size)
{
	/*
	 * This function inverts matrix (size: size x size)
	 * 		to another matrix (size: size x size).
	 *
	 *		-> *matrix is pointer on the beginning of the original matrix
	 *		-> *matrix_i is pointer on the beginning of the new matrix
	 */

	int 	i, j, iPass, imx, icol, irow;
	double 	det, temp, temp2, temp3, temp4, temp5, pivot, factor;

	det = 1.;

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < size; j++)
		{
			input_to_matrix(matrix_i, size, size, i, j, 0.);
		}
		input_to_matrix(matrix_i, size, size, i, i, 1.);
	}

	// Aktualmnym pivotom riadku je iPass.
	// Pre kazdy prechod - najprv najst najvacsi element v stlpci pivota.
	for (iPass = 0; iPass < size; iPass++)
	{
		imx = iPass;

		for (irow = iPass; irow < size; irow++)
		{
			if (fabs(element_of_matrix(matrix, size, size, irow, iPass)) > fabs(element_of_matrix(matrix, size, size, imx, iPass))) imx = irow;

		}

			// Vymenit prvky riadku iPass a riadku imx v A aj AInverse.
			// Zmenit znamienko determinantu na opacne pri zamene riadkov

		if (imx != iPass)
		{
			det *= -1.;
			for (icol = 0; icol < size; icol++)
			{
				temp = element_of_matrix(matrix_i, size, size, iPass, icol);
				input_to_matrix(matrix_i, size, size, iPass, icol, element_of_matrix(matrix_i, size, size, imx, icol));
				input_to_matrix(matrix_i, size, size, imx, icol, temp);

				if (icol >= iPass)
				{
					temp = element_of_matrix(matrix, size, size, iPass, icol);
					input_to_matrix(matrix, size, size, iPass, icol, element_of_matrix(matrix, size, size, imx, icol));
					input_to_matrix(matrix, size, size, imx, icol, temp);

				}
			}
		}

			// Aktualnym pivotom je teraz A[iPass][iPass].
			// Determinant je sucinom pivotov.

		pivot = element_of_matrix(matrix, size, size, iPass, iPass);
		det = det * pivot;

		for (icol = 0; icol < size; icol++)
		{
				// Normalizovat riadok pivotu delenim hodnotou pivotu.
			temp2 = element_of_matrix(matrix_i, size, size, iPass, icol);
			input_to_matrix(matrix_i, size, size, iPass, icol, temp2/pivot);

			if (icol >= iPass)
			{
				temp2 = element_of_matrix(matrix, size, size, iPass, icol);
				input_to_matrix(matrix, size, size, iPass, icol, temp2/pivot);
			}
		}

		for (irow = 0; irow < size; irow++)
			// Pricitat nasobok riadku pivotu ku kazdemu riadku.
			// Nasobiaci koef. sa zvoli tak, ze prvok A v stlpci pivota je 0.
		{
			if (irow != iPass) factor = element_of_matrix(matrix, size, size, irow, iPass);

			for (icol = 0; icol < size; icol++)
			{
				if (irow != iPass)
				{
					temp2 = element_of_matrix(matrix_i, size, size, iPass, icol);
					temp3 = element_of_matrix(matrix_i, size, size, irow, icol);
					input_to_matrix(matrix_i, size, size, irow, icol, temp3 - factor*temp2);
					temp4 = element_of_matrix(matrix, size, size, iPass, icol);
					temp5 = element_of_matrix(matrix, size, size, irow, icol);
					input_to_matrix(matrix, size, size, irow, icol, temp5 - factor*temp4);
				}
			}
		}
	}
	return det;
}

///////////////////////////////////////////////////////////////////////////////////////////////
int 	setNumN(char adress[200])
{
	/*
	 * This function returns number of rows with wavelength in file 'address'
	 */

	char 	line[100];
	FILE 	*fr;
	int		rows = 0;

	if ((fr = fopen(adress, "r")) == NULL)
	{
		printf("\nUnable to open the file: \"%s\"\n", adress);
		return 1;
	}

	// read all lines
	while (fgets(line, 100, fr) != NULL) {
		if ((line[0] == '#') || (line[0] == 'W'))
		{
			// ignore lines with '#' at the beginning
			continue;
		}
		else
		{
			// increase row-number
			rows++;
		}
	}
	return rows;
}
