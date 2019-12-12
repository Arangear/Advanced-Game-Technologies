#pragma once
#include "NetworkedGame.h"

class CourseworkPacketReceiver : public PacketReceiver
{
public:
	CourseworkPacketReceiver(string name)
	{
		this->name = name;
	}

	void ReceivePacket(int type, GamePacket* payload, int source)
	{
		if (type == String_Message)
		{
			StringPacket* realPacket = (StringPacket*)payload;

			string msg = realPacket->GetStringFromData();

			std::cout << name << " received message: " << msg << std::endl;
		}
	}
protected:
	string name;
};