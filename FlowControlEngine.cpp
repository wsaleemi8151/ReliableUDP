/*
* FILE : FlowControlEngine.cpp
* PROJECT : SENG2040-22W-Sec1-Network Application Development - Assignment # 1
* PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
* FIRST VERSION : 2022-01-25
* DESCRIPTION :
* The functions in this file are used to control the flow of data transmission and 
	decide if the transfer mode is good or bad based on time it took to send and receive packets
*/

#include "FlowControlEngine.h"


#include <iostream> //for std::cout
#include <string.h> //for std::string
#include <fstream>
#include "MD5.h"

using std::cout; using std::endl;
using namespace std;

int main(int argc, char* argv[])
{
    //Start opening your file
    ifstream inBigArrayfile;
    inBigArrayfile.open("Data.dat", std::ios::binary | std::ios::in);

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

    return 0;
}
