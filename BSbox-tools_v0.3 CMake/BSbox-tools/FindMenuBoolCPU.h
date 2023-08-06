//Header file for Find Menu CPU

//@@System includes
#include <iostream>
#include <stdio.h>

using namespace std;

void findCPU_boolMenu(void) {

	printf("\n         BSbox-tools {Find Boolean function (CPU)} \n");
	cout << "\n Find and compute cryptographic properties of Boolean function with specific parameters\n";
	//cout << " with input in file 'exampl_bool' (for Boolean funtion) \n";

	//cout << "\n\n exampl_bool -> 'Result\\OutputResultXX.txt'\n\n";
	cout << "\n\n '" << infile_exampl << "' -> '" << outfile_exampl << "'\n\n";

	cout << "  1. Compute Nonlinearity: nl(f) \n";
	cout << "  2. Compute Autocorrelation: AC(f) \n";
	cout << "  3. Compute Algebraic degree: deg(f) \n\n";

	cout << "  4. Compute all parameters: Lin(f), nl(f), AC(f) and deg(f) \n\n";

	cout << "  5. Compute (bitwise) Algebraic degree: deg(f) \n\n";

	cout << "  6. Show infilename: '" << infile_exampl << "'\n";
	cout << "  7. Show outfilename: '" << outfile_exampl << "'\n\n";

	cout << "  8. Back: Main menu\n";
	cout << "\n Please choose: ";
	cin >> choice2;
	input(choice2);

	//Checking input value is an integer
	//checkInput(choice2);

	choice2 = checkInput(choice2, 1, 8);
	nl_cpuBorder = 0, AC_cpuBorder = 0, deg_cpuBorder = 0, delta_cpuBorder = 0;
}
//=======================================================================================

void findCPU_bool(void) {

	do {
		findCPU_boolMenu();

		switch (choice2) {

		case 1:
			cout << "\n==========================================================\n";
			int choose1;

			cout << "\n This function will search and save all input Boolean function above Nonlinearity nl(f) boundary (include nl(f) boundary).\n";

			do {
				cout << "\n  nl(f)=" << nl_cpuBorder << "   Lin(f)=2^n-2*nl(f))" << "\n";

				cout << "\n  1. Set Nonlinearity: nl(f)\n";
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
					cout << "\n  Set the border: nl(f)=";
					cin >> nl_cpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//0 - menuChoice Compute Nonlinearity: nl(f)
					FindReadFromFileComputeBooleanCPU(infile_exampl, 0, nl_cpuBorder, AC_cpuBorder, deg_cpuBorder);

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

			cout << "\n This function will search and save all input Boolean function under Autocorrelation AC(f) boundary (include AC(f) boundary).\n";

			do {
				cout << "\n  AC(f)=" << AC_cpuBorder << ";  ==>Set AC(f) bound bigger then 0 \n";

				cout << "\n  1. Set Autocorrelation: AC(f)\n";
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
					cout << "\n  Set the border: AC(f)=";
					cin >> AC_cpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//1 - menuChoice Compute Autocorelacion: AC(f)
					FindReadFromFileComputeBooleanCPU(infile_exampl, 1, nl_cpuBorder, AC_cpuBorder, deg_cpuBorder);
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
			cout << "\n==========================================================\n";
			int choose3;

			cout << "\n This function will search and save all input Boolean function above Algebraic degree deg(f) boundary (include deg(f) boundary).\n";

			do {
				cout << "\n  deg(f)=" << deg_cpuBorder << ";  ==>Set border deg(f) not bigger then n\n";

				cout << "\n  1. Set Algebraic degree: deg(f)\n";
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
					cout << "\n  Set the border: deg(f)=";
					cin >> deg_cpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//2 - menuChoice Compute : deg(f)
					FindReadFromFileComputeBooleanCPU(infile_exampl, 2, nl_cpuBorder, AC_cpuBorder, deg_cpuBorder);
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
			cout << "\n==========================================================\n";
			int choose4;

			cout << "\n This function will search and search all input Boolean function for set nl(f), AC(f) and deg(f) boundary (with included boundary).\n";
			cout << "\n Compute Boolean function with parameters: Lin(f), nl(f)(>), AC(f)(<) and deg(f)(>)\n";

			do {
				cout << "\n  nl(f)=" << nl_cpuBorder << "  ==>Lin(f)=2^n-2*nl(f))" << " ==>search bigger then\n";
				cout << "  AC(f)=" << AC_cpuBorder << ";  ==>Set AC(f) bound bigger then 0 ==>search less then\n";
				cout << "  deg(f)=" << deg_cpuBorder << ";  ==>Set border deg(f) not bigger then n ==>search bigger then\n";

				cout << "\n  1. Set Nonlinearity: nl(f)\n";
				cout << "  2. Set Autocorrelation: AC(f)\n";
				cout << "  3. Set Algebraic degree: deg(f)\n";
				cout << "  4. Start computing\n";
				cout << "  5. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose4;
				input(choose4);

				//Checking input value is an integer
				//choose4 = checkInput(choose4);
				choose4 = checkInput(choose4, 1, 5);

				switch (choose4)
				{
				case 1:
					cout << "\n  Set the border: nl(f)=";
					cin >> nl_cpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n  Set the border: AC(f)=";
					cin >> AC_cpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 3:
					cout << "\n  Set the border: deg(f)=";
					cin >> deg_cpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 4:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//2 - menuChoice Compute: NL(f)(Lin(f)), AC(f), deg(f)
					FindReadFromFileComputeBooleanCPU(infile_exampl, 3, nl_cpuBorder, AC_cpuBorder, deg_cpuBorder);
					cout << "\n Finish ...";
					cout << "\n==========================================================\n";
					PressEnter();
					break;

				case 5:
					cout << "\n==========================================================\n";
					cout << "Back to Sub menu.";
					cout << "\n==========================================================\n";
					break;

				default:
					cout << "\n ==== Your input is not allowed! =====\n";
					break;
				}
			} while (choose4 != 5);
			break;


		case 5:
			cout << "\n==========================================================\n";
			int choose5;

			cout << "\n This (bitwicse) function will search and save all input Boolean function above Algebraic degree deg(f) boundary (include deg(f) boundary).\n";

			do {
				cout << "\n  deg(f)=" << deg_cpuBorder << ";  ==>Set border deg(f) not bigger then n\n";

				cout << "\n  1. Set Algebraic degree: deg(f)\n";
				cout << "  2. Start computing (bitwise) \n";
				cout << "  3. Back: Sub menu\n\n";

				cout << " Choose:";
				cin >> choose5;
				input(choose5);

				//Checking input value is an integer
				//choose3 = checkInput(choose3);
				choose5 = checkInput(choose5, 1, 3);

				switch (choose5)
				{

				case 1:
					cout << "\n  Set the border: deg(f)=";
					cin >> deg_cpuBorder;
					cout << "\n==========================================================\n";
					break;

				case 2:
					cout << "\n==========================================================\n";
					cout << " Start computig ...\n";

					//2 - menuChoice Compute : deg(f)
					FindReadFromFileComputeBitWise_ANF_degCPU(infile_exampl, deg_cpuBorder);
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
			} while (choose5 != 3);

			break;

		case 6:
			cout << "\n==========================================================\n";
			cout << "  Show infilename:\n";
			readFromFileInputOuput(infile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 7:
			cout << "\n==========================================================\n";
			cout << "   Show outfilename:\n";
			readFromFileInputOuput(outfile_exampl);
			cout << "\n==========================================================\n";
			cout << "\n";
			PressEnter();
			break;

		case 8:
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
	} while (choice2 != 8);
}
//===============================================================================================