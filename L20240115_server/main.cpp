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

	// listen socket 생성
	SOCKET ListenSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	// listen socket struct ; 소켓 초기화
	struct sockaddr_in ListenSocketAddr;
	// memset
	ListenSocketAddr.sin_family = AF_INET;
	ListenSocketAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &(ListenSocketAddr.sin_addr.s_addr)); // server IP 주소를 socket 구조체에 초기화
	//ListenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	// bind : listen 소켓에 주소 정보 연결
	//Result = bind(ListenSocket, (SOCKADDR*)&ListenSocketAddr, sizeof(ListenSocketAddr));
	bind(ListenSocket, (struct sockaddr*)&ListenSocketAddr, sizeof(ListenSocketAddr));

	// listen : 기다림 (listen 소켓의 상태 변경 = 접속 대기 상태)
	//Result = listen(ListenSocket, 5);
	listen(ListenSocket, 5);

	// client socket 생성
	//SOCKADDR_IN ClientSocketAddr;
	struct sockaddr_in ClientSocketAddr;
	// memset
	int ClientSocketAddrLength = sizeof(ClientSocketAddr);

	// accept : listen socket으로 들어온 클라이언트에 대해 접속요청 수락 -> return SOCKET
	SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientSocketAddr, &ClientSocketAddrLength);
	
	// 통신 ; client.exe
	/*char Message[] = "show me the money.";
	send(ClientSocket, Message, (int)strlen(Message), 0);

	char Buffer[1024] = {0, };
	recv(ClientSocket, Buffer, 1024, 0);

	std::cout << Buffer << std::endl;*/


	// 통신 ; 
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
		Packet.SecondNumber == 0 ? 1 : Packet.SecondNumber; // 0으로 나눌 때 에러 방지
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
		fseek(InputFile, 0, SEEK_END); // 커서 위치를 맨 뒤로 옮기기
		unsigned short FileSize = (unsigned short)ftell(InputFile); // 현재 커서 위치 가져오기
		cout << FileSize << endl;
		//		Header 
		//		header 를 재설정해서 보내야, 클라에서 PacketHeader.PakcetType을 읽어 로직을 처리한다.
		DataHeader.Length = FileSize;
		DataHeader.PacketType = (unsigned short)(EPacketType::Image);
		send(ClientSocket, (char*)&DataHeader, (u_int)sizeof(DataHeader), 0);

		char* FileBuffer = new char[FileSize];
		fseek(InputFile, 0, SEEK_SET); // 파일 크기만 알아 오고 커서 위치 다시 원래대로 돌리기
		size_t ReadSize = fread(Buffer, sizeof(char), (u_int)FileSize, InputFile); // InputFile을 한 칸짜리(단위사이즈)로 Buffer 크기만큼 읽어주세요
		size_t SendSize = send(ClientSocket, Buffer, ReadSize, 0); //send buffer to ClientSocket

		fclose(InputFile);
		delete[] FileBuffer;
	}

	closesocket(ListenSocket);
	closesocket(ClientSocket);

	WSACleanup();

	return 0;
}