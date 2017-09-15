#pragma once
#include <iostream>
#include <thread>
#include <string>
#include <WS2tcpip.h> // header file for winsock functions
#include "RequestHandler.h"
#pragma comment (lib, "ws2_32.lib")	// winsock lib file
#define MAX_BUFFER_SIZE (100*1024)

namespace Homecode
{
	namespace Software
	{
		class TCPServer
		{
		public:
			TCPServer(const TCPServer&) = delete;
			TCPServer& operator=(const TCPServer&) = delete;
			TCPServer(std::string ipAddress, int port, RequestReceivedHandler handler);
			~TCPServer();

			bool Init();									//initialize winsocket
			bool Send(int clientSocket, std::string msg);	//send msg to client
			void Run();										//main server processing
			void CleanUp();									//shut winsocket

		private:

			SOCKET CreateSocket();							//create a socket
			SOCKET WaitForConnection(SOCKET listenig);		//wait for connection
			void MainListeningThreadRun();					//run main server proccesing in new thread

			std::string					m_ipAddress;
			int							m_port;
			char						m_buffer[MAX_BUFFER_SIZE];
			RequestReceivedHandler		m_handler;
			std::thread					m_mainListeningThread;
			bool						m_mainListeningThreadRunningCondition;
		};
	}
}

