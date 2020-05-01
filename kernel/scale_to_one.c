/*
	=================================================================
	Name: Scale-to-one program
	Author: Ondrej Bily
	Description: This program takes spectra and scales them to have
				 an area equal to 1
	Date of latest update: 12.12.2018

	Compile:	gcc -Wall scale_to_one.c -o scale_to_one
	Run:		./scale_to_one
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
	// find absolute path to this program
 	char cwd[100000];
 	char path_name[100000];
	getcwd(cwd, sizeof(cwd));
	sprintf(path_name, "%s/spectra_cut", cwd);

	//printf("PATH to DIRECTORY: %s\n", path_name);

	// log file
	char file_name[100000];
	sprintf(file_name, "%s/logFileScaleToOne.txt", cwd);

    DIR* FD;
    struct dirent* in_file;
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

		/* Scanning the in directory */
		if (NULL == (FD = opendir (path_name)))
		{
		    fprintf(log_file, "Error : Failed to open input directory - %s\n", strerror(errno));
		    fclose(log_file);

		    return 1;
		}

			while ((in_file = readdir(FD)))
			{
				// I have one file of directory
				if (!strcmp (in_file->d_name, "."))
				    continue;
				if (!strcmp (in_file->d_name, ".."))
				    continue;

				/* Open entry file for common operation */
				char entry_file_name[100000];
				sprintf(entry_file_name, "spectra_cut/%s", in_file->d_name);
				entry_file = fopen(entry_file_name, "r");
				fprintf(log_file, "%s\n", entry_file_name);

				if (entry_file == NULL)
				{
				    fprintf(log_file, "\tError : Failed to open entry file - %s\n", strerror(errno));
				    fclose(log_file);

				    return 1;
				}

					// variable for trapesoidal method
					double sum = 0.;
					double lambda_cur, intensity_cur;
					double lambda_prev, intensity_prev;
					int i = 0;



					// read all lines in file
					while (fgets(buffer, BUFSIZ, entry_file) != NULL)
					{
						// read all line and sum all intensities
						if ((buffer[0] != '#') && (buffer[0] != 'W') )
						{
						    if (i == 0)  { // skip first line of data

						        // identify lambda and intensity in each line = buffer
						        sscanf(buffer, "%lf\t%lf\n", &lambda_prev, &intensity_prev);
						        i++;
						    }
						    else {
						        // identify lambda and intensity in each line = buffer
								sscanf(buffer, "%lf\t%lf\n", &lambda_cur, &intensity_cur);

						        sum += 0.5*(intensity_cur + intensity_prev)*(lambda_cur - lambda_prev);

						        lambda_prev = lambda_cur;
						        intensity_prev = intensity_cur;
						    }

						}
					}
					fprintf(log_file, "Integral: %f\n", sum);

				// close entry file
				fclose(entry_file);

				// open the same file again and open the new file with the same name in different folder
				entry_file = fopen(entry_file_name, "r");
				if (entry_file == NULL)
				{
				    fprintf(log_file, "\tError : Failed to open entry file - %s\n", strerror(errno));
				    fclose(log_file);

				    return 1;
				}

				char new_file_name[100000];
				sprintf(new_file_name, "spectra_used/%s", in_file->d_name);
				new_file = fopen(new_file_name, "w");

				fprintf(log_file, "%s\n\n", new_file_name);

				if (new_file == NULL)
				{
				    fprintf(log_file, "\tError : Failed to open NEW file - %s\n", strerror(errno));
				    fclose(log_file);

				    return 1;
				}

					// read all lines in file and write a NEW number in a new file
					while (fgets(buffer, BUFSIZ, entry_file) != NULL)
					{
						// read all line and sum all intensities
						if ((buffer[0] != '#') && (buffer[0] != 'W') )
						{
							// identify lambda and intensity in each line = buffer
							sscanf(buffer, "%lf\t%lf\n", &lambda_cur, &intensity_cur);

							fprintf(new_file, "%f\t%f\n", lambda_cur, 100*intensity_cur/sum);
						}
						else {
							fprintf(new_file, "%s", buffer);
						}
					}

				// close entry file and new file
				fclose(entry_file);
				fclose(new_file);
			}

    /* Don't forget to close log file before leaving */
    fclose(log_file);

    return 0;
}
