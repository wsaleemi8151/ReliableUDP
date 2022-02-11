/*
* FILE : FileOperations.cpp
* PROJECT : SENG2040-22W-Sec1-Network Application Development - Assignment # 1
* PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
* FIRST VERSION : 2022-01-25
* DESCRIPTION :
* The functions in this file are used to handle reading/writing files to/from the file system
*/


#include<string.h>
#include<stdlib.h>
#include <iostream> //for std::cout
#include <string.h> //for std::string
#include <fstream>
#include "MD5.h"
#include "FileOperations.h"


using std::cout; using std::endl;
using namespace std;
#pragma warning (disable:4996)


void ReadFile(string fileName, char* content)
{
	ifstream inBigArrayfile;
	inBigArrayfile.open(fileName, std::ios::binary | std::ios::in);

	//Find length of file
	inBigArrayfile.seekg(0, std::ios::end);
	long Length = inBigArrayfile.tellg();
	inBigArrayfile.seekg(0, std::ios::beg);

	//read in the data from your file
	char* InFileData = new char[Length];
	inBigArrayfile.read(InFileData, Length);
	strcpy(content, InFileData);
}

void WritetoFile(string fileName, char* content, long length)
{

	ofstream oFile;

	oFile.open(fileName, std::ios::binary | std::ios::out);
	oFile.write(content, length);
	oFile.close();
}