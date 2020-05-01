/*
	=================================================================
	Name: Cut_spectra program
	Author: Ondrej Bily
	Description: This program takes spectra and cuts them to
                interval (min_lambda, max_lambda)
	Date of latest update: 12.05.2019

	Compile:	gcc -Wall cut_spectra.c -o cut_spectra
	Run:		./cut_spectra <MIN> <MAX>
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
    double min = atof(argv[1]);
    double max = atof(argv[2]);

	// find absolute path to this program
 	char cwd[100000];
 	char path_name[100000];
	getcwd(cwd, sizeof(cwd));
	sprintf(path_name, "%s/spectra_original", cwd);

	//printf("PATH to DIRECTORY: %s\n", path_name);

	// log file
	char file_name[100000];
	sprintf(file_name, "%s/logFileCutSpectra.txt", cwd);

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
				sprintf(entry_file_name, "spectra_original/%s", in_file->d_name);
				entry_file = fopen(entry_file_name, "r");
				fprintf(log_file, "%s\n", entry_file_name);

				if (entry_file == NULL)
				{
				    fprintf(log_file, "\tError : Failed to open entry file - %s\n", strerror(errno));
				    fclose(log_file);

				    return 1;
				}

                // open new file
                char new_file_name[100000];
				sprintf(new_file_name, "spectra_cut/%s", in_file->d_name);
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
			}

    /* Don't forget to close log file before leaving */
    fclose(log_file);

    return 0;
}
