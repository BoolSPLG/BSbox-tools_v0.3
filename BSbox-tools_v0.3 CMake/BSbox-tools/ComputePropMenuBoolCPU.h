//Header file for Compute cryptographic properties (GPU) of Boolean function

//@@System includes
#include <iostream>
#include <stdio.h>

using namespace std;


void CPU_boolMenu(void) {

	printf("\n         BSbox-tools {Boolean function (CPU)} \n");
	cout << "\n Compute cryptographic properties of Boolean function\n";
	//cout << " with input in file 'exampl_bool' (for Boolean funtion) \n";

	//cout << "\n\n exampl_bool -> 'Result\\OutputResultXX.txt'\n\n";
	cout << "\n\n'" << infile_exampl << "' -> '" << outfile_exampl << "'\n\n";

	cout << "  1. Compute Walsh spectrum of Boolean function: WS(f) \n";
	cout << "  2. Linearity: Lin(f) \n";
	cout << "  3. Compute Walsh spectrum and Linearity: WS(f), Lin(f) \n";
	cout << "  4. Nonlinearity: nl(f): \n";
	cout << "  5. Autocorrelation Spectrum: AS(f) \n";
	cout << "  6. Autocorrelation: AC(f) \n";
	cout << "  7. Compute Autocorrelation spectrum and Autocorrelation: AS(f), AC(f) \n";
	cout << "  8. Compute Algebraic Normal Form from True Table and vice versa: TT(f)->ANF(f) and ANF(f) -> TT(f) \n";
	cout << "   Depend from the input vector ANF(f) (or TT(f))\n";
	cout << "  9. Algebraic degree: deg(f) \n\n";

	cout << "  10. Compute (Bitwise) Algebraic Normal Form from True Table and vice versa: TT(f)->ANF(f) and ANF(f)->TT(f) \n";
	cout << "  11. Compute (Bitwise) Algebraic degree: deg(f) \n\n";

	cout << "  12. Show infilename: '" << infile_exampl << "' \n";
	cout << "  13. Show outfilename: '" << outfile_exampl << "' \n\n";

	cout << "  14. Back: Main menu \n";
	cout << "\n Please choose: ";
	cin >> choice2;
	input(choice2);

	//Checking input value is an integer
	//choice2 = checkInput(choice2);
	choice2 = checkInput(choice2, 1, 14);
}

void CPU_bool(void) {

	do {
		CPU_boolMenu();

		switch (choice2) {

		case 1:
			cout << "\n==========================================================\n";
			cout << "  1. Compute Walsh spectrum of Boolean function: WS(f)\n";

			//0 - menuChoice for compute Walsh spectrum
			ReadFromFileComputeNlCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 2:
			cout << "\n==========================================================\n";
			cout << "  2. Compute Linearity: Lin(f)\n";

			//1 - menuChoice for compute Lin
			ReadFromFileComputeNlCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 3:
			cout << "\n==========================================================\n";
			cout << "  3. Compute Walsh spectrum and Linearity: WS(f), Lin(f)\n";

			//2 - menuChoice for compute Walsh spectrum and Lin
			ReadFromFileComputeNlCPU(infile_exampl, 2);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 4:
			cout << "\n==========================================================\n";
			cout << "  4. Compute Nonlinearity: nl(f):\n";

			//3 - menuChoice for compute nl
			ReadFromFileComputeNlCPU(infile_exampl, 3);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 5:
			cout << "\n==========================================================\n";
			cout << "  5. Autocorrelation Spectrum: AS(f)\n";

			//0 - menuChoice for compute Autocorrelation Spectrum
			ReadFromFileComputeACCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 6:
			cout << "\n==========================================================\n";
			cout << "  6. Autocorrelation: AC(f)\n";

			//1 - menuChoice for compute AC
			ReadFromFileComputeACCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 7:
			cout << "\n==========================================================\n";
			cout << "  7. Compute Autocorrelation spectrum and Autocorrelation: AS(f), AC(f)\n";

			//2 - menuChoice for compute Autocorrelation spectrum and AC
			ReadFromFileComputeACCPU(infile_exampl, 2);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 8:
			cout << "\n==========================================================\n";
			cout << "  8. Compute Algebraic Normal Form from True Table and vice versa: TT(f)->ANF(f) and ANF(f) -> TT(f) \n";
			cout << "   Depend from the input vector ANF(f) (or TT(f))\n";

			//0 - menuChoice for compute TT(f)->ANF(f) or ANF(f) -> TT(f)
			ReadFromFileComputeANF_degCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 9:
			cout << "\n==========================================================\n";
			cout << "  9. Algebraic degree: deg(f)\n";

			//1 - menuChoice for compute deg(f)
			ReadFromFileComputeANF_degCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 10:
			cout << "\n==========================================================\n";
			cout << "  10. Compute (Bitwise) Algebraic Normal Form from True Table and vice versa: TT(f)->ANF(f) and ANF(f)->TT(f) \n";

			//0 - menuChoice for Compute (Bitwise) ANF from TT and vice versa
			ReadFromFileComputeBitWise_ANFCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 11:
			cout << "\n==========================================================\n";
			cout << "  11. Compute (Bitwise) Algebraic degree: deg(f) \n";

			//1 - menuChoice for Compute (Bitwise) Algebraic degree: deg(f)
			ReadFromFileComputeBitWise_ANFCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 12:
			cout << "\n==========================================================\n";
			cout << "  Show infilename:\n";
			readFromFileInputOuput(infile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;


		case 13:
			cout << "\n==========================================================\n";
			cout << "   Show outfilename:\n";
			readFromFileInputOuput(outfile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 14:
			cout << "\n==========================================================\n";
			cout << "Back to Main menu.";
			cout << "\n==========================================================\n";
			break;

		default:
			cout << "\n==========================================================\n";
			cout << "\n ==== Please choose from the allowed options! =====";
			cout << "\n==========================================================\n";
			break;
		}
	} while (choice2 != 14);
}
//===============================================================================================