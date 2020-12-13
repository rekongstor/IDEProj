//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2016 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>


#include "BoostClient.h"
#include "message.hpp"
#include "ConsoleGui.h"

using boost::asio::ip::tcp;

typedef std::deque<message> message_queue;


int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: chat_client <host> <port>\n";
			return 1;
		}

		boost::asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(argv[1], argv[2]);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		BoostClient client(io_service, iterator);
		ConsoleGui gui(&client);
		
		boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

		char line[message::max_body_length + 1];
		gui.draw();
		while (std::cin.getline(line, message::max_body_length + 1))
		{
			// we want to check our input if we are't waiting for server message
			if (!client.receive)
			{
				using namespace std; // For strlen and memcpy.
				message msg;
				msg.body_length(strlen(line));
				memcpy(msg.body(), line, msg.body_length());
				msg.encode_header();

				if (client.gameState == egs::preparation)
				{
					if ((strlen(line) == 9 && line[0] == 's' && line[2] >= '0' &&
						line[2] <= '9' && line[4] >= '0' && line[4] <= '9' &&
						line[6] >= '1' && line[6] <= '4') && (line[8] == 'v' || line[8] == 'h'))
					{
						if (!client.m_my.set_ship(line[2] - '0', line[4] - '0', line[6] - '0', line[8]))
						{
							gui.draw();
							cout << "\x1B[31mInvalid placement!\x1B[0m" << endl;
							continue;
						}
						// we want to check here our placement
						client.write(msg);
						client.receive = true;
						continue;
					} // s x y w d command

					if (strlen(line) == 1 && line[0] == 'r')
					{
						if (client.m_my.ships_ready())
						{
							client.ready = true;
							client.write(msg);
							client.receive = true;
							continue;
						}
						else
						{
							gui.draw();
							cout << "\x1B[31mShips not ready!\x1B[0m" << endl;
							continue;
						}
					}
					gui.draw();
					cout << "\x1B[31mInvalid input!\x1B[0m" << endl;
					// otherwise we don't send anything
				}

				// turns
				if (client.gameState == egs::my_turn)
				{
					if (strlen(line) == 3 && line[0] >= '0' && line[0] <= '9' &&
						line[2] >= '0' && line[2] <= '9')
					{
						client.write(msg);
						client.receive = true;
						continue;
					}
					// otherwise we don't send anything
				}

				if (client.gameState == egs::end)
				{
					client.write(msg);
					continue;
				}
			}
		}

		client.close();
		t.join();
	}
	catch (std::exception & e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}