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



	std::cout << "\t\t... NAMED PIPE CLIENT ...." << std::endl;
	//defining local variable
	HANDLE hCreatefile;
	

	//writefile local variable
	bool bWriteFile;
	char writefilebuffer[1023] = "Hello From NamedPipe Client!!";
	DWORD writebuffersize = sizeof(writefilebuffer);
	DWORD nobyteswrite;

	
	//Readfile local variable
	bool breadfile;
	char readfilebuffer[1023];
	DWORD readbuffersize = sizeof(readfilebuffer);
	DWORD nobytesread;


	//...............................................................................................//


	// create file

	hCreatefile= CreateNamedPipe(L"\\\\.\\pipe\\MYNAMEDPIPE", GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if (INVALID_HANDLE_VALUE==hCreatefile)
	{
		std::cout << "FILE creation failed" << GetLastError() << std::endl;
	}
	std::cout << "File Creation Success" << std::endl;

	// read file operatiion
	breadfile = writefile(hCreatefile, readfilebuffer, readbuffersize, nobytes, NULL);

	if (breadfile == false)
	{
		std::cout << "readfile failed==" << GetLastError() << std::endl;
	}
	std::cout << "ReadFile Success" << std::endl;
	std::cout << "DATA READING FROM SERVER->" << readfilebuffer << std::endl;

	bWriteFile = writefile(hCreatefile, writefilebuffer, writebuffersize, nobyteswrite, NULL);

	if (bWriteFile == false)
	{
		std::cout << "Writefile failed==" << GetLastError() << std::endl;
	}
	std::cout << "WriteFile Success" << std::endl;

	CloseHandle(namedpipe);
	system("pause");
	return 0;
}
