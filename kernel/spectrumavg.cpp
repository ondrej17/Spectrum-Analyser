/*
	=============================================================================================================================
	Name: spectrumavg.cpp
	Author: doc. RNDr. Frantisek Kundracik, PhD., Ondrej Bily
	Description: Program SPECTRUMAVG calculates mean integral values of the spectra in constant wavelength intervals.
	Date of latest update: 8.5.2019
	=============================================================================================================================


	------------------------- PROGRAM SPECTRUMAVG--------------------------------------------------------------------------------
	Compile:	g++ -Wall spectrumavg.cpp -o spectrumavg -lm
	Run:      ./spectrumavg <wavelength_interval> <min_wavelength> <max_wavelength> <source_spectrum_file> <output_spectrum_file>
	Example:  ./spectrumavg           5                  350             780             <spc0.spc>           <spc0avg.spc>

	=============================================================================================================================
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <iomanip>

using namespace std;

double Get_spectrum(double wavelength, vector <double> lambda, vector <double> spectrum);
double Get_avg_spectrum(double lambdamin, double lambdamax, double dlambda, vector <double> lambda, vector <double> spectrum);

int main(int argc,char**argv)
{
    // checking for correct command from terminal
    if(argc!=6)
    {
        cout << "Program SPECTRUMAVG calculates mean integral values of the spectra in constant wavelength intervals suitable for SkyGlow simulator."<<endl;
        cout << "First lines of the source file are taken as comments and are copied to the output file without any change."<<endl;
        cout << "Data in the source file have to be sorted ascending by the wavelength."<<endl;
        cout << "Run:   SPECTRUMAVG <wavelength_interval> <min_wavelength> <max_wavelength> <source_spectrum_file> <output_spectrum_file>" << endl;
        cout << "Example:  SPECTRUMAVG 5 350 780 <spc0.spc> <spc0avg.spc>" << endl;
        //system("pause");
        return 1;
    }

    // LOG FILE
    ofstream cout2("logFileAvg.txt");

    // OPEN LOG FILE
    if (!cout2) {
            cout2.close();
            return 1;
    }
    else {
            cout2 << "AVG:\n";
    }
	
    // load parameters for integration
    double dlambda= atof(argv[1]);
    double lambdamin=atof(argv[2]);
    double lambdamax=atof(argv[3]);
    
    // open input file
    ifstream fin(argv[4]);
    if(!fin)
    {
        cout2 << "Can't read the source file "<< argv[4] << endl;
        return 1;
    }

    // open output file
    ofstream fout(argv[5]);
    if(!fout)
    {
        cout2 << "Can't write to output file "<< argv[5] << endl;
        // if the output file has not been opened, close the input file
        fin.close();
        return 1;
    }

    // copy lines with '#' or 'W' at the beginning from input to output file
    string line;
    while (getline(fin,line), ((line[0] == '#') || (line[0] == 'W')))
    {
        fout << line << endl;
    }

    // LOADING OF VECTOR LAMBDA AND SPECTRUM
    vector <double> lambda;
    vector <double> spectrum;
    
    //read all lines while there are two numbers in the line
    double lam_tmp, spe_tmp;

    while(fin>>lam_tmp>>spe_tmp) //while next two numbers were read successfully
    {
        // put wavelength to vector lambda
        lambda.push_back(lam_tmp);

        // put intensity to vector spectrum
        spectrum.push_back(spe_tmp);

        // print lambda and spectrum
        //cout<<lam_tmp<<"\t"<<spe_tmp<<endl;
    }

    // close input file, reading is done
    fin.close();

    // print how many wavelengths are in vector lambda
    cout << "Source file contains " << lambda.size() << " wavelengths" <<endl;

    //---------------------------------------------------------------------------------------------------------------
    //now we have to find the smallest wavelength interval not to loose any peak
    double deltalambda = 1e99;

    for (unsigned int i=0; i < (lambda.size()-1); i++)
    {
        double tmp = lambda.at(i+1)-lambda.at(i);
        if (tmp < deltalambda) deltalambda=tmp;
    }

    // print the smallest wavelength interval in vector lambda
    cout << "deltalambda = " << deltalambda << endl;

    // check whether wavelengths are ordered by magnitude
    if(deltalambda < 1e-3)
    {
        cout2 << "The same wavelengths or reversed wavelengths found in: " << argv[4] << endl;
        return 1;
    }

    //---------------------------------------------------------------------------------------------------------------

    //Now we can write the averaged (integral) spectrum
    double current_lambda = lambdamin;
    double current_spectrum;
    
    while (current_lambda <= (lambdamax+0.5*dlambda)) //we safely write data also for current_lambda=lambdamax
    {
        // calculate integral on given interval
        current_spectrum = Get_avg_spectrum(current_lambda-dlambda/2, current_lambda+dlambda/2, deltalambda, lambda, spectrum);

        // write wavelength and intensity to output file
        fout << setprecision(12) << current_lambda << "\t" << setprecision(12) << current_spectrum << endl;

        // write wavelength and intensity to terminal
        cout << setprecision(12) << current_lambda << "\t" << setprecision(12) << current_spectrum << endl;

        // increase current lambda by one step
        current_lambda += dlambda;
    }

    // close the output file
    fout.close();

    // CLOSE LOG FILE
    cout2 << "OK\n";
    cout2 << "*****************************************\n";
    cout2.close();

    return 0;
}


//returns the average (integral) value of the spectrum within an interval
double Get_avg_spectrum(double lambdamin, double lambdamax, double deltalambda, vector <double> lambda, vector <double> spectrum)
{
    // if borders are inversed, return 0
    if (lambdamax <= lambdamin)	return 0;

    // result holds integral on particular interval
    double result=0;

    //trapezoid-method of the integration
    result += 0.5*Get_spectrum(lambdamin, lambda, spectrum);

    int n = (int)((lambdamax-lambdamin)/deltalambda+0.5); //we safely get n=1 if lambdamax-lambdamin=dlambda
    double actdeltalambda=(lambdamax-lambdamin)/n;
    for(int i=1;i<n;i++)
        result+=Get_spectrum(lambdamin+i*actdeltalambda, lambda, spectrum);
    result+=0.5*Get_spectrum(lambdamax, lambda, spectrum);
    result*=actdeltalambda;
    if(result<0) cout<<lambdamin<<"\t"<<lambdamax<<"\t"<<actdeltalambda<<"\t"<<"Negative value!!!"<<endl;
    return result/(lambdamax-lambdamin); //power per 1nm
    //return result; //integral power
}

//returns the value of the spectrum by the linear interpolation. Extrapolation returns zero value.
double Get_spectrum(double wavelength, vector <double> lambda, vector <double> spectrum)
{
    // if wavelength is out of vector lambda
    if (wavelength < lambda.at(0)) return 0;
    if (wavelength > lambda.at(lambda.size()-1)) return 0;

    // go through vector lambda and find index i, so that lambda[i] > wavelength (first encounter)
    unsigned int i;
    for (i=0; i < lambda.size(); i++)
    {
        if (lambda.at(i) > wavelength)
            //neighboring value found, end for-cycle
            break;
    }

    // if lambda[i] is last value in the table, no interpolation needed, return last value of spectrum
    if (i >= (lambda.size()-1))
        // return spectrum.at(i); // Ondrej Bily changed this line on 23/6/2019
        return spectrum.at(lambda.size()-1);

    // calculate 2*integral
    double result = (spectrum.at(i)-spectrum.at(i-1))/(lambda.at(i)-lambda.at(i-1))*(wavelength-lambda.at(i-1)) + spectrum.at(i-1);

    // if integral is negative, raise the alarm
    if (result < 0)
        cout << wavelength << "\t" << "Negative value!!!!!!!!!!!" << endl;

    return result;
}
