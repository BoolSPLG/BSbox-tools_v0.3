//Header file for Main CPU Menu 

//@@System includes
#include <iostream>
#include <stdio.h>

using namespace std;

void mainMenuCPU(void) {

	printf("\n\n        BSbox-tools {Main menu CPU} \n");
	cout << "\n This program compute cryptographic properties of Boolean and Vector Boolean function (S-box)\n";
	cout << " with input file 'infile_exampl' and write result in output file 'outfile_exampl'\n\n";
	//cout << " with input in file 'exampl_bool' (for Boolean funtion) and 'exampl_Sbox' (for Vector Boolean function) \n";
	//cout << " and write the result in file  'Result\\OutputResultXX.txt'\n\n";

	cout << "   1. Compute cryptographic properties of Boolean function (CPU)\n\n";

	cout << "   2. Compute cryptographic properties of S-box(es) (CPU)\n\n";

	cout << "   3. Find Boolean function with specific parameters (CPU) \n\n";

	cout << "   4. Find S-box(es) with specific parameters (CPU) \n\n";

	cout << "   5. Generate random Boolean(s) \n";
	cout << "   6. Generate random S-box(es) \n\n";

	cout << "   7. Change the infilename: '" << infile_exampl << "' \n";
	cout << "   8. Change the outfilename: '" << outfile_exampl << "' \n\n";

	cout << "   9. Show infilename: '" << infile_exampl << "' \n";
	cout << "   10. Show outfilename: '" << outfile_exampl << "' \n\n";

	cout << "   11. About 'BSbox-tools' \n";
	cout << "   12. Help \n\n";

	cout << "   13. Quit \n";
	cout << "\n Please choose: ";

	//cin >> choice1;
	//cin >> input(choice1);

	//Checking input value is an integer
	//checkInput(choice1);
	//choice1 = checkInput(choice1, 1, 12);

	cin >> choice1;
	input(choice1);

	//Checking input value is an integer
	choice1 = checkInput(choice1, 1, 13);

}

void menuCPU() {
	do {
		choice2 = 0;
		mainMenuCPU();

		switch (choice1) {

		case 1:
			cout << "\n==========================================================\n";
			//cout << "   1. Compute cryptographic properties (CPU) of Boolean function \n";
			CPU_bool();
			cout << "\n==========================================================\n";
			break;

		case 2:
			cout << "\n==========================================================\n";
			//cout << "   2. Compute cryptographic properties (CPU) of S-box(es) \n";
			CPU_Sbox();
			cout << "\n==========================================================\n";
			break;

		case 3:
			cout << "\n==========================================================\n";
			//cout << "   3. Find Boolean function with specific parameters (CPU) \n";
			findCPU_bool();
			cout << "\n==========================================================\n";
			break;

		case 4:
			cout << "\n==========================================================\n";
			//cout << "   4. Find S-box(es) with specific parameters (CPU) \n";
			findCPU_sbox();
			cout << "\n==========================================================\n";
			break;

		case 5:
			cout << "\n==========================================================\n";
			//cout << "   5. Generate random Boolean(s)) \n";
			cout << "   Generate random Boolean(s) \n";
						GenBoolean();
			cout << "\n==========================================================\n";
			break;

		case 6:
			cout << "\n==========================================================\n";
			//cout << "   6. Generate random S-box(es) \n";
			cout << "   Generate random S-box(es) \n";
						GenSboxes();
			cout << "\n==========================================================\n";
			break;

		case 7:
			cout << "\n==========================================================\n";
			//cout << "   7. Change the infilename:\n";// '" << exampl_bool << "'\n";
			SetInputFileName();
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 8:
			cout << "\n==========================================================\n";
			//cout << "   8. Change the outfilename:\n";// '" << NameFile_OutputResult << "'\n\n";
			SeOutputFileName();
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 9:
			cout << "\n==========================================================\n";
			cout << "  Show infilename:\n";
						readFromFileInputOuput(infile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 10:
			cout << "\n==========================================================\n";
			cout << "   Show outfilename:\n";
						readFromFileInputOuput(outfile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 11:
			cout << "\n==========================================================\n";
			cout << "\n Current release: v0.3 \n";
			cout << "\n BSbox-tools\n\n";
			cout << " Copyright @2017-2023 Dusan and Iliya.  All rights reserved.\n\n";
			cout << " Please refer to the NVIDIA end user license agreement(EULA) for GPU code.\n";
			cout << " Permission is hereby given for use for educational purpose and scientific research.\n";
			cout << " If you want to use for any other purpose, contact as.\n";
			cout << " Absolutely no guarantees or warranties are made concerning the suitability, correctness,\n";
			cout << " or any other aspect of this program.\n";
			cout << " Any use is at your own risk.";
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 12:
			cout << "\n==========================================================\n";
			cout << "\n Help information’s  for BSbox-tools\n";
			cout << "\n  Console interface program that can run on CPU and Nvidia GPU.\n\n";
			cout << "\n  Full CPU-GPU functionality run only on Nvidia GPU with compute capability 3.0 or later.\n\n";
			cout << " Compute cryptographic properties of Boolean and Vector Boolean function (S-box).\n\n";
			cout << " Input file '" << infile_exampl << "' (for Boolean and S-box funtion). \n";
			cout << " Input size for Boolean funtion 2^5 to 2^20 (GPU). \n";
			cout << " Input size for Boolean funtion no limitation depend from hardware (CPU).\n\n";
			cout << " Input size for S-box 2^5 to 2^20 (GPU).\n";
			cout << " Input size for S-box no limitation depend from hardware (CPU).\n";
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 13:
			cout << "\n==========================================================\n";
			cout << "\nExit of program...\n";
			cout << "\n==========================================================\n";
			break;

		default:
			cout << "\n==========================================================\n";
			cout << "\n ==== Please choose from the allowed options! =====\n";
			cout << "\n==========================================================\n";
			break;
		}

	} while (choice1 != 13);

}