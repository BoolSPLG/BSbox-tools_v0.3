//Help Heder file "InputOutputFiles.h" 

//input header files
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>

#include <nmmintrin.h>

using namespace std;


//----------------------------------------------------------------------------
template <typename T>
struct input_t
{
	mutable T n;
	explicit input_t(T& n) : n(n) { }
	input_t(const input_t <T>& i) : n(i.n) { }
};

//----------------------------------------------------------------------------
template <typename T>
inline
input_t <T>
input(T& n)
{
	input_t <T> result(n);
	return result;
}

//----------------------------------------------------------------------------
template <typename T>
istream& operator >> (istream& ins, const input_t <T>& i)
{
	// Read a line (terminated by ENTER|NEWLINE) from the user
	string s;
	getline(ins, s);

	// Get rid of any trailing whitespace
	s.erase(s.find_last_not_of(" \f\n\r\t\v") + 1);

	// Read it into the target type
	istringstream ss(s);
	ss >> i.n;

	// Check to see that there is nothing left over
	if (!ss.eof())
		ins.setstate(ios::failbit);

	return ins;
}

//----------------------------------------------------------------------------

//inline bool exist(const std::string& name)
//{
//	ifstream file(name);
//	if (!file)            // If the file was not found, then file is 0, i.e. !file=1 or true.
//		return false;    // The file was not found.
//	else                 // If the file was found, then file is non-0.
//		return true;     // The file was found.
//}

//Function retun if filename (string) exist
inline bool fexists(const string& filename) {
	bool exist = true;

	ifstream ifile;
	ifile.open(filename);

	if (ifile.fail())
		exist = false;

	return exist;
}

//Show input file
void readFromFileInputOuput(string filename)
{
	ifstream f(filename);

	if (f.is_open())
	{
		//file opened successfully so we are here
		cout << "\nFile '" << filename << "' Opened successfully!.\n";

		cout << f.rdbuf();

		f.close();
	}
	else //file could not be opened
	{
		cout << "\n====== Error in opening file '" << filename << "' could not be opened ======\n" << endl;
		//		return;
	}
}

//Function that create ouput file
void CreateOutputFile()
{
	char buff[100];
	time_t now = time(0);
	strftime(buff, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));

	myfile.open(outfile_exampl); //=====Open file for writing the result


	myfile << "== Open File:" << buff[0] << buff[1] << buff[2] << buff[3] << buff[4] << buff[5] << buff[6] << buff[7] << buff[8] << buff[9] << buff[10] << buff[11] << buff[12] << buff[13] << buff[14] << buff[15] << buff[16] << buff[17] << buff[18];

	myfile.close(); // close the open file
}

//Function that check integer input 
int checkInput(int choice, int In_min, int In_max)
{
	//Checking input value is an integer
	//while (cin.fail()) {
	//	cout << "\n==========================================================\n";
	//	cout << " ==== Please choose from the allowed options! =====\n\n";
	//	cout << " Error input" << endl;
	//	cin.clear();
	//	cin.ignore(256, '\n');
	//	cout << "\n Please choose again: ";
	//	cin >> choice;
	//}


	//input(choice);
	while (!cin)
	{
		cin.clear();
		//cout << "Please, enter only an INTEGER> " << flush;
		cout << "\n==========================================================\n";
		cout << " ==== Please choose from the allowed options! =====\n\n";
		cout << " Error input" << endl;
		cout << "\n Please choose again: ";
		cin >> input(choice);
	}

	if ((choice > In_max) || (choice < In_min))
	{
		cin.clear();
		cout << "\n==========================================================\n";
		cout << " ==== Please choose from the allowed options! =====\n\n";
		cout << " Error input" << endl;
		//cin.clear();
		//cin.ignore(256, '\n');
		cout << "\n Please choose again: ";
		cin >> input(choice);
	}

	return choice;
}

//Change the infilename
void SetInputFileName()
{
	cout << "   Enter input file name:";
	cin >> infile_exampl;

	//check if set infile exist
	if (fexists(infile_exampl))
	{
		//file opened successfully so we are here
		cout << "\n -> '" << infile_exampl << "' exist and is set as Input file!\n";
	}
	else //file could not be opened
	{
		cout << "\nFile '" << infile_exampl << "' not exist!\n";
		cout << "\n -> '" << infile_exampl << "' is create and set as Input file " << endl;

		char buff[100];
		time_t now = time(0);
		strftime(buff, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));

		myfile.open(infile_exampl); //=====Open file for writing the result
		myfile << "== Create File:" << buff[0] << buff[1] << buff[2] << buff[3] << buff[4] << buff[5] << buff[6] << buff[7] << buff[8] << buff[9] << buff[10] << buff[11] << buff[12] << buff[13] << buff[14] << buff[15] << buff[16] << buff[17] << buff[18];
		myfile.close(); // close the open file
	}
}

//Change the outfilename
void SeOutputFileName()
{
	cout << "   Enter output file name:";
	cin >> outfile_exampl;

	//bool check=exist(outfile_exampl);

	//if (myfile.is_open())
	if (fexists(outfile_exampl))
	{
		//file opened successfully so we are here
		cout << "\n -> '" << outfile_exampl << "' exist and is set as Output file!\n";
	}
	else //file could not be opened
	{
		cout << "\n -> '" << outfile_exampl << "' is create and is set as Output file ======\n" << endl;

		char buff[100];
		time_t now = time(0);
		strftime(buff, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));

		myfile.open(outfile_exampl); //=====Open file for writing the result
		myfile << "== Create File:" << buff[0] << buff[1] << buff[2] << buff[3] << buff[4] << buff[5] << buff[6] << buff[7] << buff[8] << buff[9] << buff[10] << buff[11] << buff[12] << buff[13] << buff[14] << buff[15] << buff[16] << buff[17] << buff[18];
		myfile.close(); // close the open file
	}
}