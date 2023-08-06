//Header file for Compute cryptographic properties (GPU) of Boolean function

//@@System includes
#include <iostream>
#include <stdio.h>

using namespace std;


void CUDA_PropertiesMenu(void) {

	printf("\n         BSbox-tools {GPU - CUDA Capable device(s) characteristics (Utilities)} \n");
	cout << "\n Information for device(s) - Properties, Utilities, Bandwidth Test\n\n";

	cout << "  1. Check CUDA Capable device(s) characteristics \n";
	cout << "  2. Check CUDA Capable device(s) characteristics (full, extend informacion) \n\n";

	cout << "  Simple test function to measure the memcopy bandwidth of the GPU\n";

	cout << "  3. GPU Bandwidth Test (Input size memory MB) \n";
	cout << "  4. GPU Bandwidth Test (Input number of variables INT) \n\n";

	cout << "  5. Back: Main menu \n";
	cout << "\n Please choose: ";
	cin >> choice2;
	input(choice2);

	//Checking input value is an integer
	//choice2 = checkInput(choice2);
	choice2 = checkInput(choice2, 1, 5);
}

void CUDA_Properties(void) {

	unsigned int size = 1, nElements = 0, bytes = 0;

	do {
		CUDA_PropertiesMenu();

		switch (choice2) {

		case 1:
			cout << "\n==========================================================\n";
			cout << "  Check CUDA Capable device(s) characteristics.";

			//Function Detected and show CUDA Capable device(s) characteristics
			BoolSPLGCheckProperties();

			cout << "\n==========================================================\n";
			break;

		case 2:
			cout << "\n==========================================================\n";
			cout << "  Check CUDA Capable device(s) characteristics (full, extend informacion).";

			//Function Detected and show CUDA Capable device(s) characteristics (full, extend informacion)
			BoolSPLGCheckProperties_v1();

			cout << "\n==========================================================\n";
			break;

		case 3:
			cout << "\n==========================================================\n";
			cout << "  Bandwidth Test.\n";

			//Simple test function to measure the memcopy bandwidth of the GPU
			cout << "Input memory size for transfer (MB):";
			cin >> size;
			nElements = size * 256 * 1024;
			bytes = nElements * sizeof(int);

			bandwidthTest(bytes, nElements);
			cout << "\n==========================================================\n";
			break;

		case 4:
			cout << "\n==========================================================\n";
			cout << "  Bandwidth Test.\n";

			//Simple test function to measure the memcopy bandwidth of the GPU
			cout << "Input number of variables (INT):";
			cin >> nElements;
			//unsigned int nElements = size * 256 * 1024;
			bytes = nElements * sizeof(int);

			bandwidthTest(bytes, nElements);
			cout << "\n==========================================================\n";
			break;

		case 5:
			cout << "\n==========================================================\n";
			cout << "  Back to Main menu.";
			cout << "\n==========================================================\n";
			break;

		default:
			cout << "\n==========================================================\n";
			cout << "\n ==== Please choose from the allowed options! =====";
			cout << "\n==========================================================\n";
			break;
		}
	} while (choice2 != 5);
}
//===============================================================================================