//Help Heder file "HelpFindBoolFunction.h" - Find CPU computing S-box functions properties and other functions
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


//===== function compute Lin(S), nl(S), AC(S), deg(S) and delta(S) =========
void FindReadFromFileComputeSboxCPU(string filename, int menuChoice, int nl_cpuBorder, int AC_cpuBorder, int deg_cpuBorder, int delta_cpuBorder)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	int** STT = AllocateDynamicArray<int>(binary, sizeB);
	binary_num = (int*)malloc(sizeof(int) * binary);

	PTT = (int*)malloc(sizeof(int) * sizeB);
	TT = (int*)malloc(sizeof(int) * sizeB);
	t = (int*)malloc(sizeof(int) * binary);

	WHT = (int*)malloc(sizeof(int) * sizeB);

	rf_cpu = (int*)malloc(sizeof(int) * sizeB);

	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0, counterBoundary = 0, Lin_cpu = 0, AC_cpu = 0, degMax_cpu = 0, delta_cpu = 0;
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

				//@@check is S-box
				if ((parameter[0] == 1) & (parameter[1] > 32) & (parameter[1] <= 33554432)) // border 2^25 element input
				{
					sizeSbox = parameter[1];
					binary = parameter[2] + 1;
					//cout << sizeSbox << binary;

					int Lin_cpuBorder = sizeSbox - 2 * nl_cpuBorder; //Lin(f) = 2 ^ n - 2 * nl(f))
					//Recreate dinamic vector for a diffrent size
					if (sizeSbox != sizeB)
					{
						//@@free host memory
						free(SboxElemet);
						//Allocate memory block. Allocates a block of size bytes of memory
						SboxElemet = (int*)malloc(sizeof(int) * sizeSbox);

						//	FreeDynamicArray<int>(STT);
						//Declaration and Allocate memory blocks 
						STT = AllocateDynamicArray<int>(binary, sizeSbox);

						free(binary_num);
						binary_num = (int*)malloc(sizeof(int) * binary);

						free(PTT);
						PTT = (int*)malloc(sizeof(int) * sizeSbox);
						free(TT);
						TT = (int*)malloc(sizeof(int) * sizeSbox);
						free(t);
						t = (int*)malloc(sizeof(int) * binary);

						free(WHT);
						WHT = (int*)malloc(sizeof(int) * sizeSbox);

						free(rf_cpu);
						rf_cpu = (int*)malloc(sizeof(int) * sizeSbox);

						free(anf_cpu);
						anf_cpu = (int*)malloc(sizeof(int) * sizeSbox);

						sizeB = sizeSbox;
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}
					/////////////////////////////////////////////////////////////////////////////
					//Start CPU program
					/////////////////////////////////////////////////////////////////////////////
					SetSTT(SboxElemet, STT, binary_num);

					counterStart = 0; //counter for verification start point from one S-box

					int m = binary - 1;

					for (int e = 1; e <= m; e++)
						t[e] = e + 1;

					for (int j = 0; j < sizeSbox; j++)
					{
						TT[j] = 0;
					}

					int ii = 1;

					while (ii != m + 1)
					{
						for (int j = 0; j < sizeB; j++)
						{

							TT[j] = TT[j] ^ STT[ii][j];

							if (TT[j] == 1)
								PTT[j] = -1;
							else
								PTT[j] = 1;
						}
						t[0] = 1;
						t[ii - 1] = t[ii];
						t[ii] = ii + 1;
						ii = t[0];

						//menuChoice to find Lin(S), nl(S)
						if (menuChoice == 0)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Walsh Transformation function CPU (W_f(f))
							FastWalshTrans(sizeB, PTT, WHT);	//Find Walsh spectra on one row
							int Lin_return = reduceCPU_max(WHT, sizeB);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (Lin_cpu < Lin_return)
								Lin_cpu = Lin_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (Lin_cpu > Lin_cpuBorder)
							{
								break;
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								int nl_cpu = sizeSbox / 2 - (Lin_cpu / 2);		//Compute Nonlinearity
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nLin(S):" << Lin_cpu << " nl(S):" << nl_cpu;

								counterBoundary++;
							}
							counterStart++;
						}


						//menuChoice to find AC(S)
						if (menuChoice == 1)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//fumcion compute Autocorrelation Table (spectra) ACT(S)
							//Function: Fast Walsh Transformation function CPU (W_f(f))
							FastWalshTrans(sizeB, PTT, rf_cpu);	//Find Walsh spectra on one row
							//PTT_fun_pow2(sizeB, rf_cpu);
							fun_pow2(sizeB, rf_cpu);
							FastWalshTransInv(sizeB, rf_cpu);
							int AC_return = reduceCPU_AC(sizeB, rf_cpu);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (AC_cpu < AC_return)
								AC_cpu = AC_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (AC_cpu > AC_cpuBorder)
							{
								break;
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n\n";
								myfile << "? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nAC(S):" << AC_cpu;

								counterBoundary++;
							}
							counterStart++;
						}

						//menuChoice to find deg(S)
						if (menuChoice == 2)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Mobius Transformation function CPU (ANF(f))
							FastMobiushTrans(sizeB, TT, anf_cpu);
							int degMax_return = FindMaxDeg(sizeB, anf_cpu);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMax_cpu < degMax_return)
								degMax_cpu = degMax_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMax_cpu < deg_cpuBorder)
							{
								break;
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n\n";
								myfile << "? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\ndeg(S):" << degMax_cpu;

								counterBoundary++;
							}
							counterStart++;
						}

						//menuChoice to find delta(S)
						if (menuChoice == 3)
						{
							counterStart++;
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: for finding delta(S)
							//int delta_return = FindDelta_CPU(SboxElemet, counterStart);
							int delta_return = DDT_vector(sizeSbox, SboxElemet, counterStart);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (delta_cpu < delta_return)
								delta_cpu = delta_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////	

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (delta_cpu > delta_cpuBorder)
							{
								break;
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@ =======================
							if (counterStart == sizeB - 1)
							{
								myfile << "\n\n";
								myfile << "? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\ndelta(S):" << delta_cpu;

								counterBoundary++;
							}
						}

						//menuChoice to find Lin(S), nl(S), AC(S), deg(S), delta(S)
						if (menuChoice == 4)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Walsh Transformation function CPU (W_f(f))
							FastWalshTrans(sizeB, PTT, WHT);	//Find Walsh spectra on one row
							int Lin_return = reduceCPU_max(WHT, sizeB);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (Lin_cpu < Lin_return)
								Lin_cpu = Lin_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//fumcion compute Autocorrelation Table (spectra) ACT(S)
							//Function: Fast Walsh Transformation function CPU (W_f(f))
							//FastWalshTrans(size, PTT, rf_cpu);	//Find Walsh spectra on one row
							//PTT_fun_pow2(size, rf_cpu);
							//FastWalshTransInv(size, rf_cpu);
							//PTT_fun_pow2(sizeB, WHT);
							fun_pow2(sizeB, WHT);
							FastWalshTransInv(sizeB, WHT);
							int AC_return = reduceCPU_AC(sizeB, WHT);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (AC_cpu < AC_return)
								AC_cpu = AC_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Mobius Transformation function CPU (ANF(f))
							FastMobiushTrans(sizeB, TT, anf_cpu);
							int degMax_return = FindMaxDeg(sizeB, anf_cpu);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMax_cpu < degMax_return)
								degMax_cpu = degMax_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: for finding delta(S)
							//int delta_return = FindDelta_CPU(SboxElemet, counterStart + 1);
							int delta_return = DDT_vector(sizeSbox, SboxElemet, counterStart+1);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (delta_cpu < delta_return)
								delta_cpu = delta_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////	

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;

							/////////////////////////////////////////////////////////////////////////////////////////////////
							if ((Lin_cpu > Lin_cpuBorder) | (AC_cpu > AC_cpuBorder) | (degMax_cpu < deg_cpuBorder) | (delta_cpu > delta_cpuBorder))
							{
								break;
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								int nl_cpu = sizeSbox / 2 - (Lin_cpu / 2);		//Compute Nonlinearity
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nLin(S):" << Lin_cpu << " nl(S):" << nl_cpu << " AC(S):" << AC_cpu << " deg(S):" << degMax_cpu << " delta(S):" << delta_cpu;

								counterBoundary++;
							}
							counterStart++;
						}

						/////////////////////////////////////////////////////////////////////////////////////////////////////////////
					}
					counterSbox++;
					Lin_cpu = 0;
					AC_cpu = 0;
					degMax_cpu = 0;
					delta_cpu = 0;
					//==================================================	
				}
				//else if there is bad input line
				else
				{
					cout << " There is bad input line ===>possible problem: not S-box;\n";
					//return;
				}
				//==================================================
			}
			//	cout << "Number of element into file " << filename << ": " << counter;
		}

		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterSbox, (elapsedTime_computation / 1000000.0) / counterSbox);

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of all S-box(es) function: %d \n", counterSbox);
		printf("Number of compute S-box(es) function with specific parameters: %d \n\n", counterBoundary);

		myfile.close(); // close the open file

		//@@set old value of size
		binary = 0;
		sizeB = 0;
		counterSbox = 0;
	}

	//@@Else statment if file not exist
	else
	{
		cout << "\n====== Error in opening file '" << filename << "' could not be opened ======\n" << endl;
	}

	//Print in console End time
	ShowEndTime();

	//free memory
	free(SboxElemet);

	FreeDynamicArray<int>(STT);
	free(binary_num);

	free(PTT);
	free(TT);
	free(t);

	free(WHT);
	free(rf_cpu);
	free(anf_cpu);
}

//===== function Find (bitwise) deg(S) =========
void FindReadFromFileComputeBitwiseANF_degSboxCPU(string filename, int menuChoice, int deg_cpuBorder)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	int** STT = AllocateDynamicArray<int>(binary, sizeB);
	binary_num = (int*)malloc(sizeof(int) * binary);

	TT = (int*)malloc(sizeof(int) * sizeB);
	t = (int*)malloc(sizeof(int) * binary);

	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Allocate memory block for bitwise computation
	NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);
	NumIntVecANF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int NumOfBits, NumInt; //bitwise variables

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0, counterBoundary = 0, degMax_cpu = 0;
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

				//@@check is S-box
				if ((parameter[0] == 1) & (parameter[1] > 32) & (parameter[1] <= 33554432)) // border 2^25 element input
				{
					sizeSbox = parameter[1];
					binary = parameter[2] + 1;
					//cout << sizeSbox << binary;

					//Recreate dinamic vector for a diffrent size
					if (sizeSbox != sizeB)
					{
						//@@free host memory
						free(SboxElemet);
						//Allocate memory block. Allocates a block of size bytes of memory
						SboxElemet = (int*)malloc(sizeof(int) * sizeSbox);

						//	FreeDynamicArray<int>(STT);
						//Declaration and Allocate memory blocks 
						STT = AllocateDynamicArray<int>(binary, sizeSbox);

						free(binary_num);
						binary_num = (int*)malloc(sizeof(int) * binary);

						free(TT);
						TT = (int*)malloc(sizeof(int) * sizeSbox);
						free(t);
						t = (int*)malloc(sizeof(int) * binary);

						free(anf_cpu);
						anf_cpu = (int*)malloc(sizeof(int) * sizeSbox);

						sizeB = sizeSbox;

						//bitwise variable and memory
						NumOfBits = sizeof(unsigned long long int) * 8;
						NumInt = sizeB / NumOfBits;

						free(NumIntVecTT);
						free(NumIntVecANF);

						NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);
						NumIntVecANF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}

					/////////////////////////////////////////////////////////////////////////////
					//Start CPU program
					/////////////////////////////////////////////////////////////////////////////
					SetSTT(SboxElemet, STT, binary_num);

					counterStart = 0; //counter for verification start point from one S-box

					int m = binary - 1;

					for (int e = 1; e <= m; e++)
						t[e] = e + 1;

					for (int j = 0; j < sizeSbox; j++)
					{
						TT[j] = 0;
					}

					int ii = 1;

					while (ii != m + 1)
					{
						for (int j = 0; j < sizeB; j++)
						{
							TT[j] = TT[j] ^ STT[ii][j];
						}
						t[0] = 1;
						t[ii - 1] = t[ii];
						t[ii] = ii + 1;
						ii = t[0];


						//menuChoice for find bitwise deg(S)
						if (menuChoice == 0)
						{
							//set TT in 64 bit integer array
							BinVecToDec(NumOfBits, TT, NumIntVecTT, NumInt);

							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							// start CPU bitwise function
							//CPU_FWT_bitwise(NumIntVecTT, NumIntVecANF, NumOfBits, NumInt);
							CPU_FMT_bitwise(NumIntVecTT, NumIntVecANF, NumInt);
							//conversion from decimal to binary and fine deg(f)
							int degMax_return = DecVecToBin_maxDeg(NumOfBits, NumIntVecANF, NumInt);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMax_cpu < degMax_return)
								degMax_cpu = degMax_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							//@@ =======================
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMax_cpu < deg_cpuBorder)
							{
								break;
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n\n";
								myfile << "? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\ndeg(S):" << degMax_cpu;

								counterBoundary++;
							}
							counterStart++;
						}
					}
					counterSbox++;
					degMax_cpu = 0;
					//==================================================	
				}
				//else if there is bad input line
				else
				{
					cout << " There is bad input line ===>possible problem: not S-box;\n";
					//return;
				}
				//==================================================
			}
			//	cout << "Number of element into file " << filename << ": " << counter;
		}

		printf("\nElapsed time (only computation): %f (microseconds), %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterSbox, (elapsedTime_computation / 1000000.0) / counterSbox);

		//@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute S-box(es) function: %d \n", counterSbox);
		printf("Number of compute S-box(es) function with specific parameters: %d \n\n", counterBoundary);

		myfile.close(); // close the open file

		//@@set old value of size
		binary = 0;
		sizeB = 0;
		counterSbox = 0;
	}
	//@@Else statment if file not exist
	else
	{
		cout << "\n====== Error in opening file '" << filename << "' could not be opened ======\n" << endl;
	}
	//Print in console End time
	ShowEndTime();

	//free memory
	free(SboxElemet);

	FreeDynamicArray<int>(STT);
	free(binary_num);

	free(TT);
	free(t);

	free(anf_cpu);

	free(NumIntVecTT);
	free(NumIntVecANF);
}