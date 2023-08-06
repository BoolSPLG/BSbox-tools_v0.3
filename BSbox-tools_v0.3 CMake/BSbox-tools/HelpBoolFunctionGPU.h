//Help Heder file "HelpBoolFunction.h" - GPU computing Boolean functions properties and other functions
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
void ReadFromFileComputeNlGPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_PTT = (int*)malloc(sizeof(int) * sizeB);
	walshvec_cpu = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

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

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
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
						//@@free host memory
						free(host_Bool_PTT);
						free(walshvec_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_PTT = (int*)malloc(sizeof(int) * sizeBool);
						walshvec_cpu = (int*)malloc(sizeof(int) * sizeBool);

						//@Free device memory
						cudaFree(device_Bool_PTT);
						cudaFree(device_Bool_rez);

						//@@Allocate Device memory
						cudaMalloc((void**)&device_Bool_PTT, sizeof(int) * sizeBool);
						cudaMalloc((void**)&device_Bool_rez, sizeof(int) * sizeBool);

						sizeB = sizeBool;
					}
					//=====================================

					for (int i = 0; i < sizeBool; i++)
					{
						fin >> str;

						//copy from file to host memory
						host_Bool_PTT[i] = atoi(str.c_str());
						//cout << str << " ";

						//cout << Element[counter] << " ";
						if (host_Bool_PTT[i] == 0)
							host_Bool_PTT[i] = 1;
						else
							host_Bool_PTT[i] = -1;

						//	cout << host_Bool_PTT[i] << " ";
					}

					/////////////////////////////////////////////////////////////////////////////
					//Start GPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Bool_PTT, host_Bool_PTT, sizeof(int) * sizeBool, cudaMemcpyHostToDevice);
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

					//menuChoice for compute Walsh spectar WS(f)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function GPU (W_f(f))

						//@ Set grid
						setgrid(sizeB);

						/////////////////////////////////////////////////////
						//fwt_kernel_shfl_xor_SM << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_Bool, device_Bool_rez, sizethread);
						//if (sizeB > 1024)
						//	fwt_kernel_shfl_xor_SM_MP << < sizeblok, sizethread, sizethread * sizeof(int) >> > (device_Bool_rez, sizeblok, sizeblok1);
						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Call BoolSPLG Library, Boolean Fast Walsh Transform: return Walsh Spectra and Lin(f)
						WalshSpecTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeBool, false); //use Butterfly Max
						//////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy output vector from GPU buffer to host memory.
						cudaStatus = cudaMemcpy(walshvec_cpu, device_Bool_rez, sizeof(int) * sizeBool, cudaMemcpyDeviceToHost);
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

						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function GPU (W_f(f))
						int Lin_gpu = WalshSpecTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeB, true); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						myfile << "\n? " << "0 " << sizeBool << "\nLin(f):" << Lin_gpu << "\n";
						counterBoolean++;
					}
					//menuChoice for compute Walsh spectar WS(f) and Lin(f)
					if (menuChoice == 2)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function GPU (W_f(f))

						//Lin_gpu = WalshSpecTranBoolGPU(device_Vect, device_Vect_rez, size, true); //use reduction Max
						int Lin_gpu = WalshSpecTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeB, true); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy output vector from GPU buffer to host memory.
						cudaStatus = cudaMemcpy(walshvec_cpu, device_Bool_rez, sizeof(int) * sizeBool, cudaMemcpyDeviceToHost);
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

						myfile << "\n? " << "0 " << sizeBool << " WS(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << walshvec_cpu[ii] << " ";
						}
						myfile << "\nLin(f):" << Lin_gpu << "\n";
						counterBoolean++;
					}

					//menuChoice for compute nl
					if (menuChoice == 3)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: Fast Walsh Transformation function GPU (W_f(f))
						//Lin_gpu = WalshSpecTranBoolGPU(device_Vect, device_Vect_rez, size, true); //use reduction Max
						int Lin_gpu = WalshSpecTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeB, true); //use Butterfly Max
						int nl_gpu = sizeBool / 2 - (Lin_gpu / 2);		//Compute Nonlinearity
						//////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel ********************************
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						myfile << "\n? " << "0 " << sizeBool << "\nnl(f):" << nl_gpu << "\n";

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

		//@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterBoolean);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterBoolean);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterBoolean) + (time_cudaMemcpy_all / counterBoolean));

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
	free(host_Bool_PTT);
	free(walshvec_cpu);

	//@Free device memory
	cudaFree(device_Bool_PTT);
	cudaFree(device_Bool_rez);
}
//============================================================================

//===== function compute Altocorelation spectra, AC =========
void ReadFromFileComputeACGPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	rf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

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

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
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

		//============================================================================
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

						//@Free device memory
						cudaFree(device_Bool_PTT);
						cudaFree(device_Bool_rez);

						//@@Allocate Device memory
						cudaMalloc((void**)&device_Bool_PTT, sizeof(int) * sizeBool);
						cudaMalloc((void**)&device_Bool_rez, sizeof(int) * sizeBool);

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
					//Start GPU program
					/////////////////////////////////////////////////////////////////////////////

					//@@Start timer on cudaMemcpy
					checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

					// Copy input vectors from host memory to GPU buffers.
					cudaStatus = cudaMemcpy(device_Bool_PTT, host_Bool_TT, sizeof(int) * sizeBool, cudaMemcpyHostToDevice);
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

					//menuChoice for compute Autocorrelation spectar AS(f)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Call Functions: Compute Autocorrelation GPU (r_f(f))

						//@ Set grid
						setgrid(sizeB);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//fwt_kernel_shfl_xor_SM << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_Bool, device_Bool_rez, sizethread);
						//if (sizeB > 1024)
						//	fwt_kernel_shfl_xor_SM_MP << < sizeblok, sizethread, sizethread * sizeof(int) >> > (device_Bool_rez, sizeblok, sizeblok1);
						//powInt << < sizeblok, sizethread >> > (device_Bool_rez, 2);
						//ifmt_kernel_shfl_xor_SM << <sizeblok, sizethread, sizethread * sizeof(int) >> > (device_Bool_rez, device_Bool_rez, sizethread);
						//if (sizeB > 1024)
						//	ifmt_kernel_shfl_xor_SM_MP << < sizeblok, sizethread, sizethread * sizeof(int) >> > (device_Bool_rez, sizeblok, sizeblok1);
						//////////////////////////////////////////////////////////////////////////////////////////////////
						//Call BoolSPLG Library, Boolean Autocorrelation Transform function ACT: return Autocorrelation spectra and AC(f)
						AutocorrelationTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeBool, false); //not return AC(f)
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy output vector from GPU buffer to host memory.
						cudaStatus = cudaMemcpy(rf_cpu, device_Bool_rez, sizeof(int) * sizeBool, cudaMemcpyDeviceToHost);
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

						myfile << "\n? " << "0 " << sizeBool << " AS(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << rf_cpu[ii] << " ";
						}
						myfile << "\n";
						counterBoolean++;
					}

					//menuChoice for compute AC(f)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: BoolSPLG Library Autocorrelation GPU (r_f(f)) - two diffrent functions for FWT(f) calculation
						//int AC_gpu = AutocorrelationTranBoolGPU(device_Bool, device_Bool_rez, size, true); //use reduction Max
						int AC_gpu = AutocorrelationTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeBool, true); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						myfile << "\n? " << "0 " << sizeBool << "\nAC(f):" << AC_gpu << "\n";

						counterBoolean++;
					}

					//menuChoice for compute Autocorrelation spectar AS(f) and AC(f)
					if (menuChoice == 2)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						/////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: BoolSPLG Library Autocorrelation GPU (r_f(f)) - two diffrent functions for FWT(f) calculation
						//int AC_gpu = AutocorrelationTranBoolGPU(device_Bool, device_Bool_rez, size, true); //use reduction Max
						int AC_gpu = AutocorrelationTranBoolGPU_ButterflyMax(device_Bool_PTT, device_Bool_rez, sizeBool, true); //use Butterfly Max
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy output vector from GPU buffer to host memory.
						cudaStatus = cudaMemcpy(rf_cpu, device_Bool_rez, sizeof(int) * sizeBool, cudaMemcpyDeviceToHost);
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

						myfile << "\n? " << "0 " << sizeBool << " AS(f)\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << rf_cpu[ii] << " ";
						}
						myfile << "\nAC:" << AC_gpu << "\n";
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

		//@@End Timer Read and write @@time.h 
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;

		myfile.close(); // close the open file

		printf("\n  Elapsed time (only GPU computation): %f (ms),  (%f seconds) \n", elapsedTime_gpu_all, elapsedTime_gpu_all / 1000.0);
		printf("  Average time per function (only GPU computation): %f (ms) \n", elapsedTime_gpu_all / counterBoolean);
		//printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\n  Time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all);
		printf("  Average time spend for cudaMemcpy: %f (ms)\n", time_cudaMemcpy_all / counterBoolean);

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterBoolean) + (time_cudaMemcpy_all / counterBoolean));

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

	//@Free device memory
	cudaFree(device_Bool_PTT);
	cudaFree(device_Bool_rez);
}
//============================================================================

//===== function compute TT(f)->ANF(f) or ANF(f) -> TT(f) and deg(f) =========
void ReadFromFileComputeANF_degGPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//check CUDA component status
	cudaError_t cudaStatus;

	//input device vector - boolean function
	cudaStatus = cudaMalloc((void**)&device_Bool_TT, sizeof(int) * sizeB);
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

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
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
						//@@free host memory
						free(host_Bool_TT);
						free(anf_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						anf_cpu = (int*)malloc(sizeof(int) * sizeBool);

						//@Free device memory
						cudaFree(device_Bool_TT);
						cudaFree(device_Bool_rez);

						//@@Allocate Device memory
						cudaMalloc((void**)&device_Bool_TT, sizeof(int) * sizeBool);
						cudaMalloc((void**)&device_Bool_rez, sizeof(int) * sizeBool);

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
					//Start GPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

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

					//menuChoice for compute TT(f)->ANF(f) or ANF(f)->TT(f)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						//////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: BoolSPLG Library FMT(f)
						//////////////////////////////////////////////////////////////////////////////////////////////////
						MobiusTranBoolGPU(device_Bool_TT, device_Bool_rez, sizeB);
						//////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy output vector from GPU buffer to host memory.
						cudaStatus = cudaMemcpy(anf_cpu, device_Bool_rez, sizeof(int) * sizeBool, cudaMemcpyDeviceToHost);
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

						myfile << "\n? " << "0 " << sizeBool << "\n";
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
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						//////////////////////////////////////////////////////////////////////////////////////////////////
						//Call BoolSPLG Library FMT(f) function - two diffrent functions for FMT(f) - Algebraic Degree calculation
						//////////////////////////////////////////////////////////////////////////////////////////////////
						//int degMax_gpu = AlgebraicDegreeBoolGPU(device_Bool, device_Bool_rez, size); //use reduction Max
						int degMax_gpu = AlgebraicDegreeBoolGPU_ButterflyMax(device_Bool_TT, device_Bool_rez, sizeB); //use Butterfly Max
						//////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						myfile << "\n? " << "0 " << sizeBool << "\ndeg(f):" << degMax_gpu << "\n";

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

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterBoolean) + (time_cudaMemcpy_all / counterBoolean));

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

	//@Free device memory
	cudaFree(device_Bool_TT);
	cudaFree(device_Bool_rez);
}
//============================================================================

//===== function Compute (Bitwise) ANF from TT and vice versa =========
void ReadFromFileComputeBitWise_ANFGPU(string filename, int menuChoice)
{
	//bitwise variable
	int NumOfBits = sizeof(unsigned long long int) * 8, NumInt;

	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int*)malloc(sizeof(int) * sizeB);
	anf_cpu = (int*)malloc(sizeof(int) * sizeB);

	//Allocate memory block for bitwise computation
	NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);
	NumIntVecANF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * sizeB);

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

	int counter = 0, parameter[3], counterBoolean = 0;// , counterNumSatisfBorder = 0;
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
						free(anf_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int*)malloc(sizeof(int) * sizeBool);
						anf_cpu = (int*)malloc(sizeof(int) * sizeBool);

						//@Free device memory
						cudaFree(device_NumIntVecTT);
						cudaFree(device_NumIntVecANF);
						cudaFree(device_max_values_AD);

						//@@Allocate Device memory
						cudaMalloc((void**)&device_NumIntVecTT, sizeof(unsigned long long int) * NumInt);
						cudaMalloc((void**)&device_NumIntVecANF, sizeof(unsigned long long int) * NumInt);
						cudaMalloc((void**)&device_max_values_AD, sizeof(int) * NumInt);

						sizeB = sizeBool;

						//bitwise additional memory
						free(NumIntVecTT);
						free(NumIntVecANF);

						//Allocate memory block for bitwise computation
						NumIntVecTT = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);
						NumIntVecANF = (unsigned long long int*)malloc(sizeof(unsigned long long int) * NumInt);

						//bitwise additional host memory
						if (NumInt <= 256)
						{
							free(host_max_values_AD);

							host_max_values_AD = (int*)malloc(sizeof(int) * sizeB);
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

					/////////////////////////////////////////////////////////////////////////////
					//Start GPU program
					//////////////////////////////////////////////////////////////////////////////////////////////////

					//convert bool into integers
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

					//menuChoice for compute bitwise TT(f)->ANF(f) or ANF(f)->TT(f)
					if (menuChoice == 0)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						//////////////////////////////////////////////////////////////////////////////////////////////////
						//Function: BoolSPLG Library Bitwise FMT(f)
						BitwiseMobiusTranBoolGPU(device_NumIntVecTT, device_NumIntVecANF, sizeB);
						//////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						//@@Start timer on cudaMemcpy
						checkCuda(cudaEventRecord(startEvent_cudaMemcpy, 0));

						// Copy output vector from GPU buffer to host memory.
						cudaStatus = cudaMemcpy(NumIntVecANF, device_NumIntVecANF, sizeof(unsigned long long int) * NumInt, cudaMemcpyDeviceToHost);
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

						//conversion from decimal to binary on ANF array
						DecVecToBin(NumOfBits, anf_cpu, NumIntVecANF, NumInt);

						myfile << "\n? " << "0 " << sizeBool << "\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << anf_cpu[ii] << " ";
						}
						counterBoolean++;
					}

					//menuChoice for compute deg(f)
					if (menuChoice == 1)
					{
						//@@Start timer on kernel
						cudaEventRecord(start_gpu_all, 0);

						//////////////////////////////////////////////////////////////////////////////////////////////////
						//Call BoolSPLG Library FMT(f) function - Algebraic Degree calculation
						int degMax_bitwise_gpu = BitwiseAlgebraicDegreeBoolGPU_ButterflyMax(device_NumIntVecTT, device_NumIntVecANF, device_max_values_AD, host_max_values_AD, sizeB);
						//////////////////////////////////////////////////////////////////////////////////////////////////

						//@@End timer on kernel
						cudaEventRecord(stop_gpu_all, 0);
						cudaEventSynchronize(stop_gpu_all);
						cudaEventElapsedTime(&elapsedTime_gpu_all_part, start_gpu_all, stop_gpu_all);
						elapsedTime_gpu_all = elapsedTime_gpu_all + elapsedTime_gpu_all_part;

						// cudaDeviceSynchronize waits for the kernel to finish, and returns
						// any errors encountered during the launch.
						//cudaStatus = cudaDeviceSynchronize();
						//if (cudaStatus != cudaSuccess) {
						//	fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
						//	//goto Error;
						//	exit(EXIT_FAILURE);
						//}

						myfile << "\n? " << "0 " << sizeBool << "\ndeg(f):" << degMax_bitwise_gpu << "\n";

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

		printf("  Average time spend for GPU computation + cudaMemcpy: %f (ms)\n", (elapsedTime_gpu_all / counterBoolean) + (time_cudaMemcpy_all / counterBoolean));

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

	//@Free bitwise additional memory
	free(NumIntVecTT);
	free(NumIntVecANF);
	free(host_max_values_AD);

	//@Free device memory
	cudaFree(device_NumIntVecTT);
	cudaFree(device_NumIntVecANF);
	cudaFree(device_max_values_AD);
}
//============================================================================

//===== function Compute (Bitwise) ANF from TT and vice versa =========
/*void ReadFromFileComputeBitWise_ANFCPU(string filename, int menuChoice)
{
	//Allocate memory block. Allocates a block of size bytes of memory
	host_Bool_TT = (int *)malloc(sizeof(int)* size);
	anf_cpu = (int *)malloc(sizeof(int)* size);

	//Allocate memory block for bitwise computation
	NumIntVecTT = (unsigned long long int *)malloc(sizeof(unsigned long long int)* size);
	NumIntVecANF = (unsigned long long int *)malloc(sizeof(unsigned long long int)* size);

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
					if (sizeBool != size)
					{
						//@@free host memory
						free(host_Bool_TT);
						free(anf_cpu);

						//Allocate memory block. Allocates a block of size bytes of memory
						host_Bool_TT = (int *)malloc(sizeof(int)* sizeBool);
						anf_cpu = (int *)malloc(sizeof(int)* sizeBool);

						size = sizeBool;

						//bitwise variable and memory
						NumOfBits = sizeof(unsigned long long int) * 8;
						NumInt = size / NumOfBits;

						free(NumIntVecTT);
						free(NumIntVecANF);

						NumIntVecTT = (unsigned long long int *)malloc(sizeof(unsigned long long int)* NumInt);
						NumIntVecANF = (unsigned long long int *)malloc(sizeof(unsigned long long int)* NumInt);
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

					//menuChoice for compute bitwise ANF from S-box(es)
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
						CPU_FWT_bitwise(NumIntVecTT, NumIntVecANF, NumOfBits, NumInt);
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

						myfile << "\n\n";
						myfile << "? " << "0 " << sizeBool << "\n";
						for (int ii = 0; ii < sizeBool; ii++)
						{
							myfile << anf_cpu[ii] << " ";
						}
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
						CPU_FWT_bitwise(NumIntVecTT, NumIntVecANF, NumOfBits, NumInt);
						//conversion from decimal to binary and fine deg(f)
						int degMax_cpu = DecVecToBin_maxDeg(NumOfBits, anf_cpu, NumIntVecANF, NumInt);
						/////////////////////////////////////////////////////////////////////////////////////////////////

						//End high resolution timer
						EndTimeComputationPrint();

						//@@End low resolution timer
						end_comp = clock();
						elapsedTime_part_comp = (double)(end_comp - start_comp) / CLOCKS_PER_SEC;
						//printf("==  Elapsed time: %f sec\n", elapsedTime_part_comp);
						elapsedTime_comp = elapsedTime_comp + elapsedTime_part_comp;
						//@@ =======================

						myfile << "\n\n";
						myfile << "? " << "0 " << sizeBool << "\n";
						myfile << "deg(f):" << degMax_cpu << " ";

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

		printf("\nElapsed time (only computation): %f microseconds,  (%f seconds) - high timer resolution \n", elapsedTime_computation, elapsedTime_computation / 1000000.0);
		printf("Elapsed time (only computation): %f seconds - low timer resolution \n", elapsedTime_comp);
		printf("\nAverage time per function (only computation): %f seconds \n", elapsedTime_comp / counterBoolean);

		////@@End Timer Read and write @@time.h
		end_t = clock();
		double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
		cout << "\n Time include Read and Write memory (data) from file, Allocate, Initialization and free memory\n";
		printf("==  Elapsed time: %f sec\n", elapsed_time);

		printf("\n\nNumber of compute Boolean(s) function: %d \n\n", counterBoolean);

		myfile.close(); // close the open file

		//@@set old value of size
		size = 0;
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
}*/
//============================================================================