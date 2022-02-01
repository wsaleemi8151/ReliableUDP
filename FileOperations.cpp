/*
* FILE : FileOperations.cpp
* PROJECT : SENG2040-22W-Sec1-Network Application Development - Assignment # 1
* PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
* FIRST VERSION : 2022-01-25
* DESCRIPTION :
* The functions in this file are used to handle reading/writing files to/from the file system
*/


#include "FileOperations.h"

// Sample code retrieved from : https://gist.github.com/haseeb-heaven/ee3f5931f7951106bea550c119a68c57

#include <iostream> /*For basic I/O.*/
#include <fstream> /*For basic I/O.*/
#include <sstream> /*For basic I/O.*/
#include <vector> /*For STL vector.*/
#include <tuple>  /*For packing multi data.*/

using std::string;
std::tuple<std::vector<uint8_t>, string, string> ReadFile(string, int);
bool WriteFile(string file_name, string, int);

#define BINARY_FILE 0
#define ASCII_FILE 1
#define HEX_FILE 2

/*
Example of binary file:
	auto file_type = ReadFile("user_data.dat", BINARY_FILE);
	auto file_data = std::get<BINARY_FILE>(file_type);
Example of ASCII file:
	auto file_type = ReadFile("my_file.txt", ASCII_FILE);
	auto file_data = std::get<ASCII_FILE>(file_type);
*/

std::tuple<std::vector<uint8_t>, string, string> ReadFile(string file_name, int file_type)
{
	/*Buffers to store output data from file.*/
	string str_buf, hex_buf;
	std::vector<uint8_t> vec_buf;

	try {
		if (file_type == BINARY_FILE) {

			/*Open the stream in binary mode.*/
			std::ifstream bin_file(file_name, std::ios::binary);

			if (bin_file.good()) {
				/*Read Binary data using streambuffer iterators.*/
				std::vector<uint8_t> v_buf((std::istreambuf_iterator<char>(bin_file)), (std::istreambuf_iterator<char>()));
				vec_buf = v_buf;
				bin_file.close();
			}

			else {
				throw std::exception();
			}

		}

		else if (file_type == ASCII_FILE) {

			/*Open the stream in default mode.*/
			std::ifstream ascii_file(file_name);
			string ascii_data;

			if (ascii_file.good()) {
				/*Read ASCII data using getline*/
				while (getline(ascii_file, ascii_data))
					str_buf += ascii_data + "\n";

				ascii_file.close();
			}
			else {
				throw std::exception();
			}
		}

		else if (file_type == HEX_FILE) {

			/*Open the stream in default mode.*/
			std::ifstream hex_file(file_name);

			if (hex_file.good()) {
				/*Read Hex data using streambuffer iterators.*/
				std::vector<char> h_buf((std::istreambuf_iterator<char>(hex_file)), (std::istreambuf_iterator<char>()));
				string hex_str_buf(h_buf.begin(), h_buf.end());
				hex_buf = hex_str_buf;

				hex_file.close();
			}
			else {
				throw std::exception();
			}
		}

	}

	catch (std::exception const& ex) {
		string ex_str = "Error: " + file_name + ": No such file or directory";
		throw std::exception(ex_str.c_str());
	}

	auto tuple_data = make_tuple(vec_buf, str_buf, hex_buf);
	return tuple_data;
}

/**
 * @description - Write data to any ASCII,BINARY or HEX file.
 * @param - Source file name, file data and file type.
 * @return - Returns true on successfully written data otherwise returns false.
 */

bool WriteFile(string file_name, string file_data, int file_type)
{
	bool write_status = false;
	std::ofstream out_stream(file_name, (file_type == ASCII_FILE) ? std::ios::out : (file_type == BINARY_FILE || file_type == HEX_FILE) ? std::ios::binary : std::ios::out);

	if (out_stream.is_open()) {
		if (file_type == ASCII_FILE) {
			out_stream << file_data;
			write_status = true;
		}

		else if (file_type == BINARY_FILE) {
			out_stream << file_data;
			write_status = true;
		}

		else if (file_type == HEX_FILE) {
			int bin_data;
			std::stringstream ss;
			ss << std::hex << file_data;

			while (ss >> bin_data)
				out_stream.write(reinterpret_cast<const char*>(&bin_data), sizeof(char));

			write_status = true;
		}
		out_stream.close();
	}
	else {
		string ex_str = "Error: couldn't open " + file_name + " for output";
		throw std::exception(ex_str.c_str());
	}
	return write_status;
}
