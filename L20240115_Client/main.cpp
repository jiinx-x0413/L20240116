#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include "Packet.h"

#pragma comment(lib, "ws2_32")

#define PORT 5001
#define PACKET_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

int main()
{
	// wsa init
	WSAData wsaData;
	//int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// server socket ����
	SOCKET ServerSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	// server socket �ʱ�ȭ
	struct sockaddr_in ServerSocketAddr; // SOCKADDR_IN ServerSocketAddr; // �����쿡���� �Ǵ� �Ŵ�!
	memset(&ServerSocketAddr, 0, sizeof(ServerSocketAddr));
	ServerSocketAddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_IP, &(ServerSocketAddr.sin_addr.s_addr)); // server IP �ּҸ� socket ����ü�� �ʱ�ȭ
	ServerSocketAddr.sin_port = htons(PORT);

	// connect
	//Result = connect(ServerSocket, (SOCKADDR*)&ServerSocketAddr, sizeof(ServerSocketAddr));
	connect(ServerSocket, (struct sockaddr*)&ServerSocketAddr, sizeof(ServerSocketAddr));

	// ���
	/*while (true)
	{
		char message = 'C';
		recv(ServerSocket, &message, 1, 0);

		char sendMessage = 'C';
		send(ServerSocket, &sendMessage, 1, 0);
	}*/

	// ��� ; server.exe
	/*const char Message[] = "give me message.";
	send(ServerSocket, Message, (int)strlen(Message), 0);

	char Buffer[1024] = { 0, };
	recv(ServerSocket, Buffer, sizeof(Buffer), 0);

	std::cout << Buffer << std::endl;*/


	// ��� ; 
	while (true)
	{
		// Packet : Header
		//		recv for Packet Header
		char Buffer[1024] = { 0, };
		int RecvByte = recv(ServerSocket, Buffer, 4, MSG_WAITALL);
		if (RecvByte <= 0)
		{
			break;
		}
		//		Header
		Header PacketHeader;
		memcpy(&PacketHeader, Buffer, sizeof(Header));

		// Packet : Calculate
		if (PacketHeader.PacketType == (unsigned short)(EPacketType::Calculate)) // ������ ó��
		{
			// �ڷ� �ޱ�
			char* DataBuffer = new char[PacketHeader.Length];
			int RecvByte = recv(ServerSocket, DataBuffer, 4, MSG_WAITALL);
			if (RecvByte <= 0)
			{
				break;
			}

			//		4byte�� �߶� �޸𸮿� �ű��
			Data Packet;
			memcpy(&Packet, DataBuffer, sizeof(Packet));
			long long Result = 0; // operator = * �� ���, int * int = 2^64 �̹Ƿ� ������� int ���� ���� �� ����. (overflow)

			cout << Packet.FirstNumber << " ";

			//		����
			switch (Packet.Operator)
			{
			case 0:
				Result = Packet.FirstNumber + Packet.SecondNumber;
				cout << " + ";
				break;
			case 1:
				Result = Packet.FirstNumber - Packet.SecondNumber;
				cout << " - ";
				break;
			case 2:
				Result = Packet.FirstNumber * Packet.SecondNumber;
				cout << " * ";
				break;
			case 3:
				Result = Packet.FirstNumber / Packet.SecondNumber;
				cout << " / ";
				break;
			case 4:
				Result = Packet.FirstNumber % Packet.SecondNumber;
				cout << " % ";
				break;
			default: // ���Ѱ� ���� ���� �ʰ� ���ϱ�� ��
				Result = Packet.FirstNumber + Packet.SecondNumber;
				cout << " + ";
				break;
			}

			cout << Packet.SecondNumber << " = ";
			cout << Result << endl;

			//		send
			char Message[8] = { 0, };
			memcpy(Message, &Result, sizeof(Result));
			send(ServerSocket, Message, (u_int)sizeof(Message), 0);
			delete[] DataBuffer;

		}
		// Packet : Image
		else if (PacketHeader.PacketType == (unsigned short)(EPacketType::Image)) // �̹��� ó��
		{
			// file 
			FILE* OutputFile = fopen("88��� �̰���.jpg", "wb"); // write binary
			char* FileBuffer = new char[PacketHeader.Length];
			int RecvByte = recv(ServerSocket, FileBuffer, PacketHeader.Length, MSG_WAITALL); // recieve buffer from ServerSocket
			if (RecvByte <= 0)
			{
				break;
			}
			int WriteSize = fwrite(FileBuffer, sizeof(char), RecvByte, OutputFile); // OutputFile �� �� ĭ¥���� Buffer ũ�⸸ŭ ���ּ���
			
			fclose(OutputFile);
			delete[] FileBuffer;
		}



		
	}


	closesocket(ServerSocket);

	WSACleanup();
	return 0;
}