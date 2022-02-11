#pragma once

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


void ReadFile(string fileName, char* content);
void WritetoFile(string fileName, char* content, long length);

//id WriteFile(string fileName, char* content);



class FileOperations
{
};

