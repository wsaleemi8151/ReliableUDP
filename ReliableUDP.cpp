/*
	Reliability and Flow Control Example
	From "Networking for Game Programmers" - http://www.gaffer.org/networking-for-game-programmers
	Author: Glenn Fiedler <gaffer@gaffer.org>
*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h> //for std::string
#include <thread>


#include "Net.h"
#include "FileIntegrityManager.h"
#include "FileOperations.h"
#include"MD5.h"

#include <chrono>
#include <thread>

//#define SHOW_ACKS

using namespace std;
using namespace net;

/*
	All these const should be set in Configuration file
	so it would be easier to change when deployed on any server or
	using it on some different computer.
*/
const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x11223344;
const float DeltaTime = 1.0f / 30.0f;
const float SendRate = 1.0f / 30.0f;
const float TimeOut = 10.0f;
const int PacketSize = 256;
const int DataSize = 256;


/*
	FlowControl class should be in a separate file named as FlowControl.cpp
*/
class FlowControl
{
public:

	FlowControl()
	{
		printf("flow control initialized\n");
		Reset();
	}

	void Reset()
	{
		mode = Bad;
		penalty_time = 4.0f;
		good_conditions_time = 0.0f;
		penalty_reduction_accumulator = 0.0f;
	}

	void Update(float deltaTime, float rtt)
	{
		const float RTT_Threshold = 250.0f;

		if (mode == Good)
		{
			if (rtt > RTT_Threshold)
			{
				printf("*** dropping to bad mode ***\n");
				mode = Bad;
				if (good_conditions_time < 10.0f && penalty_time < 60.0f)
				{
					penalty_time *= 2.0f;
					if (penalty_time > 60.0f)
						penalty_time = 60.0f;
					printf("penalty time increased to %.1f\n", penalty_time);
				}
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				return;
			}

			good_conditions_time += deltaTime;
			penalty_reduction_accumulator += deltaTime;

			if (penalty_reduction_accumulator > 10.0f && penalty_time > 1.0f)
			{
				penalty_time /= 2.0f;
				if (penalty_time < 1.0f)
					penalty_time = 1.0f;
				printf("penalty time reduced to %.1f\n", penalty_time);
				penalty_reduction_accumulator = 0.0f;
			}
		}

		if (mode == Bad)
		{
			if (rtt <= RTT_Threshold)
				good_conditions_time += deltaTime;
			else
				good_conditions_time = 0.0f;

			if (good_conditions_time > penalty_time)
			{
				printf("*** upgrading to good mode ***\n");
				good_conditions_time = 0.0f;
				penalty_reduction_accumulator = 0.0f;
				mode = Good;
				return;
			}
		}
	}

	float GetSendRate()
	{
		return mode == Good ? 30.0f : 10.0f;
	}

private:

	enum Mode
	{
		Good,
		Bad
	};

	Mode mode;
	float penalty_time;
	float good_conditions_time;
	float penalty_reduction_accumulator;
};










// ----------------------------------------------
using namespace std;

int main(int argc, char* argv[])
{

	/*char* content;
	long length = ReadFileLength("Sc.png");

	content = new char[length];


	ReadFiletoString("Sc.png", content, length);
	WritetoFile("Scc.png", content, length);*/
	//ofstream oFile;
	//
	//oFile.open("Scc.png", std::ios::binary | std::ios::out);
	//oFile.write(content, length);
	//oFile.close();


	/*CalculateMd5Hash("Sc.png");

	printf("content -> %s\n", content);*/

	string fileName;

	// Record start time
	auto start = std::chrono::high_resolution_clock::now();

	enum Mode
	{
		Client,
		Server
	};

	Mode mode = Server;
	Address address;
	string task;


	if (argc != 2 && argc != 4)
	{
		printf("Error: Not enough commands passed\n");
		printf("Usage: Run As Server:- ReliableUDP.exe 0 ---- Run As Client:- ReliableUDP.exe ServerIP FileName Task{-r(request) or -s(send)}]\n");
	}
	else
	{
		if (strcmp(argv[1], "0") == 0)
		{
			mode = Server;
		}
		else
		{
			int a, b, c, d;

			/*
				Here using command line arguments,
				we need to implement a machenism by which
				- app mode can be set to server or client
				- other end ip address
				- file transfer or receiver flag
				- if file transfer then source file path
				- if file receiving then target file path where file will be save once completed
			*/
			if (sscanf(argv[1], "%d.%d.%d.%d", &a, &b, &c, &d))
			{
				mode = Client;
				address = Address(a, b, c, d, ServerPort);
			}
			else
			{
				printf("Error: Not enough or incorrect commands passed\n");
				printf("Usage: Run As Server:- ReliableUDP.exe 0 ---- Run As Client:- ReliableUDP.exe ServerIP FileName Task{-r(request) or -s(send)}]\n");
			}
		}

		if (mode == Client)
		{
			string fileName2(argv[2]);
			fileName = fileName2;

			task = argv[3];
			if (task == "-r")
			{
				printf("Requesting file...\n");
			}
			else
			{
				printf("Sending file...\n");
			}
		}
	}





	// initialize
	if (!InitializeSockets())
	{
		printf("failed to initialize sockets\n");
		return 1;
	}

	ReliableConnection connection(ProtocolId, TimeOut);

	const int port = mode == Server ? ServerPort : ClientPort;

	if (!connection.Start(port))
	{
		printf("could not start connection on port %d\n", port);
		return 1;
	}

	if (mode == Client)
		connection.Connect(address);
	else
		connection.Listen();

	bool connected = false;
	float sendAccumulator = 0.0f;
	float statsAccumulator = 0.0f;

	FlowControl flowControl;
	int packets_Sent = 1;

	std::string hash = "";
	char* inFileData = NULL; //read file data stored here
	string strFileContent;
	long transferredLength = 0;
	long length;
	char* content = NULL;
	string strLength;

	

	bool isFinishedTransfer = false;


	//Reading file for sending
	if (task == "-s")
	{

		length = ReadFileLength(fileName);
		content = new char[length];
		ReadFiletoString(fileName, content, length);

		strLength = to_string(length);
		

		// --------------------------------------------------------------------------------

		//ifstream inBigArrayfile;
		//inBigArrayfile.open(fileName, std::ios::binary | std::ios::in);

		////Find length of file
		//inBigArrayfile.seekg(0, std::ios::end);
		//long Length = inBigArrayfile.tellg();
		//inBigArrayfile.seekg(0, std::ios::beg);

		////read in the data from your file
		//char* InFileData = new char[Length];
		//inBigArrayfile.read(InFileData, Length);
		//inFileData = InFileData; //read file data stored here


		string strFileContent2(content);
		strFileContent = strFileContent2;

		hash = CalculateMd5Hash(fileName);
		isFinishedTransfer = false;

		// --------------------------------------------------------------------------------
	
	}




	while (true)
	{
		// update flow control

		if (connection.IsConnected())
			flowControl.Update(DeltaTime, connection.GetReliabilitySystem().GetRoundTripTime() * 1000.0f);

		const float sendRate = flowControl.GetSendRate();

		// detect changes in connection state

		if (mode == Server && connected && !connection.IsConnected())
		{
			flowControl.Reset();
			printf("reset flow control\n");
			connected = false;
		}

		if (!connected && connection.IsConnected())
		{
			printf("client connected to server\n");
			connected = true;
		}

		if (!connected && connection.ConnectFailed())
		{
			printf("connection failed\n");
			break;
		}

		// send and receive packets

		sendAccumulator += DeltaTime;


		string strPacket = "";
		while (sendAccumulator > 1.0f / sendRate)
		{
			if (isFinishedTransfer)
			{
				break;
			}


			char data[DataSize];
			char status[25] = "Processing";
			unsigned char packet[PacketSize];


			if (content && strlen(content) > transferredLength)
			{
				strcpy(status, "Processing");
				int currPacketSize = PacketSize - 35 - fileName.length() - strlen(status);
				strPacket = strFileContent.substr(transferredLength, currPacketSize);
				transferredLength += strPacket.length();
				strcpy(data, (char*)strPacket.c_str());

				AddHeader((char*)fileName.c_str(), status, (char*)strLength.c_str(), data);
				strcpy((char*)packet, data);
				connection.SendPacket(packet, sizeof(packet));
				sendAccumulator -= 1.0f / sendRate;
				this_thread::sleep_for(chrono::milliseconds(100));


			}
			else
			{
				strcpy(status, "Done");
				strcpy(data, (char*)hash.c_str());

				AddHeader((char*)fileName.c_str(), status, (char*)strLength.c_str(), data);
				strcpy((char*)packet, data);
				connection.SendPacket(packet, sizeof(packet));


				strcpy((char*)packet, "");
				connection.SendPacket(packet, sizeof(packet));
				sendAccumulator -= 1.0f / sendRate;

				isFinishedTransfer = true;

				long completefilelen = atol(strLength.c_str());

				if (mode == Client)
				{
					// ----------------------------------------------
					// hard coded for now 
					double fileContentLengthInMegaBits = ((completefilelen * 8) / 1024) / 1024;
					// ----------------------------------------------

					auto finish = std::chrono::high_resolution_clock::now();
					std::chrono::duration<double> elapsed = finish - start;

					// number of seconds spent to transfer the file
					double timeSpent = elapsed.count();

					// transfer rate in megabits
					double transferRate = (fileContentLengthInMegaBits) / timeSpent;

					// 
					printf("\n\n------------------------------------------------------------------\n");
					printf("\t\tFile Transfer completed");
					printf("\n------------------------------------------------------------------\n\n");
					printf("\tFile Name: %s\n", fileName.c_str());
					printf("\tFile size (bytes): %d\n", completefilelen);
					printf("\tTime spent (in seconds): %02f\n", timeSpent);
					printf("\tFile transfer rate: %02f (megabits/sec)\n", transferRate);
					printf("\n------------------------------------------------------------------\n");


					/*
						cout << "Hello I'm waiting...." << endl;
						this_thread::sleep_for(chrono::milliseconds(20000) );
						cout << "Waited 20000 ms\n";
					
					*/
				}

				break;
			}
		}





		char data[DataSize];
		string fileContent;
		char status[25] = "Processing";
		long lengthrecv;
		char lengthgot[35];


		unsigned char packet[PacketSize];
		while (true)
		{

			int bytes_read = connection.ReceivePacket(packet, sizeof(packet));

			if (bytes_read == 0)
			{
				break;
			}
			else
			{
				printf("%s\n", packet);
			}

			//strcpy(data, (char*)packet);
			char _fileName[150] = "";
			strcpy(status, "");
			strcpy(data, "");
			strcpy(lengthgot, "");
			ExtractHeader(_fileName, status, packet, lengthgot, data);


			if (strcmp(status, "Done") == 0)
			{
				
				lengthrecv = atol(lengthgot);
				WritetoFile(_fileName, (char*)fileContent.c_str(), lengthrecv);
				/*ofstream oFile;

				oFile.open("okgood.txt", std::ios::binary | std::ios::out);
				oFile.write(fileContent.c_str(), fileContent.length());
				oFile.close();*/
				hash = CalculateMd5Hash(_fileName);

				if (strcmp(hash.c_str(), data) == 0)
				{
					printf("File transferred with no error.\n");

				}
				else
				{
					printf("File transferred with errors.\n");
				}


			}
			else if (strcmp(status, "Processing") == 0)
			{

				string data2(data);
				fileContent += data2;
				this_thread::sleep_for(chrono::milliseconds(100));

			}


		}

		// show packets that were acked this frame

#ifdef SHOW_ACKS
		unsigned int* acks = NULL;
		int ack_count = 0;
		connection.GetReliabilitySystem().GetAcks(&acks, ack_count);
		if (ack_count > 0)
		{
			printf("acks: %d", acks[0]);
			for (int i = 1; i < ack_count; ++i)
				printf(",%d", acks[i]);
			printf("\n");
		}
#endif

		// update connection

		connection.Update(DeltaTime);

		// show connection stats

		statsAccumulator += DeltaTime;

		while (statsAccumulator >= 0.25f && connection.IsConnected())
		{
			float rtt = connection.GetReliabilitySystem().GetRoundTripTime();

			unsigned int sent_packets = connection.GetReliabilitySystem().GetSentPackets();
			unsigned int acked_packets = connection.GetReliabilitySystem().GetAckedPackets();
			unsigned int lost_packets = connection.GetReliabilitySystem().GetLostPackets();

			float sent_bandwidth = connection.GetReliabilitySystem().GetSentBandwidth();
			float acked_bandwidth = connection.GetReliabilitySystem().GetAckedBandwidth();

			printf("rtt %.1fms, sent %d, acked %d, lost %d (%.1f%%), sent bandwidth = %.1fkbps, acked bandwidth = %.1fkbps\n",
				rtt * 1000.0f, sent_packets, acked_packets, lost_packets,
				sent_packets > 0.0f ? (float)lost_packets / (float)sent_packets * 100.0f : 0.0f,
				sent_bandwidth, acked_bandwidth);

			statsAccumulator -= 0.25f;
		}

		net::wait(DeltaTime);

	}

	ShutdownSockets();

	return 0;
}