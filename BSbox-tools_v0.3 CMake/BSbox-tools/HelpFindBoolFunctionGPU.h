//Help Heder file "HelpFindBoolFunction.h" - Find GPU computing Boolean functions properties and other functions
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

//===== function compute Lin, nl, deg, AC =========
void FindReadFromFileComputeBooleanGPU(string filename, int menuChoice, int nl_gpuBorder, int AC_gpuBorder, int deg_gpuBorder)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	host_Bool_PTT = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input device vector - boolean function
	cudaStatus = cudaMalloc((void**)&device_Bool_TT, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		//goto Error;
		exit(EXIT_FAILURE);
	}

	//input device vector - boolean function
	cudaStatus = cudaMalloc((void**)&device_Bool_PTT, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		//goto Error;
		exit(EXIT_FAILURE);
	}

	//output device vector
	cudaStatus = cudaMalloc((void**)&device_Bool_rez, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		//goto Error;
		exit(EXIT_FAILURE);
	}


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

		//timer cuda
		cudaEvent_t start_gpu_all, stop_gpu_all, startEvent_cudaMemcpy, stopEvent_cudaMemcpy;
		float elapsedTime_gpu_all = 0, elapsedTime_gpu_all_part = 0, time_cudaMemcpy = 0, time_cudaMemcpy_all = 0;

		checkCuda(cudaEventCreate(&start_gpu_all));
		checkCuda(cudaEventCreate(&stop_gpu_all));

		checkCuda(cudaEventCreate(&startEvent_cudaMemcpy));
		checkCuda(cudaEventCreate(&stopEvent_cudaMemcpy));

		//@@Define Timer start Read and write @@time.h 
		clock_t start_t, end_t;
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

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						host_Bool_PTT = (int*)malloc(sizeof(int) * sizeBool);

						sizeB = sizeBool;

						//@Free device memory
						cudaFree(device_Bool_TT);
						cudaFree(device_Bool_PTT);
						cudaFree(device_Bool_rez);

						//@@Allocate Device memory
						cudaMalloc((void**)&device_Bool_TT, sizeof(int) * sizeBool);
						cudaMalloc((void**)&device_Bool_PTT, sizeof(int) * sizeBool);
						cudaMalloc((void**)&device_Bool_rez, sizeof(int) * sizeBool);

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
					//Start GPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

					if (menuChoice == 2 || menuChoice == 3)
					{
						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy input vectors from host memory to GPU buffers.
						cudaStatus = cudaMemcpy(device_Bool_TT, host_Bool_TT, sizeof(int) * sizeBool, cudaMemcpyHostToDevice);
						if (cudaStatus != cudaSuccess) {
							fprintf(stderr, "cudaMemcpy failed!");
							//goto Error;
							exit(EXIT_FAILURE);
						}

						//@@End timer on cudaMemcpy
						checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
						checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
						checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
						time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;
					}

					if (menuChoice == 0 || menuChoice == 1 || menuChoice == 3)
					{
						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy input vectors from host memory to GPU buffers.
						cudaStatus = cudaMemcpy(device_Bool_PTT, host_Bool_PTT, sizeof(int) * sizeBool, cudaMemcpyHostToDevice);
						if (cudaStatus != cudaSuccess) {
							fprintf(stderr, "cudaMemcpy failed!");
							//goto Error;
							exit(EXIT_FAILURE);
						}
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//menuChoice for compute NL(f)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function GPU (W_f(f))
						int Lin_gpu = WalshSpecTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeB, true); //use Butterfly Max
						int nl_gpu = sizeBool / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						//@@ =======================

						if (nl_gpu >= nl_gpuBorder)
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}

							myfile << "\nnl(f):" << nl_gpu << " ";
							counterBoundary++;
						}
						counterBoolean++;
					}

					//menuChoice for compute AC(f)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////
						//Function: BoolSPLG Library Autocorrelation GPU (r_f(f))
						int AC_gpu = AutocorrelationTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeB, true); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
						//@@ =======================

						if (AC_gpu >= AC_gpuBorder)
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}

							myfile << "\nAC(f):" << AC_gpu << " ";
							counterBoundary++;
						}
						counterBoolean++;
					}

					//menuChoice for compute deg(f)
					if (menuChoice == 2)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Call BoolSPLG Library FMT(f) function -  Algebraic Degree calculation
						int degMax_gpu = AlgebraicDegreeBoolGPU_ButterflyMax(device_Bool_TT, device_Bool_rez, sizeB); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
						//@@ =======================

						if (degMax_gpu >= deg_gpuBorder)
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}
							myfile << "\ndeg(f):" << degMax_gpu << " ";
							counterBoundary++;
						}
						counterBoolean++;
					}
					//menuChoice for compute NL(f), AC(f), deg(f)
					if (menuChoice == 3)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function GPU (W_f(f))
						int Lin_gpu = WalshSpecTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeB, true); //use Butterfly Max
						int nl_gpu = sizeBool / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: BoolSPLG Library Autocorrelation GPU (r_f(f))
						int AC_gpu = AutocorrelationTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeB, true); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Call BoolSPLG Library FMT(f) function - Algebraic Degree calculation
						int degMax_gpu = AlgebraicDegreeBoolGPU_ButterflyMax(device_Bool_TT, device_Bool_rez, sizeB); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
						//@@ =======================

						cout << nl_gpu << " " << AC_gpu << " " << degMax_gpu << " ";

						if ((nl_gpu >= nl_gpuBorder) & (AC_gpu <= AC_gpuBorder) & (degMax_gpu >= deg_gpuBorder))
						{
							myfile << "\n\n? " << "0 " << sizeBool << "\n";
							for (int ii = 0; ii < sizeBool; ii++)
							{
								myfile << host_Bool_TT[ii] << " ";
							}

							myfile << "\nnl(f):" << nl_gpu << " AC(f):" << AC_gpu << " deg(f):" << degMax_gpu;
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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterBoolean);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterBoolean);

		printf("  Average time spend for GPU computation+cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterBoolean) + (time_cudaMemcpy_all / counterBoolean));


		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n", counterBoolean);
		printf("Number of compute Boolean(s) function with specific parameter(s): %d \n\n", counterBoundary);

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

	cudaFree(device_Bool_TT);
	cudaFree(device_Bool_PTT);
	cudaFree(device_Bool_rez);
}
//============================================================================

//===== function Find (Bitwise) deg(f) =========
void FindReadFromFileComputeBitWise_ANF_degGPU(string filename, int deg_gpuBorder)
{

	//bitwise variable
	int NumOfBits = sizeof(unsigned long long int) * 8, NumInt;

	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);

	//Allocate memory block for bitwise computation
	NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);

	host_max_values_AD = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input integer TT device vector
	cudaStatus = cudaMalloc((void**)&device_NumIntVecTT, sizeof(unsigned long long int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		//goto Error;
		exit(EXIT_FAILURE);
	}

	//output integer ANF device vector
	cudaStatus = cudaMalloc((void**)&device_NumIntVecANF, sizeof(unsigned long long int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		//goto Error;
		exit(EXIT_FAILURE);
	}

	//device max AD on every integer
	cudaStatus = cudaMalloc((void**)&device_max_values_AD, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		//goto Error;
		exit(EXIT_FAILURE);
	}

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

				//@@check is boolean function
				if ((parameter[0] == 0) & (parameter[1] > 32) & (parameter[1] <= 33554432)) // border 2^25 element input
				{
					sizeBool = parameter[1];

					//Recreate dinamic vector for a diffrent size
					if (sizeBool != sizeB)
					{
						NumInt = sizeBool / NumOfBits;

						//@@free host memory
						free(host_Bool_TT);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);

						sizeB = sizeBool;

						//@Free device memory
						cudaFree(device_NumIntVecTT);
						cudaFree(device_NumIntVecANF);
						cudaFree(device_max_values_AD);

						//@@Allocate Device memory
						cudaMalloc((void**)&device_NumIntVecTT, sizeof(unsigned long long int) * NumInt);
						cudaMalloc((void**)&device_NumIntVecANF, sizeof(unsigned long long int) * NumInt);
						cudaMalloc((void**)&device_max_values_AD, sizeof(int) * NumInt);

						//bitwise additional memory
						free(NumIntVecTT);

						//Allocate memory block for bitwise computation
						NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);

						//bitwise additional host memory
						if (NumInt <= 256)
						{
							free(host_max_values_AD);

							host_max_values_AD = (int*)malloc(sizeof(int) * NumInt);
						}
					}
					//=====================================

					for (int i = 0; i < sizeBool; i++)
					{
						fin >> str;

						//copy from file to host memory
						host_Bool_TT[i] = atoi(str.c_str());
						//cout << str << " ";
					}
					//////////////////////////////////////////////////////////////////////////////////////////////////
					//Start GPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

						//set TT in 64 bit integer array
					BinVecToDec(NumOfBits, host_Bool_TT, NumIntVecTT, NumInt);

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_NumIntVecTT, NumIntVecTT, sizeof(unsigned long long int) * NumInt, cudaMemcpyHostToDevice);
					if (cudaStatus != cudaSuccess) {
						fprintf(stderr, "cudaMemcpy failed!");
						//goto Error;
						exit(EXIT_FAILURE);
					}

					//@@End timer on cudaMemcpy
					checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
					checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
					checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
					time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

					//@@Start timer on kernel
					cudaEventRecord(start_gpu_all, 0);

					//////////////////////////////////////////////////////////////////////////////////////////////////
					//Call BoolSPLG Library FMT(f) function - Algebraic Degree calculation
					int degMax_bitwise_gpu = BitwiseAlgebraicDegreeBoolGPU_ButterflyMax(device_NumIntVecTT, device_NumIntVecANF, device_max_values_AD, host_max_values_AD, sizeB);
					//////////////////////////////////////////////////////////////////////////////////////////////////
					cout << degMax_bitwise_gpu << " ";
					//@@End timer on kernel
					cudaEventRecord(stop_gpu_all, 0);
					cudaEventSynchronize(stop_gpu_all);
					cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
					elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
					//@@ =======================


					if (degMax_bitwise_gpu >= deg_gpuBorder)
					{
						myfile << "\n\n? " << "0 " << sizeBool << "\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << host_Bool_TT[ii] << " ";
						}

						myfile << "\ndeg(f):" << degMax_bitwise_gpu << " ";
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

		////@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterBoolean);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterBoolean);

		printf("  Average time spend for GPU computation+cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterBoolean) + (time_cudaMemcpy_all / counterBoolean));

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n", counterBoolean);
		printf("Number of compute Boolean(s) function with specific parameter(s): %d \n\n", counterBoundary);

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
	free(NumIntVecTT);
	free(host_max_values_AD);

	cudaFree(device_NumIntVecTT);
	cudaFree(device_NumIntVecANF);
	cudaFree(device_max_values_AD);
}
//============================================================================