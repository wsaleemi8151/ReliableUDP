
#include<string.h>
#include<stdlib.h>
#include <iostream> //for std::cout
#include <string.h> //for std::string
#include <fstream>
#include "MD5.h"


using std::cout; using std::endl;
using namespace std;

#pragma warning (disable:4996)

int AddHeader(char* fileName, char* contentType, char* contentSize, char* fileDigest, char* data);
int ExtractHeader(char* fileName, char* contentType, char* contentSize, char* fileDigest, char* data);
string CalculateMd5Hash(string filename);	

#pragma once
class FileIntegrityManager
{
};

