// Generate_S-box.cpp : Defines the entry point for the console application.
#include <iostream>
#include <math.h>
#include <stdlib.h> 
#include <time.h> 
#include <fstream>
#include <algorithm>  //for random_shuffle http://cplusplus.com/reference/algorithm/random_shuffle/

using namespace std;

//const int n = 16384;

//int a[n];

void GenSboxes()
{
	int n, numSbox;
	//cout << "\n  Generate n-bit S-box(es)\n\n";

	cout << "\n Genereted S-box will be save in '" << infile_exampl << "' file.\n";

	cout << "\n  Input number of bits:" << "\n" << " n=";
	cin >> n;

	cout << "\n  Input number for generate S-box(es):" << "\n" << " num=";
	cin >> numSbox;

	double z = n, p = pow(2, z);
	sizeSbox = p;

	//Allocate memory block. Allocates a block of size bytes of memory
	SboxElemet = (int*)malloc(sizeof(int) * sizeSbox);

	cout << "\n Every S-box contain " << sizeSbox << " elementis.\n\n";

	ofstream myfile;
	//myfile.open("S-Box.txt");
	myfile.open(infile_exampl);
	srand(time(NULL));

	//Print Start time =================
	ShowStartTime();

	//Clock variables
	clock_t start_t, end_t;
	//start time clock
	start_t = clock();
	//============================

	for (int i = 0; i < sizeSbox; i++)
		SboxElemet[i] = i;

	for (int j = 0; j < numSbox; j++)
	{
		random_shuffle(SboxElemet, SboxElemet + sizeSbox);

		myfile << "?" << " " << 1 << " " << sizeSbox << " " << n << "\n";

		//for (int i = 0; i < sizeSbox; i++)
		//{
		//	//cout << SboxElemet[i] << " ";
		//	myfile << SboxElemet[i] << ",";
		//}
		//myfile << "\n";

		for (int i = 0; i < sizeSbox; i++)
		{
			//cout << SboxElemet[i] << " ";
			myfile << SboxElemet[i] << " ";
		}
		//cout << "\n\n";
		myfile << "\n\n";
	}

	//end time clock
	end_t = clock();

	cout << endl;
	cout << "Generated S-Box(es) are save in '" << infile_exampl << "' file.\n\n";
	myfile.close();

	//Print End time ===================

	//Print End Time
	ShowEndTime();

	double elapsed_time = (double)(end_t - start_t) / CLOCKS_PER_SEC;
	printf("==  Elapsed time: %.4fs\n\n", elapsed_time);
	//============================
	//cout << "\nPress 'Enter' to continue ... ";
	PressEnter();

	//free memory
	free(SboxElemet);
}