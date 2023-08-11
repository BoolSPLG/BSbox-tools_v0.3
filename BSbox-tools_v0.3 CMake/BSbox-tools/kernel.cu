////////////////////////////////////////////////////////////////////////////
//
// Copyright @2017-2023 Dusan and Iliya.  All rights reserved.
//
// Please refer to the NVIDIA end user license agreement (EULA) associated
// with this source code for terms and conditions that govern your use of
// this software. Any use, reproduction, disclosure, or distribution of
// this software and related documentation outside the terms of the EULA
// is strictly prohibited.
//
////////////////////////////////////////////////////////////////////////////
//Example for using GPU BoolSPLG library procedures for Boolean function  
////////////////////////////////////////////////////////////////////////////

//System includes
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <sstream>
#include <fstream>
#include <time.h>

// CUDA runtime
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

//Main Library header file
#include <BoolSPLG/BoolSPLG_v03.cuh> //#include "BoolSPLG_v0.cuh" 

//Output file 
std::ofstream myfile;

//Declaration strings for input and output files names
std::string infile_exampl = "infile_exampl";
std::string outfile_exampl = "outfile_exampl";

//@Global Time computation variables
double elapsedTime_computation_part = 0, elapsedTime_computation = 0;

//@@Global Size variable
int sizeB = 0;

//@@Global Bool variables
int sizeBool;

//Declaration host vectors for Boolean function
int* host_Bool_TT, * host_Bool_PTT, * host_GPUbool_rez, * walshvec_cpu, * rf_cpu, * anf_cpu;

//Declaration device vector for Boolean function
int* device_Bool, * device_Bool_TT, * device_Bool_PTT, * device_Bool_rez;

//@Declaration device vectors for Boolean Bitwise (ANF) function
unsigned long long int* device_NumIntVecTT, * device_NumIntVecANF;
int* device_max_values_AD;

//host vectors for S-box
int* LAT, * ACT, * ANF, * CF, * DDT, * PTT, * TT, * t, * WHT, * AC_all, * ANF_S, * DEG_ALL, * delta_ALL, * binary_num;

//@Declaration device S-box vectors
int* device_Sbox, * device_CF, * device_LAT, * device_ACT, * device_ANF, * device_DDT;

//@Declaration host vectors for Boolean Bitwise (ANF) function
unsigned long long int* NumIntVecTT, * NumIntVecANF; 
int* host_max_values_AD;

//@Declaration of host - device vector for Bitwise ANF computation (S-box)
unsigned long long int* device_NumIntVecCF, * host_NumIntVecCF;

//@Declaration of host - device vector 
int* host_CF, * host_max_values, * device_Vec_max_values;

//@@S-box variables
int sizeSbox, binary = 0;

int* BoolElemet, * SboxElemet, * CPU_CF;

//@@Variable borders
int nl_gpuBorder = 0, AC_gpuBorder = 0, deg_gpuBorder = 0, delta_gpuBorder = 0;
int nl_cpuBorder = 0, AC_cpuBorder = 0, deg_cpuBorder = 0, delta_cpuBorder = 0;

//@@Global variable
int choice1, choice2;

//function menu
void menu();
void mainMenu();

void menuCPU();
void mainMenuCPU();

void CUDA_Properties();
void CUDA_PropertiesMenu();

void CPU_boolMenu();
void CPU_bool();

void GPU_boolMenu();
void GPU_bool();

void CPU_SboxMenu();
void CPU_Sbox();

void GPU_SboxMenu();
void GPU_Sbox();

void findCPU_boolMenu();
void findCPU_bool();

void findGPU_boolMenu();
void findGPU_bool();

void findCPU_sboxMenu();
void findCPU_sbox();

void findGPU_sboxMenu();
void findGPU_sbox();

//Help Heder file - Input Output file
#include "InputOutputFiles.h"

//Help Header file - Time functions
#include "TimeHeader.h"

////Help Heder file - CPU computing boolean functions properties
//#include "func_Boolean_CPU.h"
//#include "funct_Sbox_CPU.h"
//
////Heder file 2D DynamicArray
////#include "2D_DynamicArray.h"

//Help Heder file boolean computation functions
#include "HelpBoolFunctionCPU.h"
#include "HelpBoolFunctionGPU.h"


#include "HelpFindBoolFunctionGPU.h"
#include "HelpFindBoolFunctionCPU.h"

//Help Heder file S-box computation functions
#include "HelpSboxFunctionCPU.h"
#include "HelpSboxFunctionGPU.h"

#include "HelpFindSboxFunctionCPU.h"
#include "HelpFindSboxFunctionGPU.h"

//Header file that contain S-box generation
#include "GenBoolean.h"
#include "GenSboxes.h"

//Menu Heder CPU - GPU Bool Properties
#include "ComputePropMenuBoolCPU.h"
#include "ComputePropMenuBoolGPU.h"


//Menu Heder CPU - GPU S-box Properties
#include "ComputePropMenuSboxCPU.h"
#include "ComputePropMenuSboxGPU.h"

// === Menu Heders files ===
#include "MainMenuCPU.h"
#include "MainMenu.h"

//Menu Heders CUDA Properties
#include "CUDA_Properties_menu.h"

#include "FindMenuBoolCPU.h"
#include "FindMenuBoolGPU.h"
#include "FindMenuSboxCPU.h"
#include "FindMenuSboxGPU.h"

//Help Heder file - Check for hardware Requires
#include "CheckRequires.h"

using namespace std;

int main()
{
	cout << "==========================================================";
	printf("\nBSbox-tools, Software Boolean S-box Cryptographic Properties Tool for GPUs.\n");
	printf("\n   Current release: v0.3 \n");

	//Function create Output files
	CreateOutputFile();

	cout << "\n==========================================================\n";
	//Function Check Hardware Requires, and to switch to CPU menu if does not have GPU
	BoolSPLGMinimalRequires1();
	cout << "\n==========================================================\n";

	//Function Open Main Menu
	menu();

	return 0;
}