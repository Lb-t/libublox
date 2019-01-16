#pragma once
#include <stdint.h>
namespace ubloxns
{
	enum class MsgClass : uint8_t
	{
		NAV = 1,
		RXM,
		INF = 4,
		ACK,
		CFG,
		UPD = 9,
		MON,
		AID,
		TIM = 0xd,
		ESF = 0x10,
		MGA = 0x13,
		LOG = 0x21,
		SEC = 0x27,
		HNR
	};
	struct CheckSum
	{
		uint8_t a;
		uint8_t b;
		CheckSum() : a(0), b(0) {}
		bool operator==(CheckSum &s) {
			return (a == s.a) && (b = s.b);
		}
	};
	class Msg
	{
	public:
		MsgClass CLASS;
		uint8_t ID;
		uint16_t LENGTH;
		Msg(MsgClass cls, uint8_t id) : CLASS(cls), ID(id), LENGTH(0) {}
		CheckSum check_sum()
		{
			CheckSum ck;
			ck.a += static_cast<uint8_t>(CLASS);
			ck.b += ck.a;
			ck.a += ID;
			ck.b += ck.a;
			ck.a += LENGTH & 0xff;
			ck.b += ck.a;
			ck.a += (LENGTH >> 8) & 0xff;
			ck.b += ck.a;
			uint8_t *payload = (uint8_t*)(this + 1);
			for (int i = 0; i < LENGTH; ++i)
			{
				ck.a += payload[i];
				ck.b += ck.a;
			}
			return ck;
		}
	};
} // namespace ublox