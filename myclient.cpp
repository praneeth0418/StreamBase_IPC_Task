
//PLEASE USE THE PRECOMPILE HEADERS ACCORDING TO THE VERSION. #include "pch.h" may not work for 2017 and earlier versions. 
//I don't have any idea why the header 'stdafx.h' worked for me though i'm using the new version. Hence i'm not adding any headers.
//Add the precompile header at the top of any headers


#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>

#define BUFSIZE 512

int _tmain(int arg, TCHAR *argv[])//_tmain is for redefining compiler among unicode or ascii. 
{ 
   HANDLE Pipe_handle; 
   LPTSTR MSG=TEXT("message from client."); 
   TCHAR  Buff[BUFSIZE]; 
   BOOL   fSuccess = FALSE; 
   DWORD  readch, Writeop, Write_Bytes, dword_mode; 
   LPTSTR Pipe_Name = TEXT("\\\\.\\pipe\\mynamedpipe"); 

   if( arg > 1 )
      MSG = argv[1];

// Trying to open a named pipe. 

   while (1) 
   { 
      Pipe_handle = CreateFile( 
         Pipe_Name,   // pipe name 
         GENERIC_READ |  // read and write access 
         GENERIC_WRITE, 
         0,               
         NULL,           // default security attributes
         OPEN_EXISTING,  // opens existing pipe 
         0,              // default attributes 
         NULL);          // no template file 

   // Break if the pipe handle is valid. 

      if (Pipe_handle != INVALID_HANDLE_VALUE) 
         break; 

      // if an error other than ERROR_PIPE_BUSY occurs, exit!. 

      if (GetLastError() != ERROR_PIPE_BUSY)//retrieves last error code value of calling thread
      {
         _tprintf( TEXT("Could not open pipe. error code value=%d\n"), GetLastError() ); 
         return -1;
      }

		//wait time is set for 20 seconds. this is user defined. If there are more incomings, increase the wait time.

      if ( ! WaitNamedPipe(Pipe_Name, 20000)) 
      { 
         printf("Could not open pipe: 20 second wait timed out."); 
         return -1;
      } 
   } 

// pipe connected; change to read mode. 

   dword_mode = PIPE_READMODE_MESSAGE; 
   fSuccess = SetNamedPipeHandleState( 
      Pipe_handle,    // pipe handle 
      &dword_mode,  // new pipe mode 
      NULL,     // don't set maximum bytes 
      NULL);    // don't set maximum time 
   if ( ! fSuccess) 
   {
      _tprintf( TEXT("SetNamedPipeHandleState failed. error code value=%d\n"), GetLastError() ); 
      return -1;
   }

// Send a message to the pipe server. 

   Writeop = (lstrlen(MSG)+1)*sizeof(TCHAR);
   _tprintf( TEXT("Sending %d byte message: \"%s\"\n"), Writeop, MSG); 

   fSuccess = WriteFile( 
      Pipe_handle,                  // pipe handle 
      MSG,             // message 
      Writeop,              // message length 
      &Write_Bytes,             // bytes written 
      NULL);                  // not overlapped 

   if ( ! fSuccess) 
   {
      _tprintf( TEXT("WriteFile to pipe failed. error code value=%d\n"), GetLastError() ); 
      return -1;
   }

   printf("\nMessage sent to server, receiving reply as follows:\n");

   do 
   { 
   // Read from the pipe. 

      fSuccess = ReadFile( 
         Pipe_handle,    // pipe handle 
         Buff,    // Buffer to receive reply 
         BUFSIZE*sizeof(TCHAR),  // Buffer size 
         &readch,  // number of bytes read 
         NULL);    

      if ( ! fSuccess && GetLastError() != ERROR_MORE_DATA )
         break; 

      _tprintf( TEXT("\"%s\"\n"), Buff ); 
   } while ( ! fSuccess);  // repeat loop if ERROR_MORE_DATA 

   if ( ! fSuccess)
   {
      _tprintf( TEXT("ReadFile failed. error code value=%d\n"), GetLastError() );
      return -1;
   }

   printf("\nEnd of message, please press ENTER to terminate the connection and exit !");
   _getch();

   CloseHandle(Pipe_handle); 

   return 0; 
}