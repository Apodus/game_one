
#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <climits>

#include <sys/types.h>

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <errno.h>

namespace Network
{

class SocketHandler;

class TCP_Socket
{
	friend class SocketHandler;

	public:
		TCP_Socket();
		TCP_Socket(const std::string& ip, int port);
		
		int conn_init(const std::string& ip, int port);
		void closeConnection();
		
		int write(const std::string& msg);
		
		int readyToRead();
		std::string read();
		
		int init_listener(int port);
		void accept_connection(TCP_Socket&);
		
		int setnonblocking();
		
	private:
		int sock;
		bool alive;
		struct sockaddr_in cliAddr;
		struct sockaddr_in servAddr;
};


class UDP_Socket
{
	public:
		UDP_Socket();
		UDP_Socket(const std::string& ip, int port);
		
		int conn_init(const std::string& ip, int port);
		void closeConnection();
		
		int write(const std::string& msg);
		int readyToRead();
		std::string read();
		
		int init_listener(int port);
		void accept_connection(UDP_Socket&);
		
	private:
		int sock;
		
		struct sockaddr_in cliAddr;
		struct sockaddr_in servAddr;
};



}

