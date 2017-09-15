//Author: Damian Gzik
//Simple barebone TCPServer on WinSocket
//multiClient and multitask ready with thread's 
//[windows support, MSDN std foo used]

#include <iostream>
#include <conio.h>
#include "../server/TCPServer.h"
#include "../server/RequestHandler.h"
#include <thread>

void RequestHandler(Homecode::Software::TCPServer* server, int client, std::string msg);

int main(int argc, char* argv[])
{
	Homecode::Software::TCPServer myServer("127.0.0.1", 123, RequestHandler);	//loopback to the owner Ip, or better put here your IpV4 addres,port
	
	if (myServer.Init())
	{
		myServer.Run();
		std::cout << "Server started >" << std::endl;
	}
	std::cout << "\nPress eny key to stop server " << std::endl;

	while (!_kbhit()) {}
	std::cout << "\nServer stopped > " << std::endl;
	system("PAUSE");
	return 0;
}

void RequestHandler(Homecode::Software::TCPServer* server, int client, std::string querry)
{
	//[TCPServer* server] object can perform some actions only here, not in new thred
	//f.e. send pending status to the client: server->Send(client, "querry is being processing" );
	//!!! if [TCPServer* server] object used in ne thread - can cause undefined behavior

	std::thread sendT([=]() {

		//send querry to database 
		//wait for response
		//valid response ? send it back to the client : send zero string

		//only for the tests
		//i.e. if querry == "GET /index.html HTTP/1.1 {load index.html as std::string, send it back}"
		if (querry == "Admin,123123")	//test querry
		{
			std::string response = "everest";
			int status = send(client, response.c_str(), response.size() + 1, 0);
			if (status != SOCKET_ERROR)
				return;
		}
		else
		{
			std::string response = "0";
			int status = send(client, response.c_str(), response.size() + 1, 0);
			if (status != SOCKET_ERROR)
				return;
		}
	});
	sendT.detach();
}




