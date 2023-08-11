//Help Heder file "HelpFindBoolFunction.h" - Find CPU computing Boolean functions properties and other functions
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
void FindReadFromFileComputeBooleanCPU(string filename, int menuChoice, int nl_cpuBorder, int AC_cpuBorder, int deg_cpuBorder)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	host_Bool_PTT = (int*)malloc(sizeof(int) * sizeB);
	walshvec_cpu = (int*)malloc(sizeof(int) * sizeB);
	rf_cpu = (int*)malloc(sizeof(int) * sizeB);
	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterBoolean = 0, counterBoundary = 0;// , counterNumSatisfBorder = 0;
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
						free(host_Bool_PTT);
						free(walshvec_cpu);
						free(rf_cpu);
						free(anf_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						host_Bool_PTT = (int*)malloc(sizeof(int) * sizeBool);
						walshvec_cpu = (int*)malloc(sizeof(int) * sizeBool);
						rf_cpu = (int*)malloc(sizeof(int) * sizeBool);
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

						//cout << Element[counter] << " ";
						if (host_Bool_TT[i] == 0)
							host_Bool_PTT[i] = 1;
						else
							host_Bool_PTT[i] = -1;

						//	cout << host_Bool_TT[i] << " ";
					}

					/////////////////////////////////////////////////////////////////////////////
					//Start CPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

					//menuChoice for compute NL(f)
					if (menuChoice == 0)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function CPU (W_f(f))
						FastWalshTrans(sizeBool, host_Bool_PTT, walshvec_cpu);
						int Lin_cpu = reduceCPU_max(walshvec_cpu, sizeBool);
						int nl_cpu = sizeBool / 2 - (Lin_cpu / 2);		//Compute Nonlinearity
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						if (nl_cpu >= nl_cpuBorder)
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}

							myfile << "\nnl(f):" << nl_cpu << " ";
							counterBoundary++;
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
						FastWalshTrans(sizeBool, host_Bool_PTT, rf_cpu);
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

						if (AC_cpu >= AC_cpuBorder)
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}

							myfile << "\nAC(f):" << AC_cpu << " ";
							counterBoundary++;
						}
						counterBoolean++;
					}

					//menuChoice for compute deg(f)
					if (menuChoice == 2)
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

						if (degMax_cpu >= deg_cpuBorder)
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}

							myfile << "\ndeg(f):" << degMax_cpu << " ";
							counterBoundary++;
						}
						counterBoolean++;
					}


					//menuChoice for compute NL(f), AC(f), deg(f)
					if (menuChoice == 3)
					{
						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function CPU (W_f(f))
						FastWalshTrans(sizeBool, host_Bool_PTT, walshvec_cpu);
						int Lin_cpu = reduceCPU_max(walshvec_cpu, sizeBool);
						int nl_cpu = sizeBool / 2 - (Lin_cpu / 2);		//Compute Nonlinearity
						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Call Functions: Compute Autocorrelation CPU (r_f(f))
						//FastWalshTrans(sizeBool, host_Bool_PTT, rf_cpu);
						//PTT_fun_pow2(sizeBool, rf_cpu);
						//FastWalshTransInv(sizeBool, rf_cpu);
						//PTT_fun_pow2(sizeBool, walshvec_cpu);
						fun_pow2(sizeBool, walshvec_cpu);
						FastWalshTransInv(sizeBool, walshvec_cpu);
						int AC_cpu = reduceCPU_AC(sizeBool, walshvec_cpu);
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

						if ((nl_cpu >= nl_cpuBorder) & (AC_cpu <= AC_cpuBorder) & (degMax_cpu >= deg_cpuBorder))
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}

							myfile << "\nnl(f):" << nl_cpu << " AC(f):" << AC_cpu << " deg(f):" << degMax_cpu;
							counterBoundary++;
						}
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

		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterBoolean, (elapsedTime_computation / 1000000.0) / counterBoolean);

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of all compute Boolean(s) function: %d \n", counterBoolean);
		printf("Number of compute Boolean(s) function with specific parameter(s): %d \n\n", counterBoundary);

		myfile.close(); // close the open file

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
	free(host_Bool_PTT);
	free(walshvec_cpu);
	free(rf_cpu);
	free(anf_cpu);
}
//============================================================================

//===== function Find (Bitwise) deg(f) =========
void FindReadFromFileComputeBitWise_ANF_degCPU(string filename, int deg_cpuBorder)
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

	int counter = 0, parameter[3], counterBoolean = 0, counterBoundary = 0;// , counterNumSatisfBorder = 0;
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


					if (degMax_cpu >= deg_cpuBorder)
					{
						myfile << "\n\n? " << "0 " << sizeBool << "\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << host_Bool_TT[ii] << " ";
						}

						myfile << "\ndeg(f):" << degMax_cpu << " ";
						counterBoundary++;
					}
					counterBoolean++;
					//////////////////////////////////////////////////////////////////////////////////////////////////
				}

				//else if there is bad input line
				else
				{
					cout << " There is bad input line ===>possible problem: not boolean function or input size n < 6; \n";
					//return;
				}
				//==================================================
			}
			//	cout << "Number of element into file " << filename << ": " << counter;
		}

		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterBoolean, (elapsedTime_computation / 1000000.0) / counterBoolean);

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n", counterBoolean);
		printf("Number of compute Boolean(s) function with specific parameter(s): %d \n\n", counterBoundary);

		myfile.close(); // close the open file

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