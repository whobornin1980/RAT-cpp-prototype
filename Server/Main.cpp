#include "../Shared/NetworkUtilities.h"
#include "../Shared/Utils.h"
#include "Client.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <thread>
using namespace std;

#define DEBUG
#define ADDR "127.0.0.1"
#define PORT 443
#define BUF_SIZE 512

void clientThread(SOCKET conn);

map<string, Client> clients;

int main(int argc, char** argv) {
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0) {
		MessageBoxA(NULL, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}

	SOCKADDR_IN addr;
	int addr_len = sizeof(addr);
	inet_pton(AF_INET, ADDR, &addr.sin_addr);
	addr.sin_port = htons(PORT);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	int bind_result = ::bind(sListen, (SOCKADDR*)&addr, addr_len);

	if (bind_result == SOCKET_ERROR) {
		MessageBoxA(NULL, "Bind error", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	listen(sListen, SOMAXCONN);

	cout << "Server started on " << ADDR << ":" << PORT << endl << endl;

	while (true) {
		SOCKET conn = INVALID_SOCKET;
		conn = accept(sListen, (SOCKADDR*)&addr, &addr_len);

		if (conn == INVALID_SOCKET) {
			cout << "Error accepting socket" << endl;
			continue;
		}
		
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientThread, (LPVOID)conn, NULL, NULL);
	}

	closesocket(sListen);
	WSACleanup();

	cout << endl << "\tEND" << endl;
	system("pause");

	return 0;
}

void clientThread(SOCKET conn)
{
	Client client;
	client.conn = conn;

	initCapture(&client);

	cout << "Client connected!" << endl;

	while (true) {
		Sleep(50);

		while (!client.packets.empty()) {
			SocketData packet = client.packets.back();
			client.packets.pop();

			switch (packet.tag) {
				case SocketTag::SET_ID: {
					client.id = packet.data;
					break;
				}
			}
		}
	}

	
}