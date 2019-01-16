#pragma once
#include "ubx_msg.h"
namespace ubloxns {
	enum class AckId :uint8_t {
		NAK = 0x0,
		ACK = 0x1
	};

#pragma pack(push)
#pragma pack(1)
	class Ack :public Msg {
	public:
		MsgClass clsID;
		uint8_t msgID;
		Ack(AckId id = AckId::ACK) :Msg(MsgClass::ACK, static_cast<uint8_t>(id)) {}
	};
	static_assert(sizeof(Ack) - sizeof(Msg) == 2, "size not correct");

#pragma pack(pop)
}
