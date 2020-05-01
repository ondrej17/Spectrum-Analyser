# Spectrum-Analyser

Developed by **Ondrej Bily**, Faculty of Mathematics, Physics and Informatics, Comenius University in Bratislava, Slovakia as a part of bachelor thesis.

Spectrum Analyser software was developed for obtaining the emission spectra from the measured spectrum (e. g. of a city). The main computational program was written in `C`, encapsulated in GUI in `Java`. Read in the article "Emission spectra of light-pollution sources determined from the light-scattering spectrometry of the night sky" how the software helps to identify the night sky pollution sources.

## About

Spectrum Analyzer calculates a precentage of the chosen types of lights, e.g. LED, HPS, FL. 
An experimental spectrum must have following form:

```
# lines starting with '#' are ignored
# they are used to describe spectrum (date of measurement, town, ...)
# the data must be sorted by size
# [wavelength]	[intensity]
300.0	0.2569
301.0	0.3654
302.0	1.2569
	  .
	  .
	  .
799.0	0.0268
800.0	0.3265
```

The database of lights is stored in the folder `kernel/spectra_original`.
You can add another light too. In order to use it in Spectrum Analyzer, a name of light (i. e. name of file without extension) must be added into the List of Lights. There are four lists in the folder `/SpectrumAnalyzer_v1.0/kernel/input_data`: Generic, All, One and Custom. You can add the name of light into one of them and add 0 or 1 after it:

```
CFL_Compact_Fluorescent_Lamp_GENERIC	0
CMH_Ceramic_Metal_Halide_GENERIC	0
	.
	.
	.
MH_Metal_Halide_GENERIC	0
NEW_LIGHT	1
```

It is recommended to put new lights into the Custom Database.

The results of calculation are stored in `kernel/results`. There are following files: the coefficients of decomposition, the list of used lights, the percentage and two plots.

GUI was written in JAVA. See section RUN to start and use GUI.

## Compilation of source code

In `kernel` run commands:

```
gcc -Wall smog.c -o smog -lm
g++ -Wall spectrumavg.cpp -o spectrumavg -lm
gcc -Wall scale_to_one.c -o scale_to_one -lm
gcc -Wall hard_work.c -o hard_work -lm
gcc -Wall cut_spectra.c -o cut_spectra -lm
gcc -Wall cut_exp_spectrum.c -o cut_exp_spectrum -lm
```

## Usage

* GUI can be started from terminal using command:
	* `java -jar SMOG.jar`
* After starting GUI, experimental data must be loaded.
	* `FILE -> Load Spectrum .... choose a file`
* Generic database is loaded as default. Different database can be loaded by choosing an option in Database field.
* Subsequently, it is necessary to tick those lights we want to use for decomposition.
* Set parameters in upper left panel.
* Click `Run` to start calculation.
* The results (percentage, standard deviaton) are loaded automatically.
* Click `Plot INT` and `Plot NON` to view plots with compared experimental and calculated data (before and after integration).
* Click `Find Optimum` to start calculation for each combination of selected lights and set parameters. After calculation is finished, the optimal configuration along with results is automatically loaded. Use this button very carefully. The calculations may take a lot of time.

## Requirements

* Java JRE 8 and higher
* C compiler
* C++ compiler
* Gnuplot (tested on 5.2)
* Eye of Gnome (to view plots in Linux)