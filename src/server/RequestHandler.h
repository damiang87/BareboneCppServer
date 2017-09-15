#pragma once
#include <string>

namespace Ortosoft
{
	namespace Software
	{
		class TCPServer;

		//callback to request received
		typedef void(*RequestReceivedHandler)(TCPServer* server, int socketID, std::string msg);
	}
}