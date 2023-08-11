//Header file for Main Main Menu 

//@@System includes
#include <iostream>
#include <stdio.h>
#include <fstream>  
#include <string>
#include <sstream>

using namespace std;

void mainMenu(void) {

	printf("\n         BSbox-tools {Main menu GPU - CPU} \n");
	cout << "\n This program compute cryptographic properties of Boolean and Vector Boolean function (S-box)\n";
	cout << " with input file 'infile_exampl' and write result in output file 'outfile_exampl'\n\n";
	//cout << " with input in file 'exampl_bool' (for Boolean funtion) and 'exampl_Sbox' (for Vector Boolean function) \n";
	//cout << " and write the result in file  'Result\\OutputResultXX.txt'\n\n";

	//////////////////////////////////////////////////////////////////////////////////////
	cout << "   1. Information for GPU(s) - Properties, Utilities, Bandwidth Test \n\n";

	cout << "   2. Compute cryptographic properties of Boolean function (GPU)\n";

	cout << "   3. Compute cryptographic properties of Boolean function (CPU)\n\n";

	cout << "   4. Compute cryptographic properties of S-box(es) (GPU)\n";

	cout << "   5. Compute cryptographic properties of S-box(es) (CPU)\n\n";

	cout << "   6. Find Boolean function with specific parameters (GPU) \n";

	cout << "   7. Find Boolean function with specific parameters (CPU) \n\n";

	cout << "   8. Find S-box(es) with specific parameters (GPU) \n";

	cout << "   9. Find S-box(es) with specific parameters (CPU) \n\n";

	cout << "   10. Generate random Boolean(s) \n";
	cout << "   11. Generate random S-box(es) \n\n";

	cout << "   12. Change the infilename: '" << infile_exampl << "' \n";
	cout << "   13. Change the outfilename: '" << outfile_exampl << "' \n\n";

	cout << "   14. Show infilename: '" << infile_exampl << "' \n";
	cout << "   15. Show outfilename: '" << outfile_exampl << "' \n\n";

	cout << "   16. About 'BSbox-tools' \n";
	cout << "   17. Help' \n\n";

	cout << "   18. Quit \n";
	cout << "\n Please choose: ";

	cin >> choice1;
	input(choice1);

	//Checking input value is an integer
	choice1 = checkInput(choice1, 1, 18);

	//if ((choice1 > 17) || (choice1 < 1))
	//{
	//	cout << "\n==========================================================\n";
	//	cout << " ==== Please choose from the allowed options! =====\n\n";
	//	cout << " Error input" << endl;
	//	cin.clear();
	//	cin.ignore(256, '\n');
	//	cout << "\n Please choose again: ";
	//	cin >> choice1;
	//}
}

void menu() {
	do {
		//	choice1 = 0;
		mainMenu();

		//		cout << choice1 << "n";

		switch (choice1) {

		case 1:
			cout << "\n==========================================================\n";
			CUDA_Properties();
			cout << "\n==========================================================\n";
			break;

		case 2:
			cout << "\n==========================================================\n";
			GPU_bool();
			cout << "\n==========================================================\n";
			break;

		case 3:
			cout << "\n==========================================================\n";
			CPU_bool();
			cout << "\n==========================================================\n";
			break;

		case 4:
			cout << "\n==========================================================\n";
			GPU_Sbox();
			cout << "\n==========================================================\n";
			break;

		case 5:
			cout << "\n==========================================================\n";
			CPU_Sbox();
			cout << "\n==========================================================\n";
			break;

		case 6:
			cout << "\n==========================================================\n";
			findGPU_bool();
			cout << "\n==========================================================\n";
			break;

		case 7:
			cout << "\n==========================================================\n";
			findCPU_bool();
			//cout << "   7. Find Boolean function with specific parameters (CPU) \n";
			cout << "\n==========================================================\n";
			break;

		case 8:
			cout << "\n==========================================================\n";
			findGPU_sbox();
			//cout << "   8. Find S - box(es) with specific parameters(GPU) \n";
			cout << "\n==========================================================\n";
			break;

		case 9:
			cout << "\n==========================================================\n";
			findCPU_sbox();
			//cout << "   9. Find S - box(es) with specific parameters(CPU) \n";
			cout << "\n==========================================================\n";
			break;

		case 10:
			cout << "\n==========================================================\n";
			cout << "   Generate random Boolean(s) \n";
			GenBoolean();
			cout << "\n==========================================================\n";
			break;

		case 11:
			cout << "\n==========================================================\n";
			cout << "   Generate random S-box(es) \n";
			GenSboxes();
			cout << "\n==========================================================\n";
			break;

		case 12:
			cout << "\n==========================================================\n";
			//cout << "   6. Change the infilename:\n";// '" << exampl_bool << "'\n";
			SetInputFileName();
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 13:
			cout << "\n==========================================================\n";
			//cout << "   7. Change the outfilename:\n";// '" << NameFile_OutputResult << "'\n\n";
			SeOutputFileName();
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 14:
			cout << "\n==========================================================\n";
			cout << "  Show infilename:\n";
			readFromFileInputOuput(infile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 15:
			cout << "\n==========================================================\n";
			cout << "   Show outfilename:\n";
			readFromFileInputOuput(outfile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 16:
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

		case 17:
			cout << "\n==========================================================\n";
			cout << "\n Help information’s  for BSbox-tools\n";
			cout << "\n  Console interface program that can run on CPU and Nvidia GPU.\n\n";
			cout << "\n  Full CPU-GPU functionality run only on Nvidia GPU with compute capability 3.0 or later.\n\n";
			cout << " Compute cryptographic properties of Boolean and Vector Boolean function (S-box).\n\n";
			cout << " Input file '" << infile_exampl << "' (for Boolean and S-box funtion). \n";
			cout << " Input size for Boolean funtion 2^5 to 2^20 (GPU). \n";
			cout << " Input size for Boolean funtion no limitation depend from hardware (CPU).\n\n";
			//			cout << " Input in file '" << infile_exampl << "' (for Vector Boolean function).\n";
			cout << " Input size for S-box 2^5 to 2^20 (GPU).\n";
			cout << " Input size for S-box no limitation depend from hardware (CPU).\n";
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 18:
			cout << "\n==========================================================\n";
			cout << "Exit of program...";
			cout << "\n==========================================================\n";
			break;

		default:
			cout << "\n==========================================================\n";
			cout << "\n ==== Please choose from the allowed options! =====\n";
			cout << "\n==========================================================\n";
			break;
		}

	} while (choice1 != 18);
}