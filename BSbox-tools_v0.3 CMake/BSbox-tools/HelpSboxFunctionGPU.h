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

////============ function for binary convert ========================
//void binary1(int number, int *binary_num) {
//	int w = number, c, k, i = 0;
//	for (c = binary - 1; c >= 0; c--)
//	{
//		k = w >> c;
//
//		if (k & 1)
//		{
//			binary_num[i] = 1;
//			i++;
//		}
//		else
//		{
//			binary_num[i] = 0;
//			i++;
//		}
//	}
//}
////=================================================================

////============ Set STT file for Sbox ==============================
//void SetSTT(int *SboxElemet, int **STT, int *binary_num)
//{
//	int elementS = 0;
//	for (int j = 0; j<sizeSbox; j++)
//	{
//		elementS = SboxElemet[j];
//		binary1(elementS, binary_num);
//
//		for (int i = 0; i<binary; i++)
//		{
//			STT[i][j] = binary_num[i];
//			//cout << STT[i][j] << " ";
//		}
//		//cout << "\n";
//	}
//}
////=================================================================

//===== function compute Linear Approximation Table, Lin(S) and NL(S) =========
void ReadFromFileComputeNlSboxGPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	LAT = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input S-box device vector
	cudaStatus = cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

	//CF and LAT of S-box device vector
	cudaStatus = cudaMalloc((void**)&device_CF, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

	cudaStatus = cudaMalloc((void**)&device_LAT, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

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

		//timer cuda
		cudaEvent_t start_gpu_all, stop_gpu_all, startEvent_cudaMemcpy, stopEvent_cudaMemcpy;
		float elapsedTime_gpu_all = 0, elapsedTime_gpu_all_part = 0, time_cudaMemcpy = 0, time_cudaMemcpy_all = 0;

		checkCuda(cudaEventCreate(&start_gpu_all));
		checkCuda(cudaEventCreate(&stop_gpu_all));

		checkCuda(cudaEventCreate(&startEvent_cudaMemcpy));
		checkCuda(cudaEventCreate(&stopEvent_cudaMemcpy));

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t;// , start_comp, end_comp;

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

						free(LAT);
						LAT = (int*)malloc(sizeof(int) * sizeSbox * sizeSbox);

						sizeB = sizeSbox;

						//Declaration and Allocate memory blocks 
						cudaFree(device_Sbox);
						cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeSbox);

						if (sizeSbox <= BLOCK_SIZE)
						{
							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox * sizeSbox;

							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_LAT);

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_LAT, sizeArray);
						}
						else
						{
							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_LAT);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox;

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_LAT, sizeArray);
						}
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}
					/////////////////////////////////////////////////////////////////////////////
					//Compute Linear Approximation Table (LAT) - Linearity (Lin(S))
					/////////////////////////////////////////////////////////////////////////////
					counterStart = 0;

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy S-box input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Sbox, SboxElemet, sizeof(int) * sizeSbox, cudaMemcpyHostToDevice);
					if (cudaStatus != cudaSuccess) {
						fprintf(stderr, "cudaMemcpy failed!");
						exit(EXIT_FAILURE);
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//menuChoice for compute Linear Approximation Table of S-box LAT(S)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library LAT(S) function
						//WalshSpecTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_LAT, sizeSbox, false);
						//WalshSpecTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_LAT, sizeSbox, false);
						LATSboxGPU_v03(device_Sbox, device_CF, device_LAT, sizeSbox); //v0.3
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================					
						if (sizeSbox < BLOCK_SIZE)
						{
							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(LAT, device_LAT, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " LAT(S)\n";

							//for save LAT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << LAT[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
						}
						counterSbox++;
					}
					//menuChoice for compute Lin(S)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library WST(S) function
						//int Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_LAT, sizeSbox, true);
						int Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_LAT, sizeSbox, true);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " \nLin(S):" << Lin_gpu << "\n";
						counterSbox++;
					}
					//menuChoice for compute Walsh Spectrum Table WS(S) and Lin(S) of S-box
					if (menuChoice == 2)
					{
						int Lin_gpu = 0; //max variable
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library WST(S) function
						//int Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_LAT, sizeSbox, true);
						//int Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_LAT, sizeSbox, true);
						WalshSpecTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_LAT, sizeSbox, false);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================					
						if (sizeSbox < BLOCK_SIZE)
						{
							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(LAT, device_LAT, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							//@@Start timer on kernel
							cudaEventRecord(start_gpu_all, 0);

							//Return Lin(S) value	
							//cudaMemset for device memory
							cudaMemset(device_LAT, 0, sizeSbox * sizeof(int)); //clear first row of LAT !!!
							//use Max Butterfly return Lin of the S-box
							Lin_gpu = Butterfly_max_kernel(sizeSbox * sizeSbox, device_LAT);

							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
							//========================================================

							myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " WS(S)\n";

							//for save LAT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << LAT[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
							myfile << " \nLin(S):" << Lin_gpu << "\n";
						}
						
						counterSbox++;
					}
					//menuChoice for compute nl(S)
					if (menuChoice == 3)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library WST(S) function
						//int Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_LAT, sizeSbox, true);
						int Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_LAT, sizeSbox, true);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						int nl_cpu = sizeSbox / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
						myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \nnl(S):" << nl_cpu << "\n";
						counterSbox++;
					}
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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterSbox);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterSbox);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterSbox) + (time_cudaMemcpy_all / counterSbox));

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);
		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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
	free(LAT);

	//@Free device memory
	cudaFree(device_Sbox);
	cudaFree(device_CF);
	cudaFree(device_LAT);
}

//===== function compute Autocorrelation Spectra, AC(S) =========
void ReadFromFileComputeACSboxGPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	ACT = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input S-box device vector
	cudaStatus = cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_Sbox ");
		exit(EXIT_FAILURE);
	}

	//CF and ACT of S-box device vector
	cudaStatus = cudaMalloc((void**)&device_CF, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_CF ");
		exit(EXIT_FAILURE);
	}

	cudaStatus = cudaMalloc((void**)&device_ACT, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_ACT ");
		exit(EXIT_FAILURE);
	}

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

		//timer cuda
		cudaEvent_t start_gpu_all, stop_gpu_all, startEvent_cudaMemcpy, stopEvent_cudaMemcpy;
		float elapsedTime_gpu_all = 0, elapsedTime_gpu_all_part = 0, time_cudaMemcpy = 0, time_cudaMemcpy_all = 0;

		checkCuda(cudaEventCreate(&start_gpu_all));
		checkCuda(cudaEventCreate(&stop_gpu_all));

		checkCuda(cudaEventCreate(&startEvent_cudaMemcpy));
		checkCuda(cudaEventCreate(&stopEvent_cudaMemcpy));

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t;// , start_comp, end_comp;

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

						free(ACT);
						ACT = (int*)malloc(sizeof(int) * sizeSbox * sizeSbox);

						sizeB = sizeSbox;

						//Declaration and Allocate memory blocks 
						cudaFree(device_Sbox);
						cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeSbox);

						if (sizeSbox <= BLOCK_SIZE)
						{
							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox * sizeSbox;

							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_ACT);

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_ACT, sizeArray);
						}
						else
						{
							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_ACT);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox;

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_ACT, sizeArray);
						}
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}
					/////////////////////////////////////////////////////////////////////////////
					//Compute Autocorrelation Transform (ACT) - Autocorrelation (AC)
					/////////////////////////////////////////////////////////////////////////////
					counterStart = 0;

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy S-box input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Sbox, SboxElemet, sizeof(int) * sizeSbox, cudaMemcpyHostToDevice);
					if (cudaStatus != cudaSuccess) {
						fprintf(stderr, "cudaMemcpy failed!");
						exit(EXIT_FAILURE);
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//menuChoice for compute Autocorrelation Spectra of S-box ACT(S)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ACT(S) function
						AutocorrelationTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_ACT, sizeSbox, false);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================					
						if (sizeSbox < BLOCK_SIZE)
						{
							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(ACT, device_ACT, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " ACT(S)\n";

							//for save ACT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << ACT[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
						}
						counterSbox++;
					}
					//menuChoice for compute AC(S)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ACT(S) function
						//int AC_gpu = AutocorrelationTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_ACT, sizeSbox, true);
						int AC_gpu = AutocorrelationTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_ACT, sizeSbox, true);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \nAC(S):" << AC_gpu << " ";
						counterSbox++;
					}
					//menuChoice for compute Autocorrelation Spectra ACT(S) and AC(S) of S-box
					if (menuChoice == 2)
					{
						int AC_gpu = 0; //max variable
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ACT(S) function
						//int AC_gpu = AutocorrelationTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_ACT, sizeSbox, true);
						//Function: Call BoolSPLG Library ACT(S) function
						AutocorrelationTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_ACT, sizeSbox, false);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================					
						if (sizeSbox < BLOCK_SIZE)
						{
							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(ACT, device_ACT, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							//@@Start timer on kernel
							cudaEventRecord(start_gpu_all, 0);

							//Return AC(S) value						
							cudaMemset(device_ACT, 0, sizeSbox * sizeof(int)); //clear first row of ACT !!!
							//use Max Butterfly return ACT of the S-box
							AC_gpu = Butterfly_max_kernel(sizeSbox * sizeSbox, device_ACT);

							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
							//==================================================

							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " ACT(S)\n";

							//for save ACT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << ACT[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
							myfile << " \nAC(S):" << AC_gpu << " ";
						}					
						counterSbox++;
					}
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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file


		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterSbox);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterSbox);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterSbox) + (time_cudaMemcpy_all / counterSbox));

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);
		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);


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
	free(ACT);

	//@Free device memory
	cudaFree(device_Sbox);
	cudaFree(device_CF);
	cudaFree(device_ACT);
}

//===== function compute TT(S)->ANF(S) or ANF(S) -> TT(S) and deg(S) =========
void ReadFromFileComputeANF_degSboxGPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	ANF = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input S-box device vector
	cudaStatus = cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_Sbox ");
		exit(EXIT_FAILURE);
	}

	//CF and ACT of S-box device vector
	cudaStatus = cudaMalloc((void**)&device_CF, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_CF ");
		exit(EXIT_FAILURE);
	}

	cudaStatus = cudaMalloc((void**)&device_ANF, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_ACT ");
		exit(EXIT_FAILURE);
	}

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

		//timer cuda
		cudaEvent_t start_gpu_all, stop_gpu_all, startEvent_cudaMemcpy, stopEvent_cudaMemcpy;
		float elapsedTime_gpu_all = 0, elapsedTime_gpu_all_part = 0, time_cudaMemcpy = 0, time_cudaMemcpy_all = 0;

		checkCuda(cudaEventCreate(&start_gpu_all));
		checkCuda(cudaEventCreate(&stop_gpu_all));

		checkCuda(cudaEventCreate(&startEvent_cudaMemcpy));
		checkCuda(cudaEventCreate(&stopEvent_cudaMemcpy));

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t;// , start_comp, end_comp;

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

						free(ANF);
						ANF = (int*)malloc(sizeof(int) * sizeSbox * sizeSbox);

						sizeB = sizeSbox;

						//Declaration and Allocate memory blocks 
						cudaFree(device_Sbox);
						cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeSbox);

						if (sizeSbox <= BLOCK_SIZE)
						{
							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox * sizeSbox;

							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_ANF);

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_ANF, sizeArray);
						}
						else
						{
							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_ANF);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox;

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_ANF, sizeArray);
						}
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}
					/////////////////////////////////////////////////////////////////////////////
					//Compute Algebraic Normal Form (ANF) - Algebraic Degree (deg(S))
					/////////////////////////////////////////////////////////////////////////////
					counterStart = 0;

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy S-box input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Sbox, SboxElemet, sizeof(int) * sizeSbox, cudaMemcpyHostToDevice);
					if (cudaStatus != cudaSuccess) {
						fprintf(stderr, "cudaMemcpy failed!");
						exit(EXIT_FAILURE);
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//menuChoice for compute Algebraic Normal Form of S-box ANF(S)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Mobius Transformation function CPU (ANF(f))
						MobiusTranSboxGPU(device_Sbox, device_CF, device_ANF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================					
						if (sizeSbox < BLOCK_SIZE)
						{
							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(ANF, device_ANF, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " ANF(S)\n";

							//for save ACT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << ANF[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
						}
						counterSbox++;
					}
					//menuChoice for compute Algebraic Degree Table of S-box ADT(S)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Mobius Transformation function CPU (ANF(f))					
						//MobiusTranSboxGPU(device_Sbox, device_CF, device_ANF, sizeSbox);
						AlgebraicDegreeTableSboxGPU(device_Sbox, device_CF, device_ANF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================					
						if (sizeSbox < BLOCK_SIZE)
						{
							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(ANF, device_ANF, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " ADT(S)\n";

							//for save ACT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << ANF[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
						}
						counterSbox++;
					}


					//menuChoice for compute (max) deg(S)
					if (menuChoice == 2)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ANF(S) function
						int ADmax_gpu = AlgebraicDegreeSboxGPU_ButterflyMax(device_Sbox, device_CF, device_ANF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(max)deg(S):" << ADmax_gpu << "\n";
						counterSbox++;
					}
					//menuChoice for compute (min) deg(S)
					if (menuChoice == 3)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ANF(S) function
						int ADmin_gpu = AlgebraicDegreeSboxGPU_ButterflyMin(device_Sbox, device_CF, device_ANF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(min)deg(S):" << ADmin_gpu << "\n";
						counterSbox++;
					}

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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file


		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterSbox);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterSbox);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterSbox) + (time_cudaMemcpy_all / counterSbox));

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);
		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);


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
	free(ANF);

	//@Free device memory
	cudaFree(device_Sbox);
	cudaFree(device_CF);
	cudaFree(device_ANF);
}

//===== function compute (bitwise TT(S)->ANF(S) or ANF(S) -> TT(S) and deg(S) =========
void ReadFromFileComputeBitwiseANF_degSboxGPU(string filename, int menuChoice)
{
	int NumOfBits = sizeof(unsigned long long int) * 8, NumInt;

	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	host_CF = (int*)malloc(sizeof(int) * sizeB);
	//host_max_values = (int*)malloc(sizeof(int) * sizeB);
	host_NumIntVecCF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input S-box device vector
	cudaStatus = cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_Sbox ");
		exit(EXIT_FAILURE);
	}

	//@Declaration and Alocation of memory blocks
	int sizeArray = sizeof(unsigned long long int) * sizeB;

	//CF and LAT of S-box device vector
	cudaStatus = cudaMalloc((void**)&device_NumIntVecCF, sizeArray);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

	cudaStatus = cudaMalloc((void**)&device_NumIntVecANF, sizeArray);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

	cudaStatus = cudaMalloc((void**)&device_Vec_max_values, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

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

		//timer cuda
		cudaEvent_t start_gpu_all, stop_gpu_all, startEvent_cudaMemcpy, stopEvent_cudaMemcpy;
		float elapsedTime_gpu_all = 0, elapsedTime_gpu_all_part = 0, time_cudaMemcpy = 0, time_cudaMemcpy_all = 0;

		checkCuda(cudaEventCreate(&start_gpu_all));
		checkCuda(cudaEventCreate(&stop_gpu_all));

		checkCuda(cudaEventCreate(&startEvent_cudaMemcpy));
		checkCuda(cudaEventCreate(&stopEvent_cudaMemcpy));

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t;// , start_comp, end_comp;

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

						//Declaration and Allocate memory blocks 
						cudaFree(device_Sbox);
						cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeSbox);

						/////////////////////////////////////////////////////////////////////////////////////////////////////
						if (sizeSbox < 16384) //limitation cîme from function Butterfly_max_min_kernel, where can fit 2^26/64 numbers
						{
							//Used paramether for bitwise computations
							NumInt = (sizeSbox * sizeSbox) / NumOfBits;

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(unsigned long long int) * NumInt;

							//@Free memory
							free(host_CF);
							//free(host_max_values);
							free(host_NumIntVecCF);

							host_CF = (int*)malloc(sizeof(int) * sizeSbox * sizeSbox);
							//host_max_values = (int*)malloc(sizeof(int) * NumInt);
							host_NumIntVecCF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);

							//@Free cuda memory
							cudaFree(device_NumIntVecCF);
							cudaFree(device_NumIntVecANF);
							cudaFree(device_Vec_max_values);

							//CF and other S-box device vector
							cudaMalloc((void**)&device_NumIntVecCF, sizeArray);
							cudaMalloc((void**)&device_NumIntVecANF, sizeArray);
							cudaMalloc((void**)&device_Vec_max_values, sizeof(int) * NumInt);

							//@Declaration and Alocation of memory blocks - v0.3
							int sizeArray_v03 = sizeof(int) * sizeSbox * sizeSbox;

							cudaMalloc((void**)&device_CF, sizeArray_v03);
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////////
						if ((sizeSbox > 8192) & (sizeSbox < 131072)) //limitation cîme from function Butterfly_max_min_kernel, where can fit 2^26/64 numbers
						{
							//Used paramether for bitwise computations
							NumInt = sizeSbox / NumOfBits;
							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(unsigned long long int) * NumInt * sizeSbox;

							//@Free cuda memory
							cudaFree(device_NumIntVecCF);
							cudaFree(device_NumIntVecANF);
							cudaFree(device_Vec_max_values);

							cudaMalloc((void**)&device_NumIntVecCF, sizeArray);
							cudaMalloc((void**)&device_NumIntVecANF, sizeArray);
							cudaMalloc((void**)&device_Vec_max_values, sizeof(int)* NumInt* sizeSbox);

							//@Declaration and Alocation of memory blocks - v0.3
							int sizeArray_v03 = sizeof(int) * sizeSbox;

							cudaMalloc((void**)&device_CF, sizeArray_v03);

						}
						/////////////////////////////////////////////////////////////////////////////////////////////////////
						if (sizeSbox >= 131072)
						{
							//Used paramether for bitwise computations
							NumInt = sizeSbox / NumOfBits;
							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(unsigned long long int) * NumInt;

							////@Free memory
							//free(host_CF);
							//free(host_max_values);
							//free(host_NumIntVecCF);

							//host_CF = (int*)malloc(sizeof(int) * sizeSbox);
							//host_max_values = (int*)malloc(sizeof(int) * NumInt);
							//host_NumIntVecCF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);

							//CF and other S-box device vector
							cudaMalloc((void**)&device_NumIntVecCF, sizeArray);
							cudaMalloc((void**)&device_NumIntVecANF, sizeArray);
							cudaMalloc((void**)&device_Vec_max_values, sizeof(int) * sizeArray);

							//@Declaration and Alocation of memory blocks - v0.3
							int sizeArray_v03 = sizeof(int) * sizeSbox;

							cudaMalloc((void**)&device_CF, sizeArray_v03);
						}
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}
					/////////////////////////////////////////////////////////////////////////////
					//Compute Algebraic Normal Form (ANF) - Algebraic Degree (deg(S)) Bitwise
					/////////////////////////////////////////////////////////////////////////////
					counterStart = 0;

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy S-box input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Sbox, SboxElemet, sizeof(int) * sizeSbox, cudaMemcpyHostToDevice);
					if (cudaStatus != cudaSuccess) {
						fprintf(stderr, "cudaMemcpy failed!");
						exit(EXIT_FAILURE);
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//menuChoice for compute bitwise Algebraic Normal Form of S-box ANF(S)
					if (menuChoice == 0)
					{
						//@@ =======================					
						if (sizeSbox < 16384)
						{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Mobius Transformation function GPU (ANF(f))
						BitwiseMobiusTranSboxGPU(SboxElemet, host_CF, host_NumIntVecCF, device_NumIntVecCF, device_NumIntVecANF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(host_NumIntVecCF, device_NumIntVecANF, sizeof(unsigned long long int) * NumInt, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							DecVecToBin(NumOfBits, host_CF, host_NumIntVecCF, NumInt);

							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " ANF(S)\n";

							//for save ACT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << host_CF[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
						}
						else
						{
							cout << "\nIs not implemented this funcionality for S-box size n>14. \nThe output data is to big.\n";
						}
						counterSbox++;
					}
					//menuChoice for compute bitwise (max) deg(S)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ANF(S) function Bitwise
						//int ADmax_bitwise_gpu = BitwiseAlgebraicDegreeSboxGPU_ButterflyMax(SboxElemet, host_CF, host_max_values, host_NumIntVecCF, device_NumIntVecCF, device_NumIntVecANF, device_Vec_max_values, sizeSbox);
						//int ADmax_bitwise_gpu = BitwiseAlgebraicDegreeSboxGPU_ButterflyMax_v03(SboxElemet, host_CF, host_max_values, host_NumIntVecCF, device_NumIntVecCF, device_NumIntVecANF, device_Vec_max_values, device_Sbox, device_CF, sizeSbox);
						int ADmax_bitwise_gpu = BitwiseAlgebraicDegreeSboxGPU_ButterflyMax_v03(device_NumIntVecCF, device_NumIntVecANF, device_Vec_max_values, device_Sbox, device_CF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(min)deg(S):" << ADmax_bitwise_gpu << " ";
						counterSbox++;
					}
					//menuChoice for compute bitwise (min) deg(S)
					if (menuChoice == 2)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ANF(S) function Bitwise
						int ADmin_bitwise_gpu = BitwiseAlgebraicDegreeSboxGPU_ButterflyMin_v03(device_NumIntVecCF, device_NumIntVecANF, device_Vec_max_values, device_Sbox, device_CF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n(min)deg(S):" << ADmin_bitwise_gpu << " ";
						counterSbox++;
					}
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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file


		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterSbox);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterSbox);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterSbox) + (time_cudaMemcpy_all / counterSbox));

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);
		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);


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
	//@Free memory
	cudaFree(device_NumIntVecCF);
	cudaFree(device_NumIntVecANF);
	cudaFree(device_Vec_max_values);
	cudaFree(device_CF);

	free(host_CF);
	//free(host_max_values);
	free(host_NumIntVecCF);
}

//===== function compute DDT(S) and delta(S) =========
void ReadFromFileComputeDDT_deltaSboxGPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	DDT = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input S-box device vector
	cudaStatus = cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_Sbox ");
		exit(EXIT_FAILURE);
	}

	cudaStatus = cudaMalloc((void**)&device_DDT, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed! device_ACT ");
		exit(EXIT_FAILURE);
	}

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

		//timer cuda
		cudaEvent_t start_gpu_all, stop_gpu_all, startEvent_cudaMemcpy, stopEvent_cudaMemcpy;
		float elapsedTime_gpu_all = 0, elapsedTime_gpu_all_part = 0, time_cudaMemcpy = 0, time_cudaMemcpy_all = 0;

		checkCuda(cudaEventCreate(&start_gpu_all));
		checkCuda(cudaEventCreate(&stop_gpu_all));

		checkCuda(cudaEventCreate(&startEvent_cudaMemcpy));
		checkCuda(cudaEventCreate(&stopEvent_cudaMemcpy));

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t;// , start_comp, end_comp;

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

						free(DDT);
						DDT = (int*)malloc(sizeof(int) * sizeSbox * sizeSbox);

						sizeB = sizeSbox;

						//Declaration and Allocate memory blocks 
						cudaFree(device_Sbox);
						cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeSbox);

						//if (sizeSbox <= BLOCK_SIZE)
						if (sizeSbox <= 16384)
						{
							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox * sizeSbox;

							//@Free device memory
							cudaFree(device_DDT);

							//@@Allocate Device memory
							cudaMalloc((void**)&device_DDT, sizeArray);
						}
						else
						{
							//@Free device memory
							cudaFree(device_DDT);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox;

							//@@Allocate Device memory
							cudaMalloc((void**)&device_DDT, sizeArray);
						}
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}
					/////////////////////////////////////////////////////////////////////////////
					//Compute Difference Distribution Table (DDT) - Differential uniformity (delta)
					/////////////////////////////////////////////////////////////////////////////
					counterStart = 0;

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy S-box input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Sbox, SboxElemet, sizeof(int) * sizeSbox, cudaMemcpyHostToDevice);
					if (cudaStatus != cudaSuccess) {
						fprintf(stderr, "cudaMemcpy failed!");
						exit(EXIT_FAILURE);
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//menuChoice for compute Difference distribution table of S-box DDT(S)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library DDT(S) function
						//DDTSboxGPU_ButterflyMax(device_Sbox, device_DDT, sizeSbox, false);
						DDTSboxGPU_ButterflyMax_v03_expand(device_Sbox, device_DDT, sizeSbox, false);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================					
						if (sizeSbox < BLOCK_SIZE)
						{
							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(DDT, device_DDT, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " DDT(S)\n";

							//for save ACT in file
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								for (int jj = 0; jj < sizeSbox; jj++)
								{
									myfile << DDT[counterStart] << " ";
									counterStart++;
								}
								myfile << "\n";
							}
						}
						counterSbox++;
					}
					//menuChoice for compute delta(S)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library DDT(S) function
						//int Diff_gpu = DDTSboxGPU_ButterflyMax(device_Sbox, device_DDT, sizeSbox, true);
						//int Diff_gpu = DDTSboxGPU_ButterflyMax_v03_expand(device_Sbox, device_DDT, sizeSbox, true);
						int delta_gpu = DDTSboxGPU_ButterflyMax_v03_expand(device_Sbox, device_DDT, sizeSbox, true);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================
						myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \ndelta(S):" << delta_gpu << " ";
						counterSbox++;
					}
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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file


		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterSbox);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterSbox);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterSbox) + (time_cudaMemcpy_all / counterSbox));

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);
		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);


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
	free(DDT);

	//@Free device memory
	cudaFree(device_Sbox);
	cudaFree(device_DDT);
}

//===== function Compute Component functions: CF(S) =========
void ReadFromFileComputeCFSboxGPU(string filename)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	//Declaration and Allocate memory blocks 
	CF = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input S-box device vector
	cudaStatus = cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

	//CF and LAT of S-box device vector
	cudaStatus = cudaMalloc((void**)&device_CF, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}


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

		//timer cuda
		cudaEvent_t start_gpu_all, stop_gpu_all, startEvent_cudaMemcpy, stopEvent_cudaMemcpy;
		float elapsedTime_gpu_all = 0, elapsedTime_gpu_all_part = 0, time_cudaMemcpy = 0, time_cudaMemcpy_all = 0;

		checkCuda(cudaEventCreate(&start_gpu_all));
		checkCuda(cudaEventCreate(&stop_gpu_all));

		checkCuda(cudaEventCreate(&startEvent_cudaMemcpy));
		checkCuda(cudaEventCreate(&stopEvent_cudaMemcpy));

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t;// , start_comp, end_comp;

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

						//Declaration and Allocate memory blocks 
						cudaFree(device_Sbox);
						cudaMalloc((void**)&device_Sbox, sizeof(int) * sizeSbox);

						if (sizeSbox <= BLOCK_SIZE)
						{
							free(CF);
							CF = (int*)malloc(sizeof(int) * sizeSbox * sizeSbox);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox * sizeSbox;

							//@Free device memory
							cudaFree(device_CF);

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
						}
						else
						{
							free(CF);
							CF = (int*)malloc(sizeof(int) * sizeSbox);

							//@Free device memory
							cudaFree(device_CF);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox;

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
						}
					}
					//=====================================
					for (int i = 0; i < sizeSbox; i++)
					{
						fin >> str;
						//copy from file to host memory
						SboxElemet[i] = atoi(str.c_str());
					}
					/////////////////////////////////////////////////////////////////////////////
					//Compute Component function GPU
					/////////////////////////////////////////////////////////////////////////////
					counterStart = 0;

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy S-box input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Sbox, SboxElemet, sizeof(int) * sizeSbox, cudaMemcpyHostToDevice);
					if (cudaStatus != cudaSuccess) {
						fprintf(stderr, "cudaMemcpy failed!");
						exit(EXIT_FAILURE);
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//menuChoice for compute Linear Approximation Table of S-box
					if (sizeSbox <= BLOCK_SIZE)
					{
						//@set GRID
						int sizethread = sizeSbox;
						int sizeblok = sizeSbox;

						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library component function (CF) of S-box
						ComponentFnAll_kernel << <sizeblok, sizethread >> > (device_Sbox, device_CF, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy output vector from GPU buffer to host memory.
						cudaStatus = cudaMemcpy(CF, device_CF, sizeof(int) * sizeSbox * sizeSbox, cudaMemcpyDeviceToHost);
						if (cudaStatus != cudaSuccess) {
							fprintf(stderr, "cudaMemcpy failed!");
							exit(EXIT_FAILURE);
						}

						//@@End timer on cudaMemcpy
						checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
						checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
						checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
						time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

						myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " CF(S)\n";

						//for save LAT in file
						for (int ii = 0; ii < sizeSbox; ii++)
						{
							for (int jj = 0; jj < sizeSbox; jj++)
							{
								myfile << CF[counterStart] << " ";
								counterStart++;
							}
							myfile << "\n";
						}
						counterSbox++;
					}
					//menuChoice for compute Lin(S)
					else
					{
						//@set GRID
						int sizethread = BLOCK_SIZE;
						int sizeblok = sizeSbox / BLOCK_SIZE;

						myfile << "\n? " << "1 " << sizeSbox << " " << binary - 1 << " CF(S)\n";

						for (int i = 0; i < sizeSbox; i++)
						{

							//@@Start timer on kernel
							cudaEventRecord(start_gpu_all, 0);

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@Compute Component function GPU - BoolSPLG Library function
							ComponentFnVec_kernel << <sizeblok, sizethread >> > (device_Sbox, device_CF, i);
							/////////////////////////////////////////////////////////////////////////////////////////////////

							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							// Copy output vector from GPU buffer to host memory.
							cudaStatus = cudaMemcpy(CF, device_CF, sizeof(int) * sizeSbox, cudaMemcpyDeviceToHost);
							if (cudaStatus != cudaSuccess) {
								fprintf(stderr, "cudaMemcpy failed!");
								exit(EXIT_FAILURE);
							}

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							for (int jj = 0; jj < sizeSbox; jj++)
							{
								myfile << CF[jj] << " ";
							}
							myfile << "\n";
						}
						//@@ =======================
						counterSbox++;
					}
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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterSbox);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterSbox);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterSbox) + (time_cudaMemcpy_all / counterSbox));

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);
		printf("\n\nNumber of compute S-box(es) function: %d \n\n", counterSbox);

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
	free(CF);

	//@Free device memory
	cudaFree(device_Sbox);
	cudaFree(device_CF);
}