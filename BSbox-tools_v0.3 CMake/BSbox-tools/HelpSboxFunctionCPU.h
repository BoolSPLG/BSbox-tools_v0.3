//Help Heder file "HelpBoolFunction.h" - CPU computing S-box functions properties and other functions
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

//============ function for binary convert ========================
void binary1(int number, int* binary_num) {
	int w = number, c, k, i = 0;
	for (c = binary - 1; c >= 0; c--)
	{
		k = w >> c;

		if (k & 1)
		{
			binary_num[i] = 1;
			i++;
		}
		else
		{
			binary_num[i] = 0;
			i++;
		}
	}
}
//=================================================================

//============ Set STT file for Sbox ==============================
void SetSTT(int* SboxElemet, int** STT, int* binary_num)
{
	int elementS = 0;
	for (int j = 0; j < sizeSbox; j++)
	{
		elementS = SboxElemet[j];
		binary1(elementS, binary_num);

		for (int i = 0; i < binary; i++)
		{
			STT[i][j] = binary_num[i];
			//cout << STT[i][j] << " ";
		}
		//cout << "\n";
	}
}
//=================================================================

//===== function compute Linear Approximation Table, Lin(S) and NL(S) =========
void ReadFromFileComputeNlSboxCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	int** STT = AllocateDynamicArray<int>(binary, sizeB);
	binary_num = (int*)malloc(sizeof(int) * binary);

	PTT = (int*)malloc(sizeof(int) * sizeB);
	TT = (int*)malloc(sizeof(int) * sizeB);
	t = (int*)malloc(sizeof(int) * binary);

	//	PTT_ALL = (int *)malloc(sizeof(int)* size);

	WHT = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0, Lin_cpu = 0;
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

						free(PTT);
						PTT = (int*)malloc(sizeof(int) * sizeSbox);
						free(TT);
						TT = (int*)malloc(sizeof(int) * sizeSbox);
						free(t);
						t = (int*)malloc(sizeof(int) * binary);

						free(WHT);
						WHT = (int*)malloc(sizeof(int) * sizeSbox);

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


						//menuChoice for compute Linear Approximation Table of S-box: LAT(S)
						if (menuChoice == 0)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Walsh Transformation function CPU (W_f(f))
							FastWalshTrans(sizeB, PTT, WHT);	//Find Walsh spectra on one row
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							//@@ =======================

							if (counterStart == 0)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " LAT(S)\n";
							}

							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << WHT[ii]/2 << " ";
							}
							myfile << "\n";
							counterStart++;
						}

						//menuChoice for compute Lin(S)
						if (menuChoice == 1)
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
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \nLin(S):" << Lin_cpu << " ";
							}
							counterStart++;
						}

						//menuChoice for compute Compute Walsh Spectrum Table WS(S) and Lin(S) of S-box
						if (menuChoice == 2)
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
							//@@ =======================
							if (counterStart == 0)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " WS(S)\n";
							}

							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << WHT[ii] << " ";
							}									

							if (counterStart == sizeB - 2)
							{
								myfile << "\nLin(S):" << Lin_cpu << " ";
							}
							myfile << "\n";
							counterStart++;
						}

						//menuChoice for compute nl(S)
						if (menuChoice == 3)
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
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								int nl_cpu = sizeSbox / 2 - (Lin_cpu / 2);		//Compute Nonlinearity
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \nNL(S):" << nl_cpu << " ";
							}
							counterStart++;
						}
					}
					counterSbox++;
					Lin_cpu = 0;
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

		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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
}

//===== function compute Autocorrelation Spectra, AC(S) =========
void ReadFromFileComputeACSboxCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	int** STT = AllocateDynamicArray<int>(binary, sizeB);
	binary_num = (int*)malloc(sizeof(int) * binary);

	PTT = (int*)malloc(sizeof(int) * sizeB);
	TT = (int*)malloc(sizeof(int) * sizeB);
	t = (int*)malloc(sizeof(int) * binary);

	//	PTT_ALL = (int *)malloc(sizeof(int)* size);

	rf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0, AC_cpu = 0;
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

						free(PTT);
						PTT = (int*)malloc(sizeof(int) * sizeSbox);
						free(TT);
						TT = (int*)malloc(sizeof(int) * sizeSbox);
						free(t);
						t = (int*)malloc(sizeof(int) * binary);

						free(rf_cpu);
						rf_cpu = (int*)malloc(sizeof(int) * sizeSbox);

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

						//menuChoice for compute Autocorrelation Spectra of S-box ACT(S)
						if (menuChoice == 0)
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
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							//@@ =======================

							if (counterStart == 0)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " ACT(S)\n";
							}

							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << rf_cpu[ii] << " ";
							}
							myfile << "\n";
							counterStart++;
						}
						//menuChoice for compute AC(S)
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
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \nAC(S):" << AC_cpu << " ";
							}
							counterStart++;
						}
						//menuChoice for compute Autocorrelation Spectra ACT(S) and AC(S) of S-box
						if (menuChoice == 2)
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
							//@@ =======================

							if (counterStart == 0)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " ACT(S)\n";
							}

							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << rf_cpu[ii] << " ";
							}
							myfile << "\n";

							if (counterStart == sizeB - 2)
							{
								myfile << "\nAC(S):" << AC_cpu << " ";
							}
							counterStart++;
						}
					}
					counterSbox++;
					AC_cpu = 0;
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

		printf("\nElapsed time (only computation): %f microseconds, %f (ms),  %f (seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000.0, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f (ms), %f (seconds) \n", (elapsedTime_computation / 1000.0) / counterSbox, (elapsedTime_computation / 1000000.0) / counterSbox);

		//@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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

	free(rf_cpu);
}

//===== function compute TT(S)->ANF(S) or ANF(S) -> TT(S) and deg(S) =========
void ReadFromFileComputeANF_degSboxCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	int** STT = AllocateDynamicArray<int>(binary, sizeB);
	binary_num = (int*)malloc(sizeof(int) * binary);

	TT = (int*)malloc(sizeof(int) * sizeB);
	t = (int*)malloc(sizeof(int) * binary);

	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0, degMax_cpu = 0, degMin_cpu = 100;
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

						//menuChoice for compute Algebraic Normal Form ANF(S) of S-box
						if (menuChoice == 0)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Mobius Transformation function CPU (ANF(f))
							FastMobiushTrans(sizeB, TT, anf_cpu);
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							//@@ =======================

							if (counterStart == 0)
							{
								myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " ANF(S)\n";
							}

							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << anf_cpu[ii] << " ";
							}
							myfile << "\n";
							counterStart++;
						}

						//menuChoice for Algebraic Degree Table of S-box: ADT(S)
						if (menuChoice == 1)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Mobius Transformation function CPU (ANF(f))
							FastMobiushTrans(sizeB, TT, anf_cpu);
							//int degMax_return = FindMaxDeg(sizeB, anf_cpu);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//if (degMax_cpu < degMax_return)
							//	degMax_cpu = degMax_return;
							 
//							int FindMaxDeg(int size, int* ANF_CPU)
//							{
							unsigned int ones = 0;// , max = 0;// , min = 100;
							for (int i = 0; i < sizeB; i++)
							{
								// code specific to Visual Studio compiler
							#if defined (_MSC_VER)
								ones = _mm_popcnt_u32(i) * anf_cpu[i];
							#endif
								// code specific to gcc compiler
							#if defined (__GNUC__)
								ones = __builtin_popcount(i) * anf_cpu[i];
							#endif
								anf_cpu[i] = ones;
							}
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							//@@ =======================

							if (counterStart == 0)
							{
								myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " ADT(S)\n";
							}

							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << anf_cpu[ii] << " ";
							}
							myfile << "\n";
							counterStart++;
						}

						//menuChoice for compute (max) deg(S)
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
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(max)deg(S):" << degMax_cpu << "\n";
							}
							counterStart++;
						}

						//menuChoice for compute (min) deg(S)
						if (menuChoice == 3)
						{
							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Mobius Transformation function CPU (ANF(f))
							FastMobiushTrans(sizeB, TT, anf_cpu);
							int degMin_return = FindMaxDeg(sizeB, anf_cpu);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMin_cpu > degMin_return)
								degMin_cpu = degMin_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(min)deg(S):" << degMin_cpu << "\n";
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

		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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
}

//===== function compute (bitwise TT(S)->ANF(S) or ANF(S) -> TT(S) and deg(S) =========
void ReadFromFileComputeBitwiseANF_degSboxCPU(string filename, int menuChoice)
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

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0, degMax_cpu = 0, degMin_cpu = 100;
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

						//menuChoice for compute bitwise Algebraic Normal Form of S-box ANF(S)
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
							CPU_FMT_bitwise(NumIntVecTT, NumIntVecANF, NumInt);
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;

							//conversion from decimal to binary on ANF array
							DecVecToBin(NumOfBits, anf_cpu, NumIntVecANF, NumInt);

							//@@ =======================

							if (counterStart == 0)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " ANF(S)\n";
							}

							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << anf_cpu[ii] << " ";
							}
							myfile << "\n";
							counterStart++;
						}

						//menuChoice for compute bitwise (max) deg(S)
						if (menuChoice == 1)
						{
							//set TT in 64 bit integer array
							BinVecToDec(NumOfBits, TT, NumIntVecTT, NumInt);

							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							// start CPU bitwise function
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
							if (counterStart == sizeB - 2)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(max)deg(S):" << degMax_cpu << " ";
							}
							counterStart++;
						}

						//menuChoice for compute bitwise (min) deg(S)
						if (menuChoice == 2)
						{
							//set TT in 64 bit integer array
							BinVecToDec(NumOfBits, TT, NumIntVecTT, NumInt);

							//start low resolution timer
							start_comp = clock();
							//start high resolution timer
							getStartTimeComputation();

							/////////////////////////////////////////////////////////////////////////////////////////////////
							// start CPU bitwise function
							CPU_FMT_bitwise(NumIntVecTT, NumIntVecANF, NumInt);
							//conversion from decimal to binary and fine deg(f)
							int degMin_return = DecVecToBin_maxDeg(NumOfBits, NumIntVecANF, NumInt);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMin_cpu > degMin_return)
								degMin_cpu = degMin_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//End high resolution timer
							EndTimeComputationPrint();

							//@@End low resolution timer
							end_comp = clock();
							elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
							//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
							elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
							//@@ =======================
							if (counterStart == sizeB - 2)
							{
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(min)deg(S):" << degMin_cpu << " ";
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

		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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

//===== function compute DDT(S) and delta(S) =========
void ReadFromFileComputeDDT_deltaSboxCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0, delta_cpu = 0;
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
					counterStart = 0; //counter for verification start point from one S-box

						//menuChoice for compute Difference Distribution Table of S-box DDT(S)
					if (menuChoice == 0)
					{
						if (counterStart == 0)
						{
							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
						}

						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						for (counterStart = 1; counterStart < sizeSbox; counterStart++)
						{
							//Function: for finding DDT(S)
							//DDT_vector(sizeSbox, SboxElemet, counterStart);
							int* DDT = new int[sizeSbox]();
							int x1, x2, dy;
							for (x1 = 0; x1 < sizeSbox; ++x1) {
								x2 = x1 ^ counterStart;
								dy = SboxElemet[x1] ^ SboxElemet[x2];
								++DDT[dy];
							}
							for (int i = 0; i < sizeSbox; ++i) {
								myfile << " " << DDT[i];
							}
							myfile << "\n";
							delete[] DDT;
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================
					}

					//menuChoice for compute delta(S)
					if (menuChoice == 1)
					{
						delta_cpu = 0;
						if (counterStart == 0)
						{
							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " DDT(S)\n";
						}

						//start low resolution timer
						start_comp = clock();
						//start high resolution timer
						getStartTimeComputation();

						/////////////////////////////////////////////////////////////////////////////////////////////////
						for (counterStart = 1; counterStart < sizeSbox; counterStart++)
						{
							//Function: for finding delta(S)
							//int delta_return = FindDelta_CPU(SboxElemet, counterStart);
							int delta_return = DDT_vector(sizeSbox, SboxElemet, counterStart);
							
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (delta_cpu < delta_return)
								delta_cpu = delta_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////							
						}
						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "delta(S):" << delta_cpu << " ";
					}
					counterSbox++;
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

		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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
}

//===== function Compute Component functions: CF(S) =========
void ReadFromFileComputeCFSboxCPU(string filename)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Open output file for saving data 
	myfile.open(outfile_exampl, ios::out | ios::app);
	//cout << "\n";

	int counter = 0, parameter[3], counterSbox = 0, counterStart = 0;
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
						CPU_CF = (int*)malloc(sizeof(int) * sizeSbox);
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
					counterStart = 0; //counter for verification start point from one S-box
					
					myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " CF(S)\n";

					//start low resolution timer
					start_comp = clock();
					//start high resolution timer
					getStartTimeComputation();

					/////////////////////////////////////////////////////////////////////////////////////////////////
					for (counterStart = 0; counterStart < sizeSbox; counterStart++)
					{
						//Function: Compute Component functions: CF(S)
						//GenTTComponentFuncVec(SboxElemet, counterStart);
						GenTTComponentFuncVec(counterStart, SboxElemet, CPU_CF, sizeSbox);

						for(int i=0; i<sizeSbox; i++)
						{ 
							myfile << CPU_CF[i] << " ";
						}
						myfile << "\n";
					}
					/////////////////////////////////////////////////////////////////////////////////////////////////

					//End high resolution timer
					EndTimeComputationPrint();

					//@@End low resolution timer
					end_comp = clock();
					elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
					//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
					elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
					//@@ =======================

					counterSbox++;
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

		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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
	free(CPU_CF);
}