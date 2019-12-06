#include "server.h"

#include <iostream>
#include <list>

typedef boost::shared_ptr<server> server_ptr;
typedef std::list<server_ptr> server_list;

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: chat_server <port> [<port> ...]\n";
			return 1;
		}

		boost::asio::io_service io_service;

		server_list servers;
		for (int i = 1; i < argc; ++i)
		{
			using namespace std; // For atoi.
			tcp::endpoint endpoint(tcp::v4(), atoi(argv[i]));
			server_ptr server(new server(io_service, endpoint));
			servers.push_back(server);
		}

		io_service.run();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}