
//PLEASE USE THE PRECOMPILE HEADERS ACCORDING TO THE VERSION. #include "pch.h" may not work for 2017 and earlier versions. 
//I don't have any idea why the header 'stdafx.h' worked for me though i'm using the new version. Hence i'm not adding any headers.
//Add the precompile header at the top of any headers


#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>

#define BUFSIZE 512

DWORD WINAPI InstanceThread(LPVOID); 
VOID GetAnswerToRequest(LPTSTR, LPTSTR, LPDWORD); 

int _tmain(VOID) //_tmain is for redefining compiler among unicode or ascii. 
{ 
   BOOL   Conn = FALSE; 
   DWORD  TID = 0; 
   HANDLE Pipe_handle = INVALID_HANDLE_VALUE, hThread = NULL; 
   LPTSTR Pipe_Name = TEXT("\\\\.\\pipe\\mynamedpipe"); 



   for (;;) 
   { 
      _tprintf( TEXT("\nPipe Server: Main thread waiting for the client connection on %s\n"), Pipe_Name);
      Pipe_handle = CreateNamedPipe( 
          Pipe_Name,             // This is the name of the pipe 
          PIPE_ACCESS_DUPLEX,       // This is the duplex for read and write access(this is full duplex) 
          PIPE_TYPE_MESSAGE |       // message type pipe 
          PIPE_READMODE_MESSAGE |   // message-read mode 
          PIPE_WAIT,                // blocking mode 
          PIPE_UNLIMITED_INSTANCES, // max. instances  
          BUFSIZE,                  // output buffer size 
          BUFSIZE,                  // input buffer size 
          0,                        // client time-out 
          NULL);                    // default security attribute 

      if (Pipe_handle == INVALID_HANDLE_VALUE) 
      {
          _tprintf(TEXT("CreateNamedPipe failed, error code value=%d.\n"), GetLastError()); 
          return -1;
      }

      

      Conn = ConnectNamedPipe(Pipe_handle, NULL) ? 
         TRUE : (GetLastError() == ERROR_PIPE_CONNECTED); 

      if (Conn) 
      { 
         printf("Client connected, creating a processing thread.\n"); 

         // Create a thread for this client. 
         hThread = CreateThread( 
            NULL,              // no security attribute 
            0,                 // default size of stack
            InstanceThread,    // thread proc
            (LPVOID) Pipe_handle,    // thread parameter 
            0,                 // not suspended 
            &TID);      // returns thread ID 

         if (hThread == NULL) 
         {
            _tprintf(TEXT("CreateThread failed, error code value=%d.\n"), GetLastError()); 
            return -1;
         }
         else CloseHandle(hThread); 
       } 
      else 
        //closing the pipe since client could not connect.
         CloseHandle(Pipe_handle); 
   } 

   return 0; 
} 

DWORD WINAPI InstanceThread(LPVOID lpvParam)
//This operation reads the input from the client and replies to client.

{ 
   HANDLE hHeap      = GetProcessHeap();
   TCHAR* pchRequest = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));
   TCHAR* pchReply   = (TCHAR*)HeapAlloc(hHeap, 0, BUFSIZE*sizeof(TCHAR));

   DWORD B_Read = 0, Reply_Bytes = 0, Write_Bytes = 0; 
   BOOL fSuccess = FALSE;
   HANDLE Pipe_handle  = NULL;

   
  

   if (lpvParam == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   Unexpected NULL value");
       printf( "   InstanceThread exitting.\n");
       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
       return (DWORD)-1;
   }

   if (pchRequest == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   unexpected NULL memory allocation.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchReply != NULL) HeapFree(hHeap, 0, pchReply);
       return (DWORD)-1;
   }

   if (pchReply == NULL)
   {
       printf( "\nERROR - Pipe Server Failure:\n");
       printf( "   unexpected NULL memory allocation.\n");
       printf( "   InstanceThread exitting.\n");
       if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
       return (DWORD)-1;
   }

  
   printf("InstanceThread created, receiving and processing messages.\n");

// The thread's parameter is a handle to the object instance of pipe. 

   Pipe_handle = (HANDLE) lpvParam; 

// Loop until the reading operation is done.
   while (1) 
   { 
   // Read client requests from the pipe, according to buffer size.
      fSuccess = ReadFile( 
         Pipe_handle,        // handle to pipe 
         pchRequest,    // buffer to receive data 
         BUFSIZE*sizeof(TCHAR), // buffer size 
         &B_Read, // number of bytes read 
         NULL);        

      if (!fSuccess || B_Read == 0)
      {   
          if (GetLastError() == ERROR_BROKEN_PIPE)
          {
              _tprintf(TEXT("InstanceThread: client disconnected.\n"), GetLastError()); 
          }
          else
          {
              _tprintf(TEXT("InstanceThread ReadFile failed, error code value=%d.\n"), GetLastError()); 
          }
          break;
      }

   // incoming message processing.
      GetAnswerToRequest(pchRequest, pchReply, &Reply_Bytes); 

   // Writing reply to the pipe.
      fSuccess = WriteFile( 
         Pipe_handle,        // handle to pipe 
         pchReply,     // buffer to write from 
         Reply_Bytes, // number of bytes to write 
         &Write_Bytes,   // number of bytes written 
         NULL);        // not overlapped I/O 

      if (!fSuccess || Reply_Bytes != Write_Bytes)
      {   
          _tprintf(TEXT("InstanceThread WriteFile failed, error code value=%d.\n"), GetLastError()); 
          break;
      }
  }

// flush-to read client's version, before disconnecting.

//If you disconnect the pipe before flush, read operation doesn't work.

   FlushFileBuffers(Pipe_handle); 
   DisconnectNamedPipe(Pipe_handle); 
   CloseHandle(Pipe_handle); 

   HeapFree(hHeap, 0, pchRequest);
   HeapFree(hHeap, 0, pchReply);

   printf("InstanceThread exitting.\n");
   return 1;
}

VOID GetAnswerToRequest( LPTSTR pchRequest, 
                         LPTSTR pchReply, 
                         LPDWORD pch_Bytes )
// printing client's request to console.

{
    _tprintf( TEXT("Client Request String:\"%s\"\n"), pchRequest );

    //Checking the messege(outgoing)
    if (FAILED(StringCchCopy( pchReply, BUFSIZE, TEXT("Messege sending failed") )))
    {
        *pch_Bytes = 0;
        pchReply[0] = 0;
        printf("StringCchCopy failed, no outgoing message.\n");
        return;
    }
    *pch_Bytes = (lstrlen(pchReply)+1)*sizeof(TCHAR);
}