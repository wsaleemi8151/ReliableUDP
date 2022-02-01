/*
* FILE : FileIntegrityManager.cpp
* PROJECT : SENG2040-22W-Sec1-Network Application Development - Assignment # 1
* PROGRAMMER : Gursharan Singh - Waqar Ali Saleemi
* FIRST VERSION : 2022-01-25
* DESCRIPTION :
* The functions in this file are used to check and verify the file transfer integrity
*/


#include "FileIntegrityManager.h"

#include<string.h>
#include<stdlib.h>
#pragma warning (disable:4996)


int AddHeader(char* fileName, char* contentType, char* contentSize, char* fileDigest, char* data)
{
	char temp[5000] = " ";
	char delimiter = '#';

	strcat(temp, fileName);
	strncat(temp, &delimiter, 1);
	strcat(temp, contentType);
	strncat(temp, &delimiter, 1);
	strcat(temp, contentSize);
	strncat(temp, &delimiter, 1);
	strcat(temp, fileDigest);
	strncat(temp, &delimiter, 1);
	strcat(temp, data);


	strcpy(data, temp);

	return 0;
}

int ExtractHeader(char* fileName, char* contentType, char* contentSize, char* fileDigest, char* data)
{
	strcpy(fileName, "");
	strcpy(contentType, "");
	strcpy(contentSize, "");
	strcpy(fileDigest, "");
	strcpy(data, "");

	char temp[5000] = " ";
	char delimiter = '#';

	char c = 'a';
	int i = 1;
	int j = 1;
	while (c != '\0')
	{
		c = data[i];

		if (j != 5)
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
			strncat(contentType, &c, 1);
		}
		else if (j == 3)
		{
			strncat(contentSize, &c, 1);
		}
		else if (j == 4)
		{
			strncat(fileDigest, &c, 1);
		}
		else if (j == 5)
		{
			strncat(data, &c, 1);
		}

		i++;
	}

	return 0;
}




