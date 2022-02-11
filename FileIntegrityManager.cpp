/*
* FILE : FileIntegrityManager.cpp
* PROJECT : SENG2040-22W-Sec1-Network Application Development - Assignment # 1
* PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
* FIRST VERSION : 2022-01-25
* DESCRIPTION :
* The functions in this file are used to check and verify the file transfer integrity
*/



#include<string.h>
#include<stdlib.h>
#include <iostream> //for std::cout
#include <string.h> //for std::string
#include <fstream>
#include "MD5.h"
#include "FileIntegrityManager.h"


using std::cout; using std::endl;
using namespace std;

#pragma warning (disable:4996)


int AddHeader(char* fileName, char* transferStatus, char* length, char* data)
{
	char temp[5000] = " ";
	char delimiter = '#';

	strcat(temp, fileName);
	strncat(temp, &delimiter, 1);
	strcat(temp, transferStatus);
	strncat(temp, &delimiter, 1);
	strcat(temp, length);
	strncat(temp, &delimiter, 1);
	strcat(temp, data);


	strcpy(data, temp);

	return 0;
}

int ExtractHeader(char* fileName, char* transferStatus, unsigned char* input, char* length, char* data)
{


	char delimiter = '#';

	char c = 'a';
	int i = 1;
	int j = 1;
	while (c != '\0')
	{
		c = input[i];

		if (j != 4)
		{
			if (c == delimiter)
			{
				j++;
				i++;
				continue;
			}
		}

		if (j == 1)
		{
			strncat(fileName, &c, 1);
		}
		else if (j == 2)
		{
			strncat(transferStatus, &c, 1);
		}
		else if (j == 3)
		{
			strncat(length, &c, 1);
		}
		else if (j == 4)
		{
			strncat(data, &c, 1);
		}


		i++;
	}

	return 0;
}



string CalculateMd5Hash(string filename)
{
	//Start opening your file
	ofstream oFile;
	ifstream inBigArrayfile;
	inBigArrayfile.open(filename, std::ios::binary | std::ios::in);

	//Find length of file
	inBigArrayfile.seekg(0, std::ios::end);
	long Length = inBigArrayfile.tellg();
	inBigArrayfile.seekg(0, std::ios::beg);

	//read in the data from your file
	char* InFileData = new char[Length];
	inBigArrayfile.read(InFileData, Length);



	//Calculate MD5 hash
	std::string Temp = md5(InFileData, Length);
	cout << Temp.c_str() << endl;

	//Clean up
	delete[] InFileData;

	return Temp;
}

//
//string CalculateMd5Hash(string filename)
//{
//
//
//
//	//Start opening your file
//	ifstream inBigArrayfile;
//	inBigArrayfile.open(filename, std::ios::binary | std::ios::in);
//
//	//Find length of file
//	inBigArrayfile.seekg(0, std::ios::end);
//	long Length = inBigArrayfile.tellg();
//	inBigArrayfile.seekg(0, std::ios::beg);
//
//	//read in the data from your file
//	char* InFileData = new char[Length];
//	inBigArrayfile.read(InFileData, Length);
//
//
//
//	//Calculate MD5 hash
//	std::string Temp = md5(InFileData, Length);
//	cout << Temp.c_str() << endl;
//
//	//Clean up
//	delete[] InFileData;
//
//	return Temp;
//}


long ReadFileLength(string filename)
{
	ifstream inBigArrayfile;
	inBigArrayfile.open(filename, std::ios::binary | std::ios::in);

	//Find length of file
	inBigArrayfile.seekg(0, std::ios::end);
	long Length = inBigArrayfile.tellg();
	inBigArrayfile.seekg(0, std::ios::beg);

	

	return Length;
}


void ReadFiletoString(string filename, char* contents, long Length)
{
	ifstream inBigArrayfile;
	inBigArrayfile.open(filename, std::ios::binary | std::ios::in);

	//read in the data from your file
	//char* InFileData = new char[Length];
	//strcpy(InFileData, "");
	inBigArrayfile.read(contents, Length);
	//strcpy(contents, InFileData);//read file data stored here

}

