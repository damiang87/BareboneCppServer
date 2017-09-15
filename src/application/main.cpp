#include <iostream>
#include <conio.h>
#include "../server/TCPServer.h"
#include "../server/RequestHandler.h"
#include <thread>

void RequestHandler(Ortosoft::Software::TCPServer* server, int client, std::string msg);

int main(int argc, char* argv[])
{
	Ortosoft::Software::TCPServer myServer("192.168.2.105", 123, RequestHandler);
	
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

void RequestHandler(Ortosoft::Software::TCPServer* server, int client, std::string querry)
{
	std::thread sendT([=]() {

		//send querry to database 
		//wait for response
		//valid response ? send it back to the client : send zero string

		//only for the tests
		if (querry == "Admin,123123")
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




