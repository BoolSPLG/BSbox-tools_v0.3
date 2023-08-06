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


//===== function compute Lin(S), nl(S), deg(S), AC(S) and delta(S) =========
void FindReadFromFileComputeSboxGPU(string filename, int menuChoice, int nl_gpuBorder, int AC_gpuBorder, int deg_gpuBorder, int delta_gpuBorder)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

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

	cudaStatus = cudaMalloc((void**)&device_ACT, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
		exit(EXIT_FAILURE);
	}

	cudaStatus = cudaMalloc((void**)&device_ANF, sizeof(int) * sizeB);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaMalloc failed!");
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

	int counter = 0, parameter[3], counterSbox = 0, counterBoundary = 0, Lin_gpu = 0, AC_gpu = 0, degMax_gpu = 0, delta_gpu = 0;
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

					int Lin_gpuBorder = sizeSbox - 2 * nl_gpuBorder; //Lin(f) = 2 ^ n - 2 * nl(f))
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
							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox * sizeSbox;

							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_LAT);
							cudaFree(device_ACT);
							cudaFree(device_ANF);
							cudaFree(device_DDT);

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_LAT, sizeArray);
							cudaMalloc((void**)&device_ACT, sizeArray);
							cudaMalloc((void**)&device_ANF, sizeArray);
							cudaMalloc((void**)&device_DDT, sizeArray);
						}
						else
						{
							//@Free device memory
							cudaFree(device_CF);
							cudaFree(device_LAT);
							cudaFree(device_ACT);
							cudaFree(device_ANF);
							cudaFree(device_DDT);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(int) * sizeSbox;

							//@@Allocate Device memory
							cudaMalloc((void**)&device_CF, sizeArray);
							cudaMalloc((void**)&device_LAT, sizeArray);
							cudaMalloc((void**)&device_ACT, sizeArray);
							cudaMalloc((void**)&device_ANF, sizeArray);
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
					//Start GPU program
					/////////////////////////////////////////////////////////////////////////////
					CheckSizeSbox(sizeSbox);

					counterSbox++;

					Lin_gpu = 0, AC_gpu = 0, degMax_gpu = 0, delta_gpu = 0;

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

					//menuChoice to find Lin(S), nl(S)
					if (menuChoice == 0)
					{
						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function GPU (W_f(S)), compute Lin(S), nl(S)
						/////////////////////////////////////////////////////////////////////////////////////////////////
						if (sizeSbox <= BLOCK_SIZE)
						{
							//@set GRID
							sizethread = sizeSbox;
							sizeblok = sizeSbox;

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@Start timer on kernels
							cudaEventRecord(start_gpu_all, 0);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@Compute Component function GPU
							ComponentFnAll_kernel_PTT << <sizeblok, sizethread >> > (device_Sbox, device_CF, sizeSbox);

							//@Compute LAT of S-box
							fwt_kernel_shfl_xor_SM << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_CF, device_LAT, sizethread);

							cudaMemset(device_LAT, 0, sizeSbox * sizeof(int)); //clear first row of LAT !!!

							//use Max Butterfly return Lin of the S-box
							Lin_gpu = Butterfly_max_kernel(sizeSbox * sizeSbox, device_LAT);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (Lin_gpu <= Lin_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								int nl_gpu = sizeSbox / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nLin(S):" << Lin_gpu << " nl(S):" << nl_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						else
						{
							//set GRID
							sizethread = BLOCK_SIZE;
							sizeblok = sizeSbox / BLOCK_SIZE;

							for (int i = 1; i < sizeSbox; i++)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@Start timer on kernels
								cudaEventRecord(start_gpu_all, 0);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@Compute Component function GPU
								ComponentFnVec_kernel_PTT << <sizeblok, sizethread >> > (device_Sbox, device_CF, i);
								int Lin_return = WalshSpecTranBoolGPU_ButterflyMax(device_CF, device_LAT, sizeSbox, true);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@End timer on kernels
								cudaEventRecord(stop_gpu_all, 0);
								cudaEventSynchronize(stop_gpu_all);
								cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
								elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (Lin_gpu < Lin_return)
									Lin_gpu = Lin_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (Lin_gpu > Lin_gpuBorder)
								{
									break;
								}
								/////////////////////////////////////////////////////////////////////////////////////////////////							
							}
							if (Lin_gpu <= Lin_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								int nl_gpu = sizeSbox / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nLin(S):" << Lin_gpu << " nl(S):" << nl_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////	
					}

					//menuChoice to find AC(S)
					if (menuChoice == 1)
					{
						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Autocorrelation Transformation function GPU (ACT(S)), compute AC(S)
						/////////////////////////////////////////////////////////////////////////////////////////////////
						if (sizeSbox <= BLOCK_SIZE)
						{
							//@set GRID
							sizethread = sizeSbox;
							sizeblok = sizeSbox;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@Start timer on kernels
							cudaEventRecord(start_gpu_all, 0);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@Compute Component function GPU
							ComponentFnAll_kernel_PTT << <sizeblok, sizethread >> > (device_Sbox, device_CF, sizeSbox);

							//@Compute ACT of S-box
							fwt_kernel_shfl_xor_SM << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_CF, device_ACT, sizethread);
							powInt << < sizeblok, sizethread >> > (device_ACT, 2);
							ifmt_kernel_shfl_xor_SM_Sbox << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_ACT, device_ACT, sizethread);

							cudaMemset(device_ACT, 0, sizeSbox * sizeof(int)); //clear first row of LAT !!!

							//use Max Butterfly return Lin of the S-box
							AC_gpu = Butterfly_max_kernel(sizeSbox * sizeSbox, device_ACT);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@End timer on kernels
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (AC_gpu <= AC_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nAC(S):" << AC_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						else
						{
							//set GRID
							sizethread = BLOCK_SIZE;
							sizeblok = sizeSbox / BLOCK_SIZE;

							for (int i = 1; i < sizeSbox; i++)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@Start timer on kernel
								cudaEventRecord(start_gpu_all, 0);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@Compute Component function GPU
								ComponentFnVec_kernel_PTT << <sizeblok, sizethread >> > (device_Sbox, device_CF, i);
								//@Return AC(S) of component function of the S-box
								int AC_return = AutocorrelationTranBoolGPU_ButterflyMax(device_CF, device_ACT, sizeSbox, true);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@End timer on kernel
								cudaEventRecord(stop_gpu_all, 0);
								cudaEventSynchronize(stop_gpu_all);
								cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
								elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (AC_gpu < AC_return)
									AC_gpu = AC_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (AC_gpu > AC_gpuBorder)
								{
									break;
								}
								/////////////////////////////////////////////////////////////////////////////////////////////////							
							}
							if (AC_gpu <= AC_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nAC(S):" << AC_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}

					//menuChoice to find deg(S)
					if (menuChoice == 2)
					{

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Mobius Transformation function GPU (ANF(f)), compute deg(S)
						/////////////////////////////////////////////////////////////////////////////////////////////////
						if (sizeSbox <= BLOCK_SIZE)
						{
							//@set GRID
							sizethread = sizeSbox;
							sizeblok = sizeSbox;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@Start timer on kernel
							cudaEventRecord(start_gpu_all, 0);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@Compute Component function GPU
							ComponentFnAll_kernel << <sizeblok, sizethread >> > (device_Sbox, device_CF, sizeSbox);

							//@Compute ANF of S-box
							fmt_kernel_shfl_xor_SM << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_CF, device_ANF, sizethread);
							kernel_AD_Sbox << <sizeblok, sizethread >> > (device_ANF);

							cudaMemset(device_ANF, 0, sizeSbox * sizeof(int)); //clear first row of ANF !!!

							//use Max Butterfly return Lin of the S-box
							degMax_gpu = Butterfly_max_kernel(sizeSbox * sizeSbox, device_ANF);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMax_gpu > deg_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\ndeg(S):" << degMax_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						else
						{
							//set GRID
							sizethread = BLOCK_SIZE;
							sizeblok = sizeSbox / BLOCK_SIZE;

							for (int i = 1; i < sizeSbox; i++)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@Start timer on kernel
								cudaEventRecord(start_gpu_all, 0);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@Compute Component function GPU
								ComponentFnVec_kernel << <sizeblok, sizethread >> > (device_Sbox, device_CF, i);

								//@Function return deg(S) of component function of the S-box
								int degMax_return = AlgebraicDegreeBoolGPU_ButterflyMax(device_CF, device_ANF, sizeSbox);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@End timer on kernel
								cudaEventRecord(stop_gpu_all, 0);
								cudaEventSynchronize(stop_gpu_all);
								cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
								elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (degMax_gpu < degMax_return)
									degMax_gpu = degMax_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (degMax_gpu < deg_gpuBorder)
								{
									break;
								}
								/////////////////////////////////////////////////////////////////////////////////////////////////							
							}
							if (degMax_gpu > deg_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\ndeg(S):" << degMax_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}

					//menuChoice to find delta(S)
					if (menuChoice == 3)
					{
						///////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Compute Differential uniformity, delta(S)
						///////////////////////////////////////////////////////////////////////////////////////////////////
						if (sizeSbox <= BLOCK_SIZE)
						{
							//@set GRID
							sizethread = sizeSbox;
							sizeblok = sizeSbox;

							cudaMemset(device_DDT, 0, sizeSbox * sizeSbox * sizeof(int));

							///////////////////////////////////////////////////////////////////////////////////////////////////
							//@@Start timer on kernel
							cudaEventRecord(start_gpu_all, 0);
							///////////////////////////////////////////////////////////////////////////////////////////////////

							//@Compute Differential uniformity 
							DDTFnAll_kernel << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_Sbox, device_DDT, sizeSbox);

							cudaMemset(device_DDT, 0, sizeSbox * sizeof(int)); //clear first row of LAT !!!

							//use Max Butterfly return delta of the S-box
							delta_gpu = Butterfly_max_kernel(sizeSbox * sizeSbox, device_DDT);
							///////////////////////////////////////////////////////////////////////////////////////////////////
							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (delta_gpu <= delta_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\ndelta(S):" << delta_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						else
						{
							//set GRID
							sizethread = BLOCK_SIZE;
							sizeblok = sizeSbox / BLOCK_SIZE;

							for (int i = 1; i < sizeSbox; i++)
							{
								cudaMemset(device_DDT, 0, sizeSbox * sizeof(int)); //clear DDT !!!

								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@Start timer on kernel
								cudaEventRecord(start_gpu_all, 0);
								/////////////////////////////////////////////////////////////////////////////////////////////////

								//@Compute Differential uniformity 
								DDTFnVec_kernel << <sizeblok, sizethread >> > (device_Sbox, device_DDT, i);

								int delta_return = Butterfly_max_kernel(sizeSbox, device_DDT); //use Max Butterfly return delta of the S-box component function
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@End timer on kernel
								cudaEventRecord(stop_gpu_all, 0);
								cudaEventSynchronize(stop_gpu_all);
								cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
								elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (delta_gpu < delta_return)
									delta_gpu = delta_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if (delta_gpu > delta_gpuBorder)
								{
									break;
								}
								/////////////////////////////////////////////////////////////////////////////////////////////////							
							}
							if (delta_gpu < delta_gpuBorder)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\ndelta(S):" << delta_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////
					}

					//menuChoice to find Lin(S), nl(S), AC(S), deg(S), delta(S)
					if (menuChoice == 4)
					{
						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Compute Lin(S), nl(S), AC(S), deg(S) and delta(S)
						/////////////////////////////////////////////////////////////////////////////////////////////////
						if (sizeSbox <= BLOCK_SIZE)
						{
							//@set GRID
							sizethread = sizeSbox;
							sizeblok = sizeSbox;

							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@Start timer on kernel
							cudaEventRecord(start_gpu_all, 0);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Walsh Transformation function GPU (W_f(S)), compute Lin(S), nl(S)
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_LAT, sizeSbox, true);
							Lin_gpu = WalshSpecTranSboxGPU_ButterflyMax_v03(device_Sbox, device_CF, device_LAT, sizeSbox, true); //v0.3
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Autocorrelation Transformation function GPU (ACT(S)), compute AC(S)
							/////////////////////////////////////////////////////////////////////////////////////////////////
							AC_gpu = AutocorrelationTranSboxGPU_ButterflyMax(device_Sbox, device_CF, device_ACT, sizeSbox, true);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: Fast Mobius Transformation function GPU (ANF(f)), compute deg(S)
							/////////////////////////////////////////////////////////////////////////////////////////////////
							degMax_gpu = AlgebraicDegreeSboxGPU_ButterflyMax(device_Sbox, device_CF, device_ANF, sizeSbox);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//Function: GPU (DDT(f)), compute delta(S)
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//delta_gpu = DDTSboxGPU_ButterflyMax(device_Sbox, device_DDT, sizeSbox, true);
							delta_gpu = DDTSboxGPU_ButterflyMax_v03(device_Sbox, device_DDT, sizeSbox, true);
							/////////////////////////////////////////////////////////////////////////////////////////////////
							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if ((Lin_gpu <= Lin_gpuBorder) & (AC_gpu <= AC_gpuBorder) & (degMax_gpu > deg_gpuBorder) & (delta_gpu <= delta_gpuBorder))
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								int nl_gpu = sizeSbox / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nLin(S):" << Lin_gpu << " nl(S):" << nl_gpu << " AC(S):" << AC_gpu << " deg(S):" << degMax_gpu << " delta(S):" << delta_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
						else
						{
							//set GRID
							sizethread = BLOCK_SIZE;
							sizeblok = sizeSbox / BLOCK_SIZE;

							for (int i = 1; i < sizeSbox; i++)
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@Start timer on kernel
								cudaEventRecord(start_gpu_all, 0);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@Compute Component function GPU
								/////////////////////////////////////////////////////////////////////////////////////////////////
								ComponentFnVec_kernel_PTT << <sizeblok, sizethread >> > (device_Sbox, device_CF, i);
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//Function: Fast Walsh Transformation function GPU (W_f(S)), compute Lin(S), nl(S)
								/////////////////////////////////////////////////////////////////////////////////////////////////
								int Lin_return = WalshSpecTranBoolGPU_ButterflyMax(device_CF, device_LAT, sizeSbox, true);

								if (Lin_gpu < Lin_return)
									Lin_gpu = Lin_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//Function: Autocorrelation Transformation function GPU (ACT(S)), compute AC(S)
								/////////////////////////////////////////////////////////////////////////////////////////////////
								int AC_return = AutocorrelationTranBoolGPU_ButterflyMax(device_CF, device_ACT, sizeSbox, true);

								if (AC_gpu < AC_return)
									AC_gpu = AC_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//Function: Fast Mobius Transformation function GPU (ANF(f)), compute deg(S)
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@Compute Component function GPU - true table
								ComponentFnVec_kernel << <sizeblok, sizethread >> > (device_Sbox, device_CF, i);

								int degMax_return = AlgebraicDegreeBoolGPU_ButterflyMax(device_CF, device_ANF, sizeSbox);

								if (degMax_gpu < degMax_return)
									degMax_gpu = degMax_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//Function: Differential uniformity (DDT(f)), compute delta(S)
								/////////////////////////////////////////////////////////////////////////////////////////////////
								cudaMemset(device_DDT, 0, sizeSbox * sizeof(int)); //clear DDT !!!
								DDTFnVec_kernel << <sizeblok, sizethread >> > (device_Sbox, device_DDT, i);
								int delta_return = Butterfly_max_kernel(sizeSbox, device_DDT); //use Max Butterfly return delta of the S-box component function

								if (delta_gpu < delta_return)
									delta_gpu = delta_return;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								//@@End timer on kernel
								cudaEventRecord(stop_gpu_all, 0);
								cudaEventSynchronize(stop_gpu_all);
								cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
								elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								if ((Lin_gpu > Lin_gpuBorder) | (AC_gpu > AC_gpuBorder) | (degMax_gpu < deg_gpuBorder) | (delta_gpu > delta_gpuBorder))
								{
									break;
								}
								/////////////////////////////////////////////////////////////////////////////////////////////////							
							}
							if ((Lin_gpu <= Lin_gpuBorder) & (AC_gpu <= AC_gpuBorder) & (degMax_gpu > deg_gpuBorder) & (delta_gpu < delta_gpuBorder))
							{
								/////////////////////////////////////////////////////////////////////////////////////////////////
								int nl_gpu = sizeSbox / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
								myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
								for (int ii = 0; ii < sizeSbox; ii++)
								{
									myfile << SboxElemet[ii] << " ";
								}
								myfile << "\nLin(S):" << Lin_gpu << " nl(S):" << nl_gpu << " AC(S):" << AC_gpu << " deg(S):" << degMax_gpu << " delta(S):" << delta_gpu;
								/////////////////////////////////////////////////////////////////////////////////////////////////
								counterBoundary++;
							}
						}
					}
					/////////////////////////////////////////////////////////////////////////////////////////////////////////
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
		printf("Number of compute S-box(es) function with specific parameters: %d \n\n", counterBoundary);

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

	cudaFree(device_CF);
	cudaFree(device_LAT);
	cudaFree(device_ACT);
	cudaFree(device_ANF);
	cudaFree(device_DDT);
}

//===== function Find (bitwise) deg(S) =========
void FindReadFromFileComputeBitwiseANF_degSboxGPU(string filename, int deg_cpuBorder)
{
	int NumOfBits = sizeof(unsigned long long int) * 8, NumInt;

	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeB); //Allocate memory for permutation

	host_CF = (int*)malloc(sizeof(int) * sizeB);
	host_max_values = (int*)malloc(sizeof(int) * sizeB);
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

	int counter = 0, parameter[3], counterSbox = 0, counterBoundary = 0;
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

						if (sizeSbox < 16384)
						{
							NumInt = (sizeSbox * sizeSbox) / NumOfBits;

							//@Free memory
							free(host_CF);
							free(host_max_values);
							free(host_NumIntVecCF);

							host_CF = (int*)malloc(sizeof(int) * sizeSbox * sizeSbox);
							host_max_values = (int*)malloc(sizeof(int) * NumInt);
							host_NumIntVecCF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(unsigned long long int) * NumInt;

							//@Free cuda memory
							cudaFree(device_NumIntVecCF);
							cudaFree(device_NumIntVecANF);
							cudaFree(device_Vec_max_values);

							//CF and other S-box device vector
							cudaMalloc((void**)&device_NumIntVecCF, sizeArray);
							cudaMalloc((void**)&device_NumIntVecANF, sizeArray);
							cudaMalloc((void**)&device_Vec_max_values, sizeof(int) * NumInt);
						}
						else
						{
							NumInt = sizeSbox / NumOfBits;

							//@Free memory
							free(host_CF);
							free(host_max_values);
							free(host_NumIntVecCF);

							host_CF = (int*)malloc(sizeof(int) * sizeSbox);
							host_max_values = (int*)malloc(sizeof(int) * NumInt);
							host_NumIntVecCF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);

							//@Declaration and Alocation of memory blocks
							int sizeArray = sizeof(unsigned long long int) * NumInt;

							//CF and other S-box device vector
							cudaMalloc((void**)&device_NumIntVecCF, sizeArray);
							cudaMalloc((void**)&device_NumIntVecANF, sizeArray);
							cudaMalloc((void**)&device_Vec_max_values, sizeof(int) * sizeArray);
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
					//Compute Algebraic Degree (deg(S)) Bitwise
					/////////////////////////////////////////////////////////////////////////////
					counterSbox++;
					int degMax_gpu = 0;

					if (sizeSbox < 16384)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Call BoolSPLG Library ANF(S) function Bitwise
						degMax_gpu = BitwiseAlgebraicDegreeSboxGPU_ButterflyMax(SboxElemet, host_CF, host_max_values, host_NumIntVecCF, device_NumIntVecCF, device_NumIntVecANF, device_Vec_max_values, sizeSbox);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						/////////////////////////////////////////////////////////////////////////////////////////////////
						if (degMax_gpu > deg_gpuBorder)
						{
							/////////////////////////////////////////////////////////////////////////////////////////////////
							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << SboxElemet[ii] << " ";
							}
							myfile << "\ndeg(S):" << degMax_gpu;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							counterBoundary++;
						}
					}
					else
					{
						//bitwise variable
						NumOfBits = sizeof(unsigned long long int) * 8;
						NumInt = sizeSbox / NumOfBits;

						for (int i = 0; i < sizeSbox; i++)
						{
							///////////////////////////////////////////////////////////////////////////////////////////////
							//@Compute Component function CPU
							GenTTComponentFuncVec(i, SboxElemet, host_CF, sizeSbox);
							///////////////////////////////////////////////////////////////////////////////////////////////
							//convert bool into integers
							BinVecToDec(NumOfBits, host_CF, host_NumIntVecCF, NumInt);
							///////////////////////////////////////////////////////////////////////////////////////////////

							//@@Start timer on cudaMemcpy
							checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

							cudaMemcpy(device_NumIntVecCF, host_NumIntVecCF, sizeof(unsigned long long int) * NumInt, cudaMemcpyHostToDevice);

							//@@End timer on cudaMemcpy
							checkCuda(cudaEventRecord(stopEvent_cudaMemcpy, 0));
							checkCuda(cudaEventSynchronize(stopEvent_cudaMemcpy));
							checkCuda(cudaEventElapsedTime(&time_cudaMemcpy, startEvent_cudaMemcpy, stopEvent_cudaMemcpy));
							time_cudaMemcpy_all = time_cudaMemcpy_all + time_cudaMemcpy;

							//@@Start timer on kernel
							cudaEventRecord(start_gpu_all, 0);

							//////////////////////////////////////////////////////////////////////////////////////////////////
							//Call BoolSPLG Library FMT(f) function - Algebraic Degree calculation
							int degMax_return = BitwiseAlgebraicDegreeBoolGPU_ButterflyMax(device_NumIntVecCF, device_NumIntVecANF, device_Vec_max_values, host_max_values, sizeSbox);

							//@@End timer on kernel
							cudaEventRecord(stop_gpu_all, 0);
							cudaEventSynchronize(stop_gpu_all);
							cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
							elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

							if (degMax_return > degMax_gpu)
								degMax_gpu = degMax_return;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							if (degMax_gpu < deg_gpuBorder)
							{
								break;
							}
						}
						/////////////////////////////////////////////////////////////////////////////////////////////////							
						if (degMax_gpu > deg_gpuBorder)
						{
							/////////////////////////////////////////////////////////////////////////////////////////////////
							myfile << "\n\n? " << "1 " << sizeSbox << " " << binary - 1 << " \n";
							for (int ii = 0; ii < sizeSbox; ii++)
							{
								myfile << SboxElemet[ii] << " ";
							}
							myfile << "\ndeg(S):" << degMax_gpu;
							/////////////////////////////////////////////////////////////////////////////////////////////////
							counterBoundary++;
						}
					}
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
		//@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file



		if (sizeSbox < 16384)
		{
			printf("\n  Elapsed time (GPU computation + cudaMemcpy): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
			printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms) \n", elapsedTime_gpu_all / counterSbox);
		}
		else
		{
			printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
			printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterSbox);
			//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
			printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
			printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterSbox);
			printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterSbox) + (time_cudaMemcpy_all / counterSbox));
		}

		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);
		printf("\n\nNumber of compute S-box(es) function: %d \n", counterSbox);
		printf("Number of compute S-box(es) function with specific parameters: %d \n\n", counterBoundary);

		//@@set old value of size
		binary = 0;
		sizeB = 0;
		counterSbox = 0;
		counterBoundary = 0;
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

	free(host_CF);
	free(host_max_values);
	free(host_NumIntVecCF);
}