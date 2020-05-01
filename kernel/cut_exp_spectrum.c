/*
	=================================================================
	Name: Cut_exp_spectrum program
	Author: Ondrej Bily
	Description: This program takes experimental spectrum and cuts
				them to interval (min_lambda, max_lambda)
	Date of latest update: 23.06.2019

	Compile:	gcc -Wall cut_exp_spectrum.c -o cut_exp_spectrum
	Run:		./cut_exp_spectrum <MIN> <MAX> <SPECTRUM>
	================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

/* This is just a sample code, modify it to meet your need */
int main(int argc, char **argv)
{
    // detect command line paramaters
    double 	min = atof(argv[1]);
    double 	max = atof(argv[2]);
    char	spectrum[100];
	strcpy(spectrum, argv[3]);

	// find absolute path to this program
 	char cwd[100000];
 	char path_name[100000];
	getcwd(cwd, sizeof(cwd));
	sprintf(path_name, "%s/input_data", cwd);
	printf("PATH to SPECTRUM: %s\n", path_name);

	// LOG FILE
	char file_name[100000];
	sprintf(file_name, "%s/logFileCutSpectra.txt", cwd);

    FILE    *log_file;
    FILE    *entry_file;
    FILE	*new_file;
    char    buffer[BUFSIZ];

    /* Opening log file for writing */
    log_file = fopen(file_name, "w");
    if (log_file == NULL)
    {
        fprintf(log_file, "Error : Failed to open log_file - %s\n", strerror(errno));

        return 1;
    }

	
	/* Open old spectrum */
	char entry_file_name[100000];
	sprintf(entry_file_name, "%s/input_data/%s", cwd, spectrum);
	entry_file = fopen(entry_file_name, "r");
	fprintf(log_file, "%s\n", entry_file_name);

	if (entry_file == NULL)
	{
	    fprintf(log_file, "\tError : Failed to open entry file - %s\n", strerror(errno));
	    fclose(log_file);

	    return 1;
	}

    // open new spectrum
    char new_file_name[100000];
	sprintf(new_file_name, "%s/spectra_cut/%s", cwd, spectrum);
	new_file = fopen(new_file_name, "w");
	fprintf(log_file, "%s\n\n", new_file_name);

    if (new_file == NULL)
	{
	    fprintf(log_file, "\tError : Failed to open NEW file - %s\n", strerror(errno));
	    fclose(log_file);

	    return 1;
	}

	// variable for keeping data
	double lambda, intensity;

	// read all lines in file
	while (fgets(buffer, BUFSIZ, entry_file) != NULL)
	{
		// read all line and sum all intensities
		if ((buffer[0] != '#') && (buffer[0] != 'W') )
		{
            // scan line in entry file
		    sscanf(buffer, "%lf\t%lf\n", &lambda, &intensity);

            // copy only lines with lambda in corerct interval
            if ((min <= lambda) && (lambda <= max)) {
                fprintf(new_file, "%s", buffer);
            }
		}
        else {
            fprintf(new_file, "%s", buffer);
        }
	}

	// close entry and new file
	fclose(entry_file);
    fclose(new_file);

    /* Don't forget to close log file before leaving */
    fclose(log_file);

    return 0;
}
