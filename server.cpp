//for better understanding I've done this stepwise

//PLEASE USE THE PRECOMPILE HEADERS ACCORDING TO THE VERSION. #include "pch.h" may not work for 2017 and earlier versions. 
//I don't have any idea why the header 'stdafx.h' worked for me though i'm using the new version. Hence i'm not adding any headers.
//Add the precompile header at the top of any headers 

#include <windows.h>
#include <iostream>

using namespace std;

int main()
{
	//............Initializations.....................................................//



	std::cout << "\t\t.... NAMED PIPE SERVER ...." << std::endl;
	//Named pipe local variable
	HANDLE namedpipe;
	char inputbuffer[1023];
	char outputbuffer[1023];
	DWORD dwinput = sizeof(inputbuffer);//please note that dword is specific to microsoft windows
	DWORD dwoutput = sizeof(outputbuffer);

	//ConnectNamedPipe local variable
	bool bconnectnamedpipe;

	//writefile local variable
	bool bWriteFile;
	char writefilebuffer[1023] = "Hello From NamedPipe Server!!";
	DWORD writebuffersize = sizeof(writefilebuffer);
	DWORD nobyteswrite;

	//flushbuffer
	bool flushfilebuffer;
	//Readfile local variable
	bool breadfile;
	char readfilebuffer[1023];
	DWORD readbuffersize = sizeof(readfilebuffer);
	DWORD nobytesread;


	//...............................................................................................//


	//STEP-1 create namedpipe

	namedpipe = CreateNamedPipe(L"\\\\.\\pipe\\MYNAMEDPIPE", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, dwoutput, dwinput, 0, NULL);
	if (namedpipe == INVALID_HANDLE_VALUE)
	{
		std::cout << "namedpipe creation failer" << GetLastError() << std::endl;
	}
	std::cout << "NamedPipe Creation Success" << std::endl;

	//STEP-2 Connect namedpipe
	bconnectnamedpipe = connectnamedpipe(namedpipe, NULL);
	if (connectnamedpipe == false)
	{
		std::cout << "connection failed==" << GetLastError() << std::endl;
	}

	std::cout << "C0nnection Success" << std::endl;

	//STEP-3 write file operatiion
	bWriteFile = writefile(namedpipe, writefilebuffer, writebuffersize, nobyteswrite, NULL);

	if (bWriteFile == false)
	{
		std::cout << "Writefile failed==" << GetLastError() << std::endl;
	}
	std::cout << "WriteFile Success" << std::endl;

	//STEP-4 flush operation

	flushfilebuffer = FlushFileBuffer(namedpipe);
	if (flushfilebuffer == false)
	{
		std::cout << "flushfile failed==" << GetLastError() << std::endl;
	}
	std::cout << "Flushfile Buffer Success" << std::endl;


	//STEP-5 read file operatiion
	breadfile = writefile(namedpipe, readfilebuffer, readbuffersize, nobytesread, NULL);

	if (breadfile == false)
	{
		std::cout << "readfile failed==" << GetLastError() << std::endl;
	}
	std::cout << "ReadFile Success" << std::endl;
	std::cout << "DATA READING FROM CLIENT->" << readfilebuffer << std::endl;

	//Step-6 disconnect namedpipe
	DisconnectNamedPipe(namedpipe);
	CloseHandle(namedpipe);
	system("pause");
	return 0;
}
