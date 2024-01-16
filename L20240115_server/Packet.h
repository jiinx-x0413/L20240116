#pragma once
#ifndef __PACKET_H__
#define __PACKET_H__

enum class EPacketType
{
	Calculate = 10,
	Image = 20,
	MAX
};

#pragma pack(push, 1)
typedef struct _Header
{
	unsigned short Length;
	unsigned short PacketType; // packet type
	//unsigned short CRC; // �����ڵ� ; ��� �� ���� ����� CRC�� ������ ������ ��ŷ�̴�!
} Header;

typedef struct _Data
{
	// u_short PacketType; // packet type
	int FirstNumber;
	int SecondNumber;
	char Operator;
} Data;
#pragma pack(pop)

#endif // __PACKET_H__