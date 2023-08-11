//Header file for Find Menu S-box GPU

//@@System includes
#include <iostream>
#include <stdio.h>

using namespace std;


void findGPU_sboxMenu(void) {

	printf("\n         BSbox-tools {Find S-box(es) (GPU)} \n");
	cout << "\n Find and compute cryptographic properties of S-box(es) with specific parameters\n";
	//cout << " with input in file 'exampl_bool' (for Boolean funtion) \n";

	//cout << "\n\n exampl_bool -> 'Result\\OutputResultXX.txt'\n\n";
	cout << "\n\n '" << infile_exampl << "' -> '" << outfile_exampl << "'\n\n";

	cout << "  1. Compute Nonlinearity: nl(S) \n";
	cout << "  2. Compute Autocorrelation: AC(S) \n";
	cout << "  3. Compute Algebraic degree: (max)deg(S) \n";
	cout << "  4. Differential uniformity: delta(S) \n\n";

	cout << "  5. Compute all parameters: Lin(S), nl(S), AC(S), (max)deg(S) and delta(S) \n\n";

	cout << "  6. Compute (bitwise) Algebraic degree: (max)deg(S) \n\n";

	cout << "  7. Show infilename: '" << infile_exampl << "' \n";
	cout << "  8. Show outfilename: '" << outfile_exampl << "' \n\n";

	cout << "  9. Back: Main menu\n";
	cout << "\n Please choose: ";
	cin >> choice2;
	input(choice2);

	//Checking input value is an integer
	//checkInput(choice2);
	choice2 = checkInput(choice2, 1, 9);
}
//=======================================================================================

void findGPU_sbox(void) {

	do {
		findGPU_sboxMenu();

		switch (choice2) {

		case 1:
			cout << "\n==========================================================\n";
			int choose1;
			//			SetParBoolAllFunc();

			cout << "\n This function will search and save all input S-box(es) above Nonlinearity nl(S) boundary (include nl(S) boundary).\n";

			do {
				cout << "\n  nl(S)=" << nl_gpuBorder << "   Lin(S)=2^n-2*nl(S))" << "\n";

				cout << "\n  1. Set Nonlinearity: nl(S)\n";
				cout << "  2. Start computing\n";
				cout << "  3. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose1;
				input(choose1);
				//Check integer input 
				//choose1 = checkInput(choose1);
				choose1 = checkInput(choose1, 1, 3);

				switch (choose1)
				{

				case 1:
					cout << "\n  Set the border: nl(S)=";
					cin >> nl_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//0 - menuChoice Compute Nonlinearity: Lin(S), nl(S)
					FindReadFromFileComputeSboxGPU(infile_exampl, 0, nl_gpuBorder, AC_gpuBorder, deg_gpuBorder, delta_gpuBorder);
					cout << "\n Finish ...";
					cout << "\n==========================================================\n";
					PressEnter();
					break;

				case 3:
					cout << "\n==========================================================\n";
					cout << "Back to Sub menu.";
					cout << "\n==========================================================\n";
					break;

				default:
					cout << "\n ==== Your input is not allowed! =====\n";
					break;
				}
			} while (choose1 != 3);

			break;

		case 2:
			cout << "\n==========================================================\n";
			int choose2;

			//cout << "\nCompute Autocorrelation: AC(f)\n";

			cout << "\n This function will search and save all input S-box(es) under Autocorrelation AC(S) boundary (include AC(S) boundary).\n";

			do {
				cout << "\n  AC(S)=" << AC_gpuBorder << ";  ==>Set AC(S) bound bigger then 0 \n";

				cout << "\n  1. Set Autocorrelation: AC(S)\n";
				cout << "  2. Start computing\n";
				cout << "  3. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose2;
				input(choose2);

				//choose2 = checkInput(choose2);
				choose2 = checkInput(choose2, 1, 3);

				switch (choose2)
				{

				case 1:
					cout << "\n  Set the border: AC(S)=";
					cin >> AC_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//1 - menuChoice Compute: AC(S)
					FindReadFromFileComputeSboxGPU(infile_exampl, 1, nl_gpuBorder, AC_gpuBorder, deg_gpuBorder, delta_gpuBorder);
					cout << "\n Finish ...";
					cout << "\n==========================================================\n";
					PressEnter();
					break;

				case 3:
					cout << "\n==========================================================\n";
					cout << "Back to Sub menu.";
					cout << "\n==========================================================\n";
					break;

				default:
					cout << "\n ==== Your input is not allowed! =====\n";
					break;
				}
			} while (choose2 != 3);


			break;

		case 3:

			int choose3;
			//			SetParBoolAllFunc();

			cout << "\n This function will search and save all input S-box(es) above Algebraic degree (max)deg(S) boundary (include (max)deg(S) boundary).\n";

			do {
				cout << "\n  (max)deg(S)=" << deg_gpuBorder << ";  ==>Set border (max)deg(S) not bigger then n\n";

				cout << "\n  1. Set Algebraic degree: (max)deg(S)\n";
				cout << "  2. Start computing\n";
				cout << "  3. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose3;
				input(choose3);

				//Checking input value is an integer
				//choose3 = checkInput(choose3);
				choose3 = checkInput(choose3, 1, 3);

				switch (choose3)
				{

				case 1:
					cout << "\n  Set the border: (max)deg(S)=";
					cin >> deg_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//2 - menuChoice Compute: (max)deg(S)
					FindReadFromFileComputeSboxGPU(infile_exampl, 2, nl_gpuBorder, AC_gpuBorder, deg_gpuBorder, delta_gpuBorder);
					cout << "\n Finish ...";
					cout << "\n==========================================================\n";
					PressEnter();
					break;

				case 3:
					cout << "\n==========================================================\n";
					cout << "Back to Sub menu.";
					cout << "\n==========================================================\n";
					break;

				default:
					cout << "\n ==== Your input is not allowed! =====\n";
					break;
				}
			} while (choose3 != 3);

			break;

		case 4:

			int choose4;
			//			SetParBoolAllFunc();

			cout << "\n This function will search and save all input S-box(es) under Differential uniformity delta(S) boundary (include delta(S) boundary).\n";

			do {
				cout << "\n  delta(S)=" << delta_gpuBorder << ";  ==>Set border delta(S) bigger then 2\n";

				cout << "\n  1. Set Differential uniformity: delta(S)\n";
				cout << "  2. Start computing\n";
				cout << "  3. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose4;
				input(choose4);

				//Checking input value is an integer
				//choose4 = checkInput(choose4);
				choose4 = checkInput(choose4, 1, 3);

				switch (choose4)
				{

				case 1:
					cout << "\n  Set the border: delta(S)=";
					cin >> delta_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//3 - menuChoice Compute: delta(S)
					FindReadFromFileComputeSboxGPU(infile_exampl, 3, nl_gpuBorder, AC_gpuBorder, deg_gpuBorder, delta_gpuBorder);
					cout << "\n Finish ...";
					cout << "\n==========================================================\n";
					PressEnter();
					break;

				case 3:
					cout << "\n==========================================================\n";
					cout << "Back to Sub menu.";
					cout << "\n==========================================================\n";
					break;

				default:
					cout << "\n ==== Your input is not allowed! =====\n";
					break;
				}
			} while (choose4 != 3);

			break;


		case 5:
			cout << "\n==========================================================\n";
			int choose5;

			cout << "\n This function will search and search all input S-box(es) for set nl(S), AC(S), (max)deg(S) and delta(S) boundary (with included boundary).\n";
			cout << "\n Compute Boolean function with parameters: Lin(S), nl(S)(>), AC(S)(<), (max)deg(S)(>)  and delta(S)\n";

			do {
				cout << "\n  nl(S)=" << nl_gpuBorder << "  ==>Lin(S)=2^n-2*nl(S))" << " ==>search bigger then\n";
				cout << "  AC(S)=" << AC_gpuBorder << ";  ==>Set AC(S) bound bigger then 0 ==>search less then\n";
				cout << "  (max)deg(S)=" << deg_gpuBorder << ";  ==>Set border (max)deg(S) not bigger then n ==>search bigger then\n";
				cout << "  delta(S)=" << delta_gpuBorder << ";  ==>Set border delta(S) bigger then 2 ==>search less then\n";

				cout << "\n  1. Set Nonlinearity: nl(S)\n";
				cout << "  2. Set Autocorrelation: AC(S)\n";
				cout << "  3. Set Algebraic degree: (max)deg(S)\n";
				cout << "  4. Set Differential uniformity: delta(S)\n";
				cout << "  5. Start computing\n";
				cout << "  6. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose5;
				input(choose5);

				//Checking input value is an integer
				//choose5 = checkInput(choose5);
				choose5 = checkInput(choose5, 1, 5);

				switch (choose5)
				{
				case 1:
					cout << "\n  Set the border: nl(S)=";
					cin >> nl_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n  Set the border: AC(S)=";
					cin >> AC_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 3:
					cout << "\n  Set the border: (max)deg(S)=";
					cin >> deg_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 4:
					cout << "\n  Set the border: delta(S)=";
					cin >> delta_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 5:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//4 - menuChoice Compute: Lin(S), nl(S), AC(S), (max)deg(S), delta(S)
					FindReadFromFileComputeSboxGPU(infile_exampl, 4, nl_gpuBorder, AC_gpuBorder, deg_gpuBorder, delta_gpuBorder);
					cout << "\n Finish ...";
					cout << "\n==========================================================\n";
					PressEnter();
					break;

				case 6:
					cout << "\n==========================================================\n";
					cout << "Back to Sub menu.";
					cout << "\n==========================================================\n";
					break;

				default:
					cout << "\n ==== Your input is not allowed! =====\n";
					break;
				}
			} while (choose5 != 6);
			break;


		case 6:

			int choose6;
			//			SetParBoolAllFunc();

			cout << "\n This (bitwise) function will search and save all input S-box(es) above Algebraic degree (max)deg(S) boundary (include deg(S) boundary).\n";

			do {
				cout << "\n  (max)deg(S)=" << deg_gpuBorder << ";  ==>Set border (max)deg(S) not bigger then n\n";

				cout << "\n  1. Set Algebraic degree: (max)deg(S)\n";
				cout << "  2. Start computing (bitwise) \n";
				cout << "  3. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose6;
				input(choose6);

				//Checking input value is an integer
				//choose3 = checkInput(choose3);
				choose3 = checkInput(choose6, 1, 3);

				switch (choose6)
				{

				case 1:
					cout << "\n  Set the border: (max)deg(S)=";
					cin >> deg_gpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//Compute: (max)deg(S)
					FindReadFromFileComputeBitwiseANF_degSboxGPU(infile_exampl, deg_gpuBorder);
					cout << "\n Finish ...";
					cout << "\n==========================================================\n";
					PressEnter();
					break;

				case 3:
					cout << "\n==========================================================\n";
					cout << "Back to Sub menu.";
					cout << "\n==========================================================\n";
					break;

				default:
					cout << "\n ==== Your input is not allowed! =====\n";
					break;
				}
			} while (choose6 != 3);

			break;

		case 7:
			cout << "\n==========================================================\n";
			cout << "  Show infilename:\n";
			readFromFileInputOuput(infile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 8:
			cout << "\n==========================================================\n";
			cout << "   Show outfilename:\n";
			readFromFileInputOuput(outfile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 9:
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
	} while (choice2 != 9);
}
//===============================================================================================