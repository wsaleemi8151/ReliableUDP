/*
* FILE : FileOperations.h
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

void ReadFile(string fileName, char* content);
void WritetoFile(string fileName, char* content, long length);

class FileOperations
{
};

