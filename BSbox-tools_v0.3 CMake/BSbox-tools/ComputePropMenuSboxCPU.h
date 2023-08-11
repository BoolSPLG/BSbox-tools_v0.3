//Header file for Compute cryptographic properties (GPU) of Boolean function

//@@System includes
#include <iostream>
#include <stdio.h>

using namespace std;


void CPU_SboxMenu(void) {

	printf("\n         BSbox-tools {S-box(es) (CPU)} \n");
	cout << "\n Compute cryptographic properties of S-box(es)\n";

	cout << "\n\n'" << infile_exampl << "' -> '" << outfile_exampl << "'\n\n";

	cout << "  1. Compute Linear Approximation Table of S-box(es): LAT(S) \n";
	cout << "  2. Linearity: Lin(S) \n";
	cout << "  3. Compute Walsh Spectrum Table and Linearity: WST(S), Lin(S) \n";
	cout << "  4. Nonlinearity: nl(S) \n";
	cout << "  5. Autocorrelation Spectrum: ACT(S) \n";
	cout << "  6. Autocorrelation: AC(S) \n";
	cout << "  7. Compute Autocorrelation spectrum and Autocorrelation: ACT(S), AC(S) \n";
	cout << "  8. Compute Algebraic Normal Form of input S-box(es): ANF(S) \n";
	cout << "  9. Compute Algebraic Degree Table of input S-box(es): ADT(S) \n";
	cout << "  10. Algebraic degree (max): deg(S) \n";
	cout << "  11. Algebraic degree (min): deg(S) \n\n";

	cout << "  12. Compute (bitwise) Algebraic Normal Form of input S-box(es): ANF(S) \n";
	cout << "  13. (bitwise) Algebraic degree (max): deg(S) \n";
	cout << "  14. (bitwise) Algebraic degree (min): deg(S) \n\n";

	cout << "  15. Compute Component functions: CF(S) \n";
	cout << "  16. Compute Difference distribution table: DDT(S) \n";
	cout << "  17. Compute Differential uniformity: delta(S) \n\n";

	cout << "  18. Show infilename: '" << infile_exampl << "' \n";
	cout << "  19. Show outfilename: '" << outfile_exampl << "' \n\n";

	cout << "  20. Back: Main menu \n";
	cout << "\n Please choose: ";

	cin >> choice2;
	input(choice2);

	//Checking input value is an integer
	//choice2 = checkInput(choice2);
	choice2 = checkInput(choice2, 1, 20);
}

void CPU_Sbox(void) {

	do {
		CPU_SboxMenu();

		switch (choice2) {

		case 1:
			cout << "\n==========================================================\n";
			cout << "  1. Compute Linear Approximation Table of S-box(es): LAT(S)\n";

			//0 - menuChoice for compute Linear Approximation Table of S-box
			ReadFromFileComputeNlSboxCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 2:
			cout << "\n==========================================================\n";
			cout << "  2. Linearity: Lin(S)\n";

			//1 - menuChoice for compute Lin of S-box
			ReadFromFileComputeNlSboxCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 3:
			cout << "\n==========================================================\n";
			cout << "  3. Compute Walsh Spectrum Table and Linearity: WST(S), Lin(S)\n";

			//2 - menuChoice for compute Linear Approximation Table and Lin of S-box
			ReadFromFileComputeNlSboxCPU(infile_exampl, 2);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 4:
			cout << "\n==========================================================\n";
			cout << "  4. Nonlinearity: nl(S):\n";

			//3 - menuChoice for compute NL of S-box
			ReadFromFileComputeNlSboxCPU(infile_exampl, 3);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 5:
			cout << "\n==========================================================\n";
			cout << "  5. Autocorrelation Spectrum: ACT(S)\n";

			//0 - menuChoice for compute Autocorrelation Spectrum of S-box
			ReadFromFileComputeACSboxCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 6:
			cout << "\n==========================================================\n";
			cout << "  6. Autocorrelation: AC(S)\n";

			//1 - menuChoice for compute AC(S) of S-box
			ReadFromFileComputeACSboxCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 7:
			cout << "\n==========================================================\n";
			cout << "  7. Compute Autocorrelation spectrum and Autocorrelation: ACT(S), AC(S)\n";

			//2 - menuChoice for compute Autocorrelation spectrum and AC(S) of S-box
			ReadFromFileComputeACSboxCPU(infile_exampl, 2);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 8:
			cout << "\n==========================================================\n";
			cout << "  8. Compute Algebraic Normal Form of input S-box(es): ANF(S) \n";

			//0 - menuChoice for compute Algebraic Normal Form of input S-box(es)
			ReadFromFileComputeANF_degSboxCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 9:
			cout << "\n==========================================================\n";
			cout << "  9. Compute Algebraic Degree Table of input S-box(es): ADT(S) \n";

			//0 - menuChoice for compute Algebraic Normal Form of input S-box(es)
			ReadFromFileComputeANF_degSboxCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;		

		case 10:
			cout << "\n==========================================================\n";
			cout << "  10. Algebraic degree (max): deg(S)\n\n";

			//1 - menuChoice for compute deg(S)
			ReadFromFileComputeANF_degSboxCPU(infile_exampl, 2);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 11:
			cout << "\n==========================================================\n";
			cout << "  10. Algebraic degree (min): deg(S)\n\n";

			//1 - menuChoice for compute deg(S)
			ReadFromFileComputeANF_degSboxCPU(infile_exampl, 3);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 12:
			cout << "\n==========================================================\n";
			cout << "  12. Compute (bitwise) Algebraic Normal Form of input S-box(es): ANF(S) \n";

			//0 - menuChoice for compute (bitwise) Algebraic Normal Form of input S-box(es)
			ReadFromFileComputeBitwiseANF_degSboxCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 13:
			cout << "\n==========================================================\n";
			cout << "  13. (bitwise) Algebraic degree (max): deg(S)\n\n";

			//1 - menuChoice for compute (bitwise) deg(S)
			ReadFromFileComputeBitwiseANF_degSboxCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 14:
			cout << "\n==========================================================\n";
			cout << "  14. (bitwise) Algebraic degree (min): deg(S)\n\n";

			//1 - menuChoice for compute (bitwise) deg(S)
			ReadFromFileComputeBitwiseANF_degSboxCPU(infile_exampl, 2);
			cout << "\n==========================================================\n";
			PressEnter();
			break;


		case 15:
			cout << "\n==========================================================\n";
			cout << "  15. Compute Component functions: CF(S) \n";

			//Compute Component functions: CF(S) 
			ReadFromFileComputeCFSboxCPU(infile_exampl);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 16:
			cout << "\n==========================================================\n";
			cout << "  16. Compute Difference distribution table: DDT(S) \n";

			//0 - menuChoice for compute Difference distribution table: DDT(S)
			ReadFromFileComputeDDT_deltaSboxCPU(infile_exampl, 0);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 17:
			cout << "\n==========================================================\n";
			cout << "  17. Compute Differential uniformity: delta(S) \n";

			//1 - menuChoice for compute delta(S)
			ReadFromFileComputeDDT_deltaSboxCPU(infile_exampl, 1);
			cout << "\n==========================================================\n";
			PressEnter();
			break;

		case 18:
			cout << "\n==========================================================\n";
			cout << "  Show infilename:\n";
			readFromFileInputOuput(infile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;


		case 19:
			cout << "\n==========================================================\n";
			cout << "   Show outfilename:\n";
			readFromFileInputOuput(outfile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 20:
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
	} while (choice2 != 20);
}
//===============================================================================================