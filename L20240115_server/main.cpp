#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <iostream>
#include <WS2tcpip.h>
#include "Packet.h"

#pragma comment(lib, "ws2_32")

#define PORT 5001
#define PACKET_SIZE 1024
#define SERVER_IP "127.0.0.1"

using namespace std;


int main()
{
	srand((u_int)(time(nullptr)));

	// wsa struct
	WSAData wsaData;
	//int Result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// listen socket ����
	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// listen socket struct ; ���� �ʱ�ȭ
	struct sockaddr_in ListenSocketAddr;
	// memset
	ListenSocketAddr.sin_family = AF_INET;
	ListenSocketAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &(ListenSocketAddr.sin_addr.s_addr)); // server IP �ּҸ� socket ����ü�� �ʱ�ȭ
	//ListenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind : listen ���Ͽ� �ּ� ���� ����
	//Result = bind(ListenSocket, (SOCKADDR*)&ListenSocketAddr, sizeof(ListenSocketAddr));
	bind(ListenSocket, (struct sockaddr*)&ListenSocketAddr, sizeof(ListenSocketAddr));

	// listen : ��ٸ� (listen ������ ���� ���� = ���� ��� ����)
	//Result = listen(ListenSocket, 5);
	listen(ListenSocket, 5);

	// client socket ����
	//SOCKADDR_IN ClientSocketAddr;
	struct sockaddr_in ClientSocketAddr;
	// memset
	int ClientSocketAddrLength = sizeof(ClientSocketAddr);

	// accept : listen socket���� ���� Ŭ���̾�Ʈ�� ���� ���ӿ�û ���� -> return SOCKET
	SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientSocketAddr, &ClientSocketAddrLength);
	
	// ��� ; client.exe
	/*char Message[] = "show me the money.";
	send(ClientSocket, Message, (int)strlen(Message), 0);

	char Buffer[1024] = {0, };
	recv(ClientSocket, Buffer, 1024, 0);

	std::cout << Buffer << std::endl;*/


	// ��� ; 
	while(true)
	{ 
		//	Packet : Operator
		//		Header
		Header DataHeader;
		DataHeader.Length = sizeof(Data);
		DataHeader.PacketType = (unsigned short)(EPacketType::Calculate);
		send(ClientSocket, (char*)&DataHeader, (u_int)sizeof(DataHeader), 0);

		Data Packet;
		Packet.FirstNumber = (rand() % 20000) - 10000; // 0 ~ 9999
		Packet.SecondNumber = (rand() % 20000) - 10000; // 1 ~ 9999
		Packet.SecondNumber == 0 ? 1 : Packet.SecondNumber; // 0���� ���� �� ���� ����
		Packet.Operator = rand() % 5;

		//		send
		send(ClientSocket, (char*)&Packet, (u_int)sizeof(Packet), 0);

		//		recv
		char Buffer[1024] = { 0, };
		int RecvByte = recv(ClientSocket, Buffer, 1024, 0);
		if (RecvByte <= 0)
		{
			break;
		}

		long long Result = 0;
		memcpy(&Result, Buffer, sizeof(Result));
		cout << Result << endl;


		//	Packet : file send
		FILE* InputFile = fopen("nathan.jpg", "rb"); // read binary
		fseek(InputFile, 0, SEEK_END); // Ŀ�� ��ġ�� �� �ڷ� �ű��
		unsigned short FileSize = (unsigned short)ftell(InputFile); // ���� Ŀ�� ��ġ ��������
		cout << FileSize << endl;
		//		Header 
		//		header �� �缳���ؼ� ������, Ŭ�󿡼� PacketHeader.PakcetType�� �о� ������ ó���Ѵ�.
		DataHeader.Length = FileSize;
		DataHeader.PacketType = (unsigned short)(EPacketType::Image);
		send(ClientSocket, (char*)&DataHeader, (u_int)sizeof(DataHeader), 0);

		char* FileBuffer = new char[FileSize];
		fseek(InputFile, 0, SEEK_SET); // ���� ũ�⸸ �˾� ���� Ŀ�� ��ġ �ٽ� ������� ������
		size_t ReadSize = fread(Buffer, sizeof(char), (u_int)FileSize, InputFile); // InputFile�� �� ĭ¥��(����������)�� Buffer ũ�⸸ŭ �о��ּ���
		size_t SendSize = send(ClientSocket, Buffer, ReadSize, 0); //send buffer to ClientSocket

		fclose(InputFile);
		delete[] FileBuffer;
	}

	closesocket(ListenSocket);
	closesocket(ClientSocket);

	WSACleanup();

	return 0;
}