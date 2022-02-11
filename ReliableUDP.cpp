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

#include "Net.h"
#include "FileIntegrityManager.h"
#include"MD5.h"

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
	string fileName;

	enum Mode
	{
		Client,
		Server
	};

	Mode mode = Server;
	Address address;
	string task;


	if (argc < 4)
	{
		printf("Error: Not enough commands passed\n");
		printf("Usage: IP{for server 0} FileName Task{-r(request) or -s(send)}\n");
	}
	else if (argc > 4)
	{
		printf("Error: More than required commands passed\n");
		printf("Usage: IP{for server 0} FileName Task{-r(request) or -s(send)}\n");
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
		}
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



	//Reading file for sending
	if (task == "-s")
	{
		// --------------------------------------------------------------------------------
		ifstream inBigArrayfile;
		inBigArrayfile.open(fileName, std::ios::binary | std::ios::in);

		//Find length of file
		inBigArrayfile.seekg(0, std::ios::end);
		long Length = inBigArrayfile.tellg();
		inBigArrayfile.seekg(0, std::ios::beg);

		//read in the data from your file
		char* InFileData = new char[Length];
		inBigArrayfile.read(InFileData, Length);
		inFileData = InFileData; //read file data stored here


		string strFileContent2(InFileData);
		strFileContent = strFileContent2;

		hash = CalculateMd5Hash(fileName);
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



		while (sendAccumulator > 1.0f / sendRate)
		{

			char data[DataSize];
			char status[25] = "Processing";
			unsigned char packet[PacketSize];


			if (strlen(inFileData) > transferredLength)
			{
				strcpy(status, "Processing");
				int currPacketSize = PacketSize - 35 - fileName.length() - strlen(status);
				string strPacket = strFileContent.substr(transferredLength, currPacketSize);
				transferredLength += currPacketSize;
				strcpy(data, (char*)strPacket.c_str());

				AddHeader((char*)fileName.c_str(), status, data);
				strcpy((char*)packet, data);
				connection.SendPacket(packet, sizeof(packet));
				sendAccumulator -= 1.0f / sendRate;
			}
			else
			{
				strcpy(status, "Done");
				strcpy(data, (char*)hash.c_str());

				AddHeader((char*)fileName.c_str(), status, data);
				strcpy((char*)packet, data);
				connection.SendPacket(packet, sizeof(packet));


				strcpy((char*)packet, "");
				connection.SendPacket(packet, sizeof(packet));
				sendAccumulator -= 1.0f / sendRate;
				break;
			}
		}





		while (true)
		{
			char data[DataSize];
			string fileContent;
			char status[25] = "Processing";


			unsigned char packet[PacketSize];

			int bytes_read = connection.ReceivePacket(packet, sizeof(packet));


			//strcpy(data, (char*)packet);
			char _fileName[150] = "";
			strcpy(status, "");
			strcpy(data, "");
			ExtractHeader(_fileName, status, (char*)packet, data);


			if (strcmp(status, "Done") == 0)
			{
				ofstream oFile;

				oFile.open("rev.txt", std::ios::binary | std::ios::out);
				oFile.write(fileContent.c_str(), fileContent.length());
				oFile.close();
				hash = CalculateMd5Hash("rev.txt");

				if (strcmp(hash.c_str(), data) == 0)
				{
					printf("File transfer successfully\n");

				}
				else
				{
					printf("File transfer failed\n");
				}
			}
			else if (strcmp(status, "Processing") == 0)
			{
				string data2(data);
				fileContent += data2;
			}


			if (bytes_read == 0)
			{

				break;

				/*
					Here when reading received packet is done,
					it should validate that received packet followed the ReliableUDP protocols
					also, it should also check
					if the mode is server or client and task is receive a file,
					then according to ReliableUDP protocol,
					it must split file meta info every chunk received
					and deserialize file meta info to obtain

					File meta info must have:-
					FileName:		It is the name of the file that is being transferred.
					ContentSize:	It is the size if the actual content data being sent.
					ContentType:	It is the content type of the data so that the server knows how to store the file.
					Status:			It is the status code exchange between the client and server.
					FileDigest:		It is the verification string generated by the appropriate hashing method.

					File meta info must be a serializable string
					and must follow the sequence: ...fileName...|-|...contentsize...|-|...contenttype...|-|...status...|-|...filedigest
					where "|-|" delimiter to separate attribute values of meta info
				*/

			}
			else
			{

				printf("%s\n", packet);
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