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

void GenBoolean()
{
	int n, numBoolean;

	cout << "\n Genereted Boolean will be save in '" << infile_exampl << "' file.\n";

	cout << "\n  Input number of bits:" << "\n" << " n=";
	cin >> n;

	cout << "\n  Input number for generate Boolean(s):" << "\n" << " num=";
	cin >> numBoolean;

	double z = n, p = pow(2, z);
	sizeBool = p;

	//Allocate memory block. Allocates a block of size bytes of memory
	BoolElemet = (int*)malloc(sizeof(int) * sizeBool);

	cout << "\n Every Boolean contain " << sizeBool << " elementis.\n\n";

	ofstream myfile;
	myfile.open(infile_exampl);
	srand(time(NULL));

	//Print Start time =================
	ShowStartTime();

	//Clock variables
	clock_t start_t, end_t;
	//start time clock
	start_t = clock();
	//============================

	for (int i = 0; i < sizeBool; i++)
		BoolElemet[i] = i % 2;

	for (int j = 0; j < numBoolean; j++)
	{
		random_shuffle(BoolElemet, BoolElemet + sizeBool);

		myfile << "?" << " " << 0 << " " << sizeBool << "\n";

		for (int i = 0; i < sizeBool; i++)
		{
			//cout << SboxElemet[i] << " ";
			myfile << BoolElemet[i] << ", ";
			//	myfile << BoolElemet[i] ;
				//cout << i << " ";
		}
		//cout << "\n\n";
		myfile << "\n\n";
		//	myfile << "\n";
	}

	//end time clock
	end_t = clock();

	cout << endl;
	cout << "Generated Boolean(s) are save in '" << infile_exampl << "' file.\n\n";
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
	free(BoolElemet);
}