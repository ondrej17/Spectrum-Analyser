/*
	==============================================================================================
	Name: hard_work.c
	Author: Ondrej Bily
	Description: Program pre vypocet zloziek svetelneho znecistenia hrubou silou.
	Date of latest update: 25.6.2019
	==============================================================================================


	------------------------- PROGRAM hard_work -------------------------------------------------------
	Compile:   gcc -Wall hard_work.c -o hard_work -lm
	Run:       ./hard_work <min_lambda> <max_lambda> <delta_lambda> <file_name> <spectrum>
	Example:   ./hard_work 330 770 23 list_of_lights.txt experiment.txt

	Time:

	time ./hard_work 400 700 19 list_of_lights_custom.txt bratislava_2010.txt ; echo Process returned $?
	==============================================================================================
*/

// system header files
#include 	<math.h>
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>
#include 	<unistd.h>
#include 	<ctype.h>

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

void runSmogRun(double max_lambda, double min_lambda, double delta_lambda,
                char *config_file, char *spectrum);
char *decimal_to_binary(int n, int number);
int getNumOfLights(char *config_file);
void makeConfigFile(char *config_file, char *p_binary);
double checkAic();
void makeOptimalConfigFile(char *config_file, char *binary_optimum);
int checkCoef();
void copyOptimalToOld(char *config_file);

int 	main(int argc, char *argv[])
{
    // checking for correct command from terminal
	if(argc != 6)
    {
        printf("--------------------------------------------------------------------------------\n");
    	printf("                                Program HARD_WORK\n");
    	printf("--------------------------------------------------------------------------------\n");
        printf("Run:	 ./hard_work <min_lambda> <max_lambda> <delta_lambda> <file_name> <spectrum>\n");
        printf("Example: ./hard_work 330 770 23 list_of_lights.txt experiment.txt\n\n");
        return 1;
    }

    // flush the stdout immediately
    setbuf(stdout, NULL);

    // LOG FILE
	FILE   *logFile;

    // OPEN LOG FILE
	if ((logFile = fopen("logFileHardWork.txt", "w")) == NULL) {
		printf("LOG FILE COULD NOT BE OPENED !\n");
		return 1;
	}
	else {
		fprintf(logFile, "HARD_WORK:\n");
		//printf("LOG FILE CREATED\n");
		setbuf(logFile, NULL);
	}

    // variables loaded from start-command
	double	min_lambda = atof(argv[1]);		// minimal wavelength (start of integration)
	double	max_lambda = atof(argv[2]);		// maximal wavelength (end of integration)
	double	delta_lambda = atof(argv[3]);	// step of integration of spectra
	char	config_file[100];
	sprintf(config_file, "%s", argv[4]);
	char 	spectrum[200];
	sprintf(spectrum, "%s", argv[5]);
    char	new_config_file[200];
	sprintf(new_config_file, "new_%s", config_file);

	// run smog for different configuration of light, remember the one with the smallest sigma

    // number of lights in config_file
    int numOfLights = getNumOfLights(config_file);
    //printf("numOfLights: %d\n", numOfLights);
    // starting with number 2^n - 1
    int count = ((int) pow(2, numOfLights)) - 1;
    //printf("Count: %d\n", count);

    // pointer to binary number
    char *p_binary;

	// pointer to optimal binary number
	char binary_optimum[100];

	// aic - Akaike Information Coeficient
	double aic_best = 100000.;
	double aic_current = 0.;

	// is there a negative coeficient?
	int is_negative_coef;

    // run throuch every combination
    while (count > 0) {
        // create binary number
        p_binary = decimal_to_binary(numOfLights, count);

        // create config_file with participation given by binary number
        makeConfigFile(config_file, p_binary);

        // Run, Smog, run!
        runSmogRun(min_lambda, max_lambda, delta_lambda, new_config_file, spectrum);

        // now, we need to check results
		aic_current = checkAic();
		is_negative_coef = checkCoef();
		printf("AIC_current: %f\n", aic_current);

		if ((aic_best > aic_current) && (is_negative_coef == 0)) {
			// remember best sigma
			aic_best = aic_current;
			// remember optimum binary number
			sprintf(binary_optimum, "%s", p_binary);
			printf("AIC: %f\n OPT: %s\n", aic_best, binary_optimum);
		}
        count--;
    }

	// now we have a string, binary_optimum, with our optimal configuration
	// we need to remake config_file with setup given by binary_optimum
	makeOptimalConfigFile(config_file, binary_optimum);

	putchar('\n');

	// delete new_config_file
	char new_config_file_new[200];

	char delCommand[100];
	if (os == 1) {
		sprintf(new_config_file_new, "input_data\\%s", new_config_file);
		sprintf(delCommand, "del /f \"%s\"", new_config_file_new);
		system(delCommand);
		printf("New Config File deleted...\n");
	}
	else if (os == 2) {
		sprintf(new_config_file_new, "input_data/%s", new_config_file);
		sprintf(delCommand, "rm %s", new_config_file_new);
		system(delCommand);
	}
	else if (os == 3) {	/* do something*/ }
	else { /* I don't know what to do here*/ }

	// copy content from optimal config file to to old config file
	copyOptimalToOld(config_file);
	printf("Optimal Config File copied to Old Config File ..\n");

    // at the end, run smog again, with the optimal configuration
    runSmogRun(min_lambda, max_lambda, delta_lambda, config_file, spectrum);

    return 0;

}

/***********************************************
         Functions used in main scope
***********************************************/

// runs smog with given parameters
void runSmogRun(double min_lambda, double max_lambda, double delta_lambda,
                char *config_file, char *spectrum)
{
    // command to run smog
    char command[1000];
    if (os == 1) {        // windows
        sprintf(command, "smog %f %f %f %s %s",
                min_lambda, max_lambda, delta_lambda, config_file, spectrum);

        system(command);
    }
	else if (os == 2) {  // linux
        sprintf(command, "./smog %f %f %f %s %s",
                min_lambda, max_lambda, delta_lambda, config_file, spectrum);
        //printf("%s\n", command);
        system(command);
    }
	else if (os == 3) {	/* do something*/ }
	else { /* I don't know what to do here*/ }
}

// changes decimal number 'number' to binary with 'n' cifres
char *decimal_to_binary(int n, int number)
{
   int c, d, count;
   char *pointer;
   //int number = (int) pow(2, n-1);
   count = 0;
   pointer = (char*)malloc(n);

   if (pointer == NULL)
      exit(EXIT_FAILURE);

   for (c = n-1 ; c >= 0 ; c--)
   {
      d = number >> c;

      if (d & 1)
         *(pointer+count) = 1 + '0';
      else
         *(pointer+count) = 0 + '0';

      count++;
   }
   *(pointer+count) = '\0';

   printf("\nBinary string is: %s\n", pointer);

   return  pointer;
}

// finds number of rows in file
int getNumOfLights(char *config_file)
{
	char 	line[100];
	FILE 	*fr;
	int		rows = 0;
    char	old_config_file[200];
	sprintf(old_config_file, "input_data/%s", config_file);
	char name[100];
	int partic;

	if ((fr = fopen(old_config_file, "r")) == NULL)
	{
		printf("\nUnable to open the file: \"%s\"\n", old_config_file);
		return 1;
	}

	// read all lines
	while (fgets(line, 100, fr) != NULL) {
		if ( !(isalpha(line[0])) ) {
			// do nothing
		}
		else {
			// check if there is 1
			sscanf(line, "%s\t%d", name, &partic);

			if (partic == 1) {
				rows++;
			}
		}
	}

	return rows;
}

// creates config_file with 0/1 given by p_binary
void makeConfigFile(char *config_file, char *p_binary)
{
    // load all lights from old config_file to array of strings

    char 	line[100], name[100];
	FILE 	*fr, *fw;
    char	old_config_file[200];
	sprintf(old_config_file, "input_data/%s", config_file);
    //printf("Old: %s\n", old_config_file);
    char	new_config_file[200];
	sprintf(new_config_file, "input_data/new_%s", config_file);
    //printf("New: %s\n", new_config_file);

    if ((fr = fopen(old_config_file, "r")) == NULL)
	{
		printf("Unable to open the file: %s\n", old_config_file);
		//return 1;
	}
    if ((fw = fopen(new_config_file, "w")) == NULL)
	{
		printf("Unable to open the file: \"%s\"\n", new_config_file);
		//return 1;
	}

    int i = 0;
    int number = 0;
    while (fgets(line, 100, fr) != NULL)
    {
		if ( !(isalpha(line[0])) ) {
			// ignore blank line
		}
		else {
			sscanf(line, "%s\t%d", name, &number);
			if (number == 1) {
				fprintf(fw, "%s\t%c\n", name, p_binary[i]);
		        printf("\t%s\t%c\n", name, p_binary[i]);
		        i++;
			}
		}
		// ignore light with 0
    }

    if (fclose(fr) == EOF)
	{
		printf("Unable to close the file: %s\n", old_config_file);
		//return 1;
	}
    if (fclose(fw) == EOF)
	{
		printf("Unable to close the file: %s\n", new_config_file);
		//return 1;
	}

    // make a new file, that is named exactly same, and put there lights and their 1s or 0s
    // new file is ready for use!
}

double checkAic()
{
    // checking results
	FILE *fr;
	double aic;
	char str[100];

	if ((fr = fopen("running_files/aic.txt", "r")) == NULL)
	{
		printf("Unable to open the file: running_files/aic.txt\n");
		//return 1;
	}

	// read line in file
	fgets(str, 100, fr);
	// find delta_lambda and sigma
	sscanf(str, "%lf", &aic);

	if (fclose(fr) == EOF)
	{
		printf("Unable to close the file: running_files/aic.txt\n");
		//return 1;
	}
    return aic;
}

// creates optimal_config_file with 0/1 given by binary_optimum
void makeOptimalConfigFile(char *config_file, char *binary_optimum)
{
    // load all lights from old config_file to array of strings

    char 	line[100], name[100];
	FILE 	*fr, *fw;
    char	old_config_file[200];
	sprintf(old_config_file, "input_data/%s", config_file);
    //printf("Old: %s\n", old_config_file);
    char	opt_config_file[200];
	sprintf(opt_config_file, "input_data/optimal_%s", config_file);
    //printf("Opt: %s\n", opt_config_file);

    if ((fr = fopen(old_config_file, "r")) == NULL)
	{
		printf("Unable to open the file: %s\n", old_config_file);
	}
    if ((fw = fopen(opt_config_file, "w")) == NULL)
	{
		printf("Unable to open the file: \"%s\"\n", opt_config_file);
	}

	putchar('\n');
	printf("Optimal Configuration:\n");
    int i = 0;
    int number = 0;
    while (fgets(line, 100, fr) != NULL)
    {
		if ( !(isalpha(line[0])) ) {
			// ignore blank line
		}
		else {
			sscanf(line, "%s\t%d", name, &number);
			if (number == 1) {
				fprintf(fw, "%s\t%c\n", name, binary_optimum[i]);
		        printf("\t%s\t%c\n", name, binary_optimum[i]);
		        i++;
			}
			else {
				fprintf(fw, "%s\t0\n", name);
			}
		}
		// ignore light with 0
    }

    if (fclose(fr) == EOF)
	{
		printf("Unable to close the file: %s\n", old_config_file);
		//return 1;
	}
    if (fclose(fw) == EOF)
	{
		printf("Unable to close the file: %s\n", opt_config_file);
		//return 1;
	}

    // make a opt file, that is named exactly same, and put there lights and their 1s or 0s
    // new file is ready for use!
}

int checkCoef()
{
	char 	line[100], name[100];
	FILE 	*fr;

    if ((fr = fopen("results/coeficients.txt", "r")) == NULL)
	{
		printf("Unable to open the file: results/coeficients.txt\n");
		//return 1;
	}

	putchar('\n');

    double coef = 0;
	double deviation = 0;
    while (fgets(line, 100, fr) != NULL)
    {
		if ( !(isalpha(line[0])) ) {
			// ignore blank line
		}
		else {
			sscanf(line, "%s\t%lf\t%lf", name, &coef, &deviation);
			if (coef < 0.) {
				return 1; // if there is negative coefficient
			}
		}
    }

    if (fclose(fr) == EOF)
	{
		printf("Unable to close the file: results/coeficients.txt\n");
		//return 1;
	}

	return 0; // if there is no such thing as negative coefficient
}


void copyOptimalToOld(char *config_file)
{
	FILE 	*fr, *fw;
	char	old_config_file[200];
	char	optimal_config_file[200];

	if (os == 1) { // windows
		sprintf(old_config_file, "input_data\\%s", config_file);
		sprintf(optimal_config_file, "input_data\\optimal_%s", config_file);
	}
	else if (os == 2) { // linux
		sprintf(old_config_file, "input_data/%s", config_file);
		sprintf(optimal_config_file, "input_data/optimal_%s", config_file);
	}
	else if (os == 3) {	/* do something*/ }
	else { /* I don't know what to do here*/ }

    // open files to read and write
    if ((fr = fopen(optimal_config_file, "r")) == NULL)
	{
		printf("Unable to open the file: %s\n", optimal_config_file);
		//return 1;
	}
    if ((fw = fopen(old_config_file, "w")) == NULL)
	{
		printf("Unable to open the file: %s\n", old_config_file);
		//return 1;
	}

	// copy content from optimal to old
	int c = fgetc(fr);
    while (c != EOF)
    {
        fputc(c, fw);
        c = fgetc(fr);
    }

	// content copied

	if (fclose(fr) == EOF)
	{
		printf("Unable to close the file: %s\n", optimal_config_file);
		//return 1;
	}
    if (fclose(fw) == EOF)
	{
		printf("Unable to close the file: %s\n", old_config_file);
		//return 1;
	}
}
