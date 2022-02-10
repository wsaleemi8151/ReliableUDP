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




