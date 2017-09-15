#include "TCPServer.h"

namespace Ortosoft
{
	namespace Software
	{
		TCPServer::TCPServer(std::string ipAddress, int port, RequestReceivedHandler handler)
			:m_ipAddress(ipAddress), m_port(port), m_handler(handler)
		{
			m_mainListeningThreadRunningCondition = false;
		}

		TCPServer::~TCPServer()
		{
			CleanUp();
			m_mainListeningThread.join();
		}

		bool TCPServer::Init()
		{
			WSADATA wsData;
			WORD ver = MAKEWORD(2, 2);

			int wsInit = WSAStartup(ver, &wsData);
			
			if (wsInit != 0)
			{
				std::cerr << "Can't start winsocket, err: " << wsInit << std::endl;
				return wsInit == 0;
			}

			return wsInit == 0;
		}

		bool TCPServer::Send(int clientSocket, std::string msg)
		{
			int status = send(clientSocket, msg.c_str(), msg.size() + 1, 0);
			if (status != SOCKET_ERROR)
				return true;
			else
				return false;
		}

		void TCPServer::Run()
		{
			m_mainListeningThreadRunningCondition = true;
			m_mainListeningThread = std::thread([this]() { this->MainListeningThreadRun(); });
		}

		void TCPServer::CleanUp()
		{
			m_mainListeningThreadRunningCondition = false;
			WSACleanup();
		}

		SOCKET TCPServer::CreateSocket()
		{
			SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);		//create a winsocket
			
			if (listening != INVALID_SOCKET)
			{
				//if socket not invalid create hint structure for socket data
				sockaddr_in hint;
				hint.sin_family = AF_INET;
				hint.sin_port = htons(m_port);
				inet_pton(AF_INET, m_ipAddress.c_str(), &hint.sin_addr);

				//bind socket with the hint structure (give socket listening parameters)
				int bindOK = bind(listening, (sockaddr*)&hint, sizeof(hint));
				if (bindOK != SOCKET_ERROR)
				{
					int listenOk = listen(listening, SOMAXCONN);	//try to listen
					if (listenOk == SOCKET_ERROR)
						return -1;
				}
				else
				{
					return -1;
				}
				return listening;
			}
			else
			{
				std::cerr << "Can't create socket, err: " << WSAGetLastError() << std::endl;
				return -1;
			}
		}

		SOCKET TCPServer::WaitForConnection(SOCKET listening)
		{
			sockaddr_in client;
			int clientSize = sizeof(client);
			SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);

			char host[NI_MAXHOST];		//client's remote name
			char service[NI_MAXSERV];	//Service (i.e port) the client is connected on

			ZeroMemory(host, NI_MAXHOST);
			ZeroMemory(service, NI_MAXSERV);

			if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
			{
				//for the test (in final version those data should be written in connections log file)
				std::cout << "Client: " << clientSocket << " " << host << " connected on port: " << service << std::endl;
			}
			else
			{
				//for the test (in final version those data should be written in connections log file)
				inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
				std::cout << host << " connected on port: " << htons(client.sin_port) << std::endl;
			}

			return clientSocket;
		}

		void TCPServer::MainListeningThreadRun()
		{
			SOCKET listening = CreateSocket();
			if (listening == INVALID_SOCKET)
			{
				m_mainListeningThreadRunningCondition = false;
				return;
			}

			// create a set(array with listening socket and all other connected sockets)
			fd_set master;
			FD_ZERO(&master);
			FD_SET(listening, &master);

			while (m_mainListeningThreadRunningCondition)
			{
				fd_set slave = master;

				int socketsCnt = select(0, &slave, nullptr, nullptr, nullptr);
				for (int i = 0; i < socketsCnt; ++i)
				{
					SOCKET _socket = slave.fd_array[i];
					if (_socket == listening)
					{
						SOCKET client = WaitForConnection(listening);
						if (client != INVALID_SOCKET)
						{
							FD_SET(client, &master);
						}
					}
					else
					{
						int bytesReceived = recv(_socket, m_buffer, MAX_BUFFER_SIZE, 0);
						if (bytesReceived <= 0)
						{
							//for the test (in final version those data should be written in connections log file)
							std::cout << "Client: " << _socket << " disconnected" << std::endl;
							closesocket(_socket);
							FD_CLR(_socket, &master);
						}
						else
						{
							m_handler(this, _socket, std::string(m_buffer, 0, bytesReceived));
						}
					}
				}
			}
		}
	}
}
