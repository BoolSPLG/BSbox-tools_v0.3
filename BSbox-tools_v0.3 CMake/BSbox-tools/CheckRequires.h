//Header file for check the Minimal Hardware Requires

//@@System includes
#include <iostream>
#include <stdio.h>

//@@CUDA runtime
#include "cuda_runtime.h"

using namespace std;

void BoolSPLGMinimalRequires1()
{
	//check CUDA component status
	cudaError_t cudaStatus;

	//cudaError_t error;
	cudaDeviceProp devProp;
	// Number of CUDA devices
	int devCount;
	cudaStatus = cudaGetDeviceCount(&devCount);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaGetDeviceCount failed!  Do you have a CUDA-capable GPU installed?");

		//set CPU menu =========
		menuCPU();
		//======================

		exit(EXIT_SUCCESS);
		//return;
	}

	// Iterate through devices
	for (int i = 0; i < devCount; ++i)
	{
		// Get device properties 
		// This check all possible CUDA capable device.
		// cudaDeviceProp devPropMR;

		cudaStatus = cudaGetDeviceProperties(&devProp, i);
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaGetDeviceProperties failed! Do you have a CUDA-capable GPU installed?");

			//set CPU menu =========
			menuCPU();
			//======================

			exit(EXIT_SUCCESS);
			//return;
		}

		printf("\n Minimal requires compute capability 3.0 or later to run the BSbox-tools (GPU)\n");

		//	printf("\nFulfilled minimal requires to run Library BooSPLG: \nCompute capability:          %d.%d\n\n", devProp.major, devProp.minor);

		//print Compute capability
		//printf("\nCompute capability:          %d.%d\n", devProp.major, devProp.minor);

		if (devProp.major < 3) //Compute capability number "3"
		{
			//	cout << "Library BoolSPLG requires a GPU with compute capability "
			//		<< "3.0 or later, exiting..." << endl;
			cout << "\n Only can run CPU funcionality.\n";
			cout << "\n==========================================================\n";

			//set CPU menu =========
			menuCPU();
			//======================

			exit(EXIT_SUCCESS);
		}
	}

	printf("\n Fulfilled minimal requires to run BSbox-tools (GPU) \n Compute capability. %d.%d\n", devProp.major, devProp.minor);
	printf("\n Running on... \n  Device: %s\n", devProp.name);
}