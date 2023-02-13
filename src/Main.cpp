#include <libos/NetIO.h>
#include <stdio.h>

#define WITH_DEBUG 1
#include "Http.hpp"



//Host: localhost - ignore
//Connection: keep-alive - ignore
//Upgrade-Insecure-Requests: 1
//User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.41
//sec-ch-ua: "Chromium";v="110", "Not A(Brand";v="24", "Microsoft Edge";v="110"
//sec-ch-ua-mobile: ?0
//sec-ch-ua-platform: "Windows"
//Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*; q = 0.8, application / signed - exchange; v = b3; q = 0.7
//Sec - Fetch - Site: none
//Sec - Fetch - Mode : navigate
//Sec - Fetch - User : ? 1
//Sec - Fetch - Dest : document
//Accept - Encoding : gzip, deflate, br
//Accept - Language : en - US, en; q = 0.9

namespace WebServer
{
	using namespace libos::http_extention;
	class Server
	{
		losSocket serverSocket;
		bool running = true;
	public:
		Server()
		{
			libOSInit();
		}

		~Server()
		{
			libOSCleanUp();
		}

		int run()
		{
			losCreateSocketInfo info{};
			info.address = "localhost";
			info.address_size = 10;
			info.socket_bits = LOS_SOCKET_SERVER | LOS_SOCKET_TCP;
			info.port = 80;

			if (losCreateSocket(&serverSocket, info) != LOS_SUCCESS)
				return 1;

			while (running)
			{
				losSocket client;
				if (losWaitForClient(serverSocket, &client) != LOS_SUCCESS)
					running = false;
                HttpRequest request{};
				if(losReadHTTPSocket(client, &request) == HTTP_SUCCESS)
					request.print();
			}

			losDestroySocket(serverSocket);
			return 0;
		}
	};
}



int main()
{
	WebServer::Server server;
	return server.run();
}