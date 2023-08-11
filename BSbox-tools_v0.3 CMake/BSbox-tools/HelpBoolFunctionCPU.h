//Help Heder file "HelpBoolFunction.h" - CPU computing Boolean functions properties and other functions
//input header files
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>

#include <algorithm> 
#include <chrono> 

#include <nmmintrin.h> //

using namespace std;
using namespace std::chrono;


//===== function compute Walsh spectra, Lin and NL =========
void ReadFromFileComputeNlCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	walshvec_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
	string str, line, sub, ch = "?", type; // Temp string to
	ifstream fin(filename); // Open it up!
	if (fin.is_open())
	{
		cout << " Input file '" << filename << "' is Opened successfully!.\n";

		//Print in console Start time
		ShowStartTime();
		cout << "\n";

		//Define low timer computation 
		double elapsedTime_comp = 0, elapsedTime_part_comp = 0;
		//@Global Time computation variables set zero
		elapsedTime_computation_part = 0, elapsedTime_computation = 0;

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t, start_comp, end_comp;
		start_t = clock();

		////============================================================================
		while (fin >> str)
		{
			//check character "?"
			if (str == ch)
			{
				//		cout << "\n";
				getline(fin, line);
				istringstream iss(line);

				counter = 0;
				while (iss >> sub)
				{
					parameter[counter] = atoi(sub.c_str());
					counter++;
				}
				//@@check is boolean function
				if ((parameter[0] == 0) & (parameter[1] > 32) & (parameter[1] <= 33554432)) // border 2^25 element input
				{
					sizeBool = parameter[1];

					//Recreate dinamic vector for a diffrent size
					if (sizeBool != sizeB)
					{
						//@@free host memory
						free(host_Bool_TT);
						free(walshvec_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						walshvec_cpu = (int*)malloc(sizeof(int) * sizeBool);

						sizeB = sizeBool;
					}
					//=====================================

					for (int i = 0; i < sizeBool; i++)
					{
						fin >> str;

						//copy from file to host memory
						host_Bool_TT[i] = atoi(str.c_str());
						//cout << str << " ";

						//cout << Element[counter] << " ";
						if (host_Bool_TT[i] == 0)
							host_Bool_TT[i] = 1;
						else
							host_Bool_TT[i] = -1;

						//	cout << host_Bool_TT[i] << " ";
					}

					/////////////////////////////////////////////////////////////////////////////
					//Start CPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

					//menuChoice for compute Walsh spectar WS(f)
					if (menuChoice == 0)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function CPU (W_f(f))
						FastWalshTrans(sizeBool, host_Bool_TT, walshvec_cpu);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << " WS(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << walshvec_cpu[ii] << " ";
						}
						myfile << "\n";
						counterBoolean++;
					}

					//menuChoice for compute Lin(f)
					if (menuChoice == 1)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function CPU (W_f(f))
						FastWalshTrans(sizeBool, host_Bool_TT, walshvec_cpu);
						int Lin_cpu = reduceCPU_max(walshvec_cpu, sizeBool);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n\n? " << "0 " << sizeBool << "\nLin(f):" << Lin_cpu << "\n";

						counterBoolean++;
					}

					//menuChoice for compute Walsh spectar WS(f) and Lin(f)
					if (menuChoice == 2)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function CPU (W_f(f))
						FastWalshTrans(sizeBool, host_Bool_TT, walshvec_cpu);
						int Lin_cpu = reduceCPU_max(walshvec_cpu, sizeBool);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << " WS(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << walshvec_cpu[ii] << " ";
						}
						myfile << "\nLin(f):" << Lin_cpu << "\n";
						counterBoolean++;
					}

					//menuChoice for compute nl(f)
					if (menuChoice == 3)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function CPU (W_f(f))
						FastWalshTrans(sizeBool, host_Bool_TT, walshvec_cpu);
						int Lin_cpu = reduceCPU_max(walshvec_cpu, sizeBool);
						int nl_cpu = sizeBool / 2 - (Lin_cpu / 2);		//Compute Nonlinearity
						//////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << "\nnl(f):" << nl_cpu << "\n";

						counterBoolean++;
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////
				}

				//else if there is bad input line
				else
				{
					cout << " There is bad input line ===>possible problem: not boolean function; \n";
					//return;
				}
				//==================================================
			}
			//	cout << "Number of element into file " << filename << ": " << counter;
		}

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file


		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterBoolean, (elapsedTime_computation / 1000000.0) / counterBoolean);

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n\n", counterBoolean);

		//@@set old value of size
		sizeB = 0;
		//	counterNumSatisfBorder = 0;
	}

	//@@Else statment if file not exist
	else
	{
		cout << "\n====== Error in opening file '" << filename << "' could not be opened ======\n" << endl;
	}

	//Print in console End time
	ShowEndTime();

	//@Free memory
	free(host_Bool_TT);
	free(walshvec_cpu);
}
//============================================================================

//===== function compute Altocorelation spectra, AC =========
void ReadFromFileComputeACCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	rf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
	string str, line, sub, ch = "?", type; // Temp string to
	ifstream fin(filename); // Open it up!
	if (fin.is_open())
	{
		cout << " Input file '" << filename << "' is Opened successfully!.\n";

		//Print in console Start time
		ShowStartTime();
		cout << "\n";

		//Define low timer computation 
		double elapsedTime_comp = 0, elapsedTime_part_comp = 0;
		//@Global Time computation variables set zero
		elapsedTime_computation_part = 0, elapsedTime_computation = 0;

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t, start_comp, end_comp;
		start_t = clock();

		////============================================================================
		while (fin >> str)
		{
			//check character "?"
			if (str == ch)
			{
				//		cout << "\n";
				getline(fin, line);
				istringstream iss(line);

				counter = 0;
				while (iss >> sub)
				{
					parameter[counter] = atoi(sub.c_str());
					counter++;
				}
				//@@check is boolean function
				if ((parameter[0] == 0) & (parameter[1] > 32) & (parameter[1] <= 33554432)) // border 2^25 element input
				{
					sizeBool = parameter[1];

					//Recreate dinamic vector for a diffrent size
					if (sizeBool != sizeB)
					{
						//@@free host memory
						free(host_Bool_TT);
						free(rf_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						rf_cpu = (int*)malloc(sizeof(int) * sizeBool);

						sizeB = sizeBool;
					}
					//=====================================

					for (int i = 0; i < sizeBool; i++)
					{
						fin >> str;

						//copy from file to host memory
						host_Bool_TT[i] = atoi(str.c_str());
						//cout << str << " ";

						//cout << Element[counter] << " ";
						if (host_Bool_TT[i] == 0)
							host_Bool_TT[i] = 1;
						else
							host_Bool_TT[i] = -1;
						//	cout << host_Bool_TT[i] << " ";
					}

					/////////////////////////////////////////////////////////////////////////////
					//Start CPU program
					/////////////////////////////////////////////////////////////////////////////

					//menuChoice for compute Autocorrelation spectar AS(f)
					if (menuChoice == 0)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Call Functions: Compute Autocorrelation CPU (r_f(f))
						FastWalshTrans(sizeBool, host_Bool_TT, rf_cpu);
						//PTT_fun_pow2(sizeBool, rf_cpu);
						fun_pow2(sizeBool, rf_cpu);
						FastWalshTransInv(sizeBool, rf_cpu);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << " AS(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << rf_cpu[ii] << " ";
						}
						counterBoolean++;
					}

					//menuChoice for compute AC(f)
					if (menuChoice == 1)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////
						//Call Functions: Compute Autocorrelation CPU (r_f(f))
						FastWalshTrans(sizeBool, host_Bool_TT, rf_cpu);
						//PTT_fun_pow2(sizeBool, rf_cpu);
						fun_pow2(sizeBool, rf_cpu);
						FastWalshTransInv(sizeBool, rf_cpu);
						int AC_cpu = reduceCPU_AC(sizeBool, rf_cpu);
						/////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << "\nAC(f):" << AC_cpu << "\n";

						counterBoolean++;
					}

					//menuChoice for compute Autocorrelation spectar AS(f) and AC(f)
					if (menuChoice == 2)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Call Functions: Compute Autocorrelation CPU (r_f(f))
						FastWalshTrans(sizeBool, host_Bool_TT, rf_cpu);
						//PTT_fun_pow2(sizeBool, rf_cpu);
						fun_pow2(sizeBool, rf_cpu);
						FastWalshTransInv(sizeBool, rf_cpu);
						int AC_cpu = reduceCPU_AC(sizeBool, rf_cpu);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << " AC(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << rf_cpu[ii] << " ";
						}
						myfile << "\nAC(f):" << AC_cpu << "\n";
						counterBoolean++;
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////
				}

				//else if there is bad input line
				else
				{
					cout << " There is bad input line ===>possible problem: not boolean function; \n";
					//return;
				}
				//==================================================
			}
			//	cout << "Number of element into file " << filename << ": " << counter;
		}

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterBoolean, (elapsedTime_computation / 1000000.0) / counterBoolean);

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n\n", counterBoolean);

		//@@set old value of size
		sizeB = 0;

		//	counterNumSatisfBorder = 0;
	}

	//@@Else statment if file not exist
	else
	{
		cout << "\n====== Error in opening file '" << filename << "' could not be opened ======\n" << endl;
	}

	//Print in console End time
	ShowEndTime();

	//@Free memory
	free(host_Bool_TT);
	free(rf_cpu);
}
//============================================================================

//===== function compute TT(f)->ANF(f) or ANF(f) -> TT(f) and deg(f) =========
void ReadFromFileComputeANF_degCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
	string str, line, sub, ch = "?", type; // Temp string to
	ifstream fin(filename); // Open it up!
	if (fin.is_open())
	{
		cout << " Input file '" << filename << "' is Opened successfully!.\n";

		//Print in console Start time
		ShowStartTime();
		cout << "\n";

		//Define low timer computation 
		double elapsedTime_comp = 0, elapsedTime_part_comp = 0;
		//@Global Time computation variables set zero
		elapsedTime_computation_part = 0, elapsedTime_computation = 0;

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t, start_comp, end_comp;
		start_t = clock();

		////============================================================================
		while (fin >> str)
		{
			//check character "?"
			if (str == ch)
			{
				//		cout << "\n";
				getline(fin, line);
				istringstream iss(line);

				counter = 0;
				while (iss >> sub)
				{
					parameter[counter] = atoi(sub.c_str());
					counter++;
				}

				//@@check is boolean function
				if ((parameter[0] == 0) & (parameter[1] > 32) & (parameter[1] <= 33554432)) // border 2^25 element input
				{
					sizeBool = parameter[1];

					//Recreate dinamic vector for a diffrent size
					if (sizeBool != sizeB)
					{
						//@@free host memory
						free(host_Bool_TT);
						free(anf_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						anf_cpu = (int*)malloc(sizeof(int) * sizeBool);

						sizeB = sizeBool;
					}
					//=====================================

					for (int i = 0; i < sizeBool; i++)
					{
						fin >> str;

						//copy from file to host memory
						host_Bool_TT[i] = atoi(str.c_str());
						//cout << str << " ";
					}
					/////////////////////////////////////////////////////////////////////////////
					//Start CPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

					//menuChoice for compute ANF(f)
					if (menuChoice == 0)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Mobius Transformation function CPU (ANF(f))
						FastMobiushTrans(sizeBool, host_Bool_TT, anf_cpu);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << " ANF(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << anf_cpu[ii] << " ";
						}
						myfile << "\n";
						counterBoolean++;
					}

					//menuChoice for compute deg(f)
					if (menuChoice == 1)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Mobius Transformation function CPU (ANF(f))
						FastMobiushTrans(sizeBool, host_Bool_TT, anf_cpu);
						int degMax_cpu = FindMaxDeg(sizeBool, anf_cpu);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << "\ndeg(f):" << degMax_cpu << "\n";

						counterBoolean++;
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////
				}

				//else if there is bad input line
				else
				{
					cout << " There is bad input line ===>possible problem: not boolean function; \n";
					//return;
				}
				//==================================================
			}
			//	cout << "Number of element into file " << filename << ": " << counter;
		}

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterBoolean, (elapsedTime_computation / 1000000.0) / counterBoolean);

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n\n", counterBoolean);


		//@@set old value of size
		sizeB = 0;
		//	counterNumSatisfBorder = 0;
	}

	//@@Else statment if file not exist
	else
	{
		cout << "\n====== Error in opening file '" << filename << "' could not be opened ======\n" << endl;
	}

	//Print in console End time
	ShowEndTime();

	//@Free memory
	free(host_Bool_TT);
	free(anf_cpu);
}
//============================================================================

//===== function Compute (Bitwise) ANF from TT and vice versa =========
void ReadFromFileComputeBitWise_ANFCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Allocate memory block for bitwise computation
	NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);
	NumIntVecANF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
	int NumOfBits, NumInt; //bitwise variables
	string str, line, sub, ch = "?", type; // Temp string to
	ifstream fin(filename); // Open it up!
	if (fin.is_open())
	{
		cout << " Input file '" << filename << "' is Opened successfully!.\n";

		//Print in console Start time
		ShowStartTime();
		cout << "\n";

		//Define low timer computation 
		double elapsedTime_comp = 0, elapsedTime_part_comp = 0;
		//@Global Time computation variables set zero
		elapsedTime_computation_part = 0, elapsedTime_computation = 0;

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t, start_comp, end_comp;
		start_t = clock();

		////============================================================================
		while (fin >> str)
		{
			//check character "?"
			if (str == ch)
			{
				//		cout << "\n";
				getline(fin, line);
				istringstream iss(line);

				counter = 0;
				while (iss >> sub)
				{
					parameter[counter] = atoi(sub.c_str());
					counter++;
				}

				//@@check is boolean function
				if ((parameter[0] == 0) & (parameter[1] > 64) & (parameter[1] <= 33554432)) // border 2^25 element input
				{
					sizeBool = parameter[1];

					//Recreate dinamic vector for a diffrent size
					if (sizeBool != sizeB)
					{
						//@@free host memory
						free(host_Bool_TT);
						free(anf_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						anf_cpu = (int*)malloc(sizeof(int) * sizeBool);

						sizeB = sizeBool;

						//bitwise variable and memory
						NumOfBits = sizeof(unsigned long long int) * 8;
						NumInt = sizeB / NumOfBits;

						free(NumIntVecTT);
						free(NumIntVecANF);

						NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);
						NumIntVecANF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);
					}
					//=====================================

					for (int i = 0; i < sizeBool; i++)
					{
						fin >> str;

						//copy from file to host memory
						host_Bool_TT[i] = atoi(str.c_str());
						//cout << str << " ";
					}
					/////////////////////////////////////////////////////////////////////////////
					//Start CPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

					//menuChoice for compute bitwise ANF(f)
					if (menuChoice == 0)
					{
						//set TT in 64 bit integer array
						BinVecToDec(NumOfBits, host_Bool_TT, NumIntVecTT, NumInt);

						////=========== Generate mask ===================
						//genMaskFunc(mask_vec, NumOfBits, mack_vec_Int);

						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						// start CPU bitwise function
						CPU_FMT_bitwise(NumIntVecTT, NumIntVecANF, NumInt);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						//conversion from decimal to binary on ANF array
						DecVecToBin(NumOfBits, anf_cpu, NumIntVecANF, NumInt);

						myfile << "\n? " << "0 " << sizeBool << " bitwise ANF(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << anf_cpu[ii] << " ";
						}
						myfile << "\n";
						counterBoolean++;
					}

					//menuChoice for compute bitwise deg(f)
					if (menuChoice == 1)
					{
						//set TT in 64 bit integer array
						BinVecToDec(NumOfBits, host_Bool_TT, NumIntVecTT, NumInt);

						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						// start CPU bitwise function
						CPU_FMT_bitwise(NumIntVecTT, NumIntVecANF, NumInt);
						//conversion from decimal to binary and fine deg(f)
						int degMax_cpu = DecVecToBin_maxDeg(NumOfBits, NumIntVecANF, NumInt);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n? " << "0 " << sizeBool << "\ndeg(f):" << degMax_cpu << "\n";

						counterBoolean++;
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////
				}

				//else if there is bad input line
				else
				{
					cout << " There is bad input line ===>possible problem: not boolean function/or too small boolean function; \n";
					//return;
				}
				//==================================================
			}
			//	cout << "Number of element into file " << filename << ": " << counter;
		}

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterBoolean, (elapsedTime_computation / 1000000.0) / counterBoolean);

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n\n", counterBoolean);

		//@@set old value of size
		sizeB = 0;
		//	counterNumSatisfBorder = 0;
	}

	//@@Else statment if file not exist
	else
	{
		cout << "\n====== Error in opening file '" << filename << "' could not be opened ======\n" << endl;
	}

	//Print in console End time
	ShowEndTime();

	//@Free memory
	free(host_Bool_TT);
	free(anf_cpu);

	free(NumIntVecTT);
	free(NumIntVecANF);
}
//============================================================================