/*
* FILE : FileIntegrityManager.h
* PROJECT : SENG2040-22W-Sec1-Network Application Development - Assignment # 1
* PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
* FIRST VERSION : 2022-01-25
* DESCRIPTION :
* Header file for prototypes
*/


#include<string.h>
#include<stdlib.h>
#include <iostream> //for std::cout
#include <string.h> //for std::string
#include <fstream>
#include "MD5.h"


using std::cout; using std::endl;
using namespace std;

#pragma warning (disable:4996)

int AddHeader(char* fileName, char* transferStatus, char* length, char* data);
int ExtractHeader(char* fileName, char* transferStatus, unsigned char* input, char* length, char* data);

string CalculateMd5Hash(string filename);	
long ReadFileLength(string filename);
void ReadFiletoString(string filename, char* contents, long Length);

#pragma once
class FileIntegrityManager
{
};

