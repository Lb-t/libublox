#pragma once
#include "ubx_msg.h"
#include <stddef.h>
namespace ubloxns
{
	const int SV_NUM_MAX = 30;

	enum class NavId
	{
		AOPSTATUS = 0x60,
		ATT = 0x05,
		CLOCK = 0x22,
		DGPS = 0x31,
		DOP = 0x04,
		EOE = 0x61,
		GEOFENCE = 0x39,
		HPPOSECEF = 0x13,
		HPPOSLLH = 0x14,
		ODO = 0x09,
		ORB = 0x34,
		POSECEF = 0x01,
		POSLLH = 0x02,
		PVT = 0x07,
		RELPOSNED = 0x3C,
		RESETODO = 0x10,
		SAT = 0x35,
		SBAS = 0x32,
		SOL = 0x06,
		STATUS = 0x03,
		SVINFO = 0x30,
		SVIN = 0x3B,
		TIMEBDS = 0x24,
		TIMEGAL = 0x25,
		TIMEGLO = 0x23,
		TIMEGPS = 0x20,
		TIMELS = 0x26,
		TIMEUTC = 0x21,
		VELECEF = 0x11,
		VELNED = 0x12,
	};
#pragma pack(push,1)
	class Nav : public Msg
	{
	public:
		Nav(NavId id) : Msg(MsgClass::NAV, static_cast<uint8_t>(id)) {}
	};

	class NavAopstatus :public Nav {
	public:
		uint32_t iTOW;
		uint8_t aopCfg;
		uint8_t status;
		uint8_t reserved1[10];
		NavAopstatus() :Nav(NavId::AOPSTATUS) {}
	};

	static_assert(sizeof(NavAopstatus) - sizeof(Msg) == 16,
		"size not correct");
	class NavAtt :public Nav {
	public:
		uint32_t iTOW;
		uint8_t version;
		uint8_t reserved1[3];
		int32_t roll;
		int32_t pitch;
		int32_t heading;
		uint32_t accRoll;
		uint32_t accPitch;
		uint32_t accHeading;
		NavAtt() :Nav(NavId::ATT) {}
	};
	static_assert(sizeof(NavAtt) - sizeof(Msg) == 32,
		"size not correct");

	class NavClock :public Nav {
	public:
		uint32_t iTOW;
		int32_t clkB;
		int32_t clkD;
		uint32_t tAcc;
		uint32_t fAcc;
		NavClock() :Nav(NavId::CLOCK) {}
	};
	static_assert(sizeof(NavClock) - sizeof(Msg) == 20,
		"size not correct");

	class NavDgpsRpt {
	public:
		uint8_t svid;
		uint8_t flags;
		uint16_t ageC;
		float prc;
		float prrc;
	};
	static_assert(sizeof(NavDgpsRpt) == 12, "size not correct");

	class NavDgps :public Nav {
	public:
		uint32_t iTOW;
		int32_t age;
		int16_t baseId;
		int16_t baseHealth;
		uint8_t numCh;
		uint8_t status;
		uint8_t reserved1[2];
		NavDgpsRpt rpt[SV_NUM_MAX];
	};
	static_assert(offsetof(NavDgps, rpt) - sizeof(Msg) == 16,
		"size not correct");

	class NavDop :public Nav {
	public:
		uint32_t iTOW;
		uint16_t gDOP;
		uint16_t pDOP;
		uint16_t tDOP;
		uint16_t vDOP;
		uint16_t hDOP;
		uint16_t nDOP;
		uint16_t eDOP;
	};
	static_assert(sizeof(NavDop) - sizeof(Msg) == 18,
		"size not correct");

	class NavEoe :public Nav {

		uint32_t iTOW;
	};
	static_assert(sizeof(NavEoe) - sizeof(Msg) == 4,
		"size not correct");

	class NavGeofenceRpt {
		uint8_t status;
		uint8_t reserved1[1];
	};
	static_assert(sizeof(NavGeofenceRpt) == 2, "size not correct");

	class NavGeofence :public Nav {

		uint32_t iTOW;
		uint8_t version;
		uint8_t status;
		uint8_t numFences;
		uint8_t combState;
	};
	static_assert(sizeof(NavGeofence) - sizeof(Msg) == 8,
		"size not correct");

	class NavHpposecef :public Nav {

		uint8_t version;
		uint8_t reserved1[3];
		uint32_t iTOW;
		int32_t ecefX;
		int32_t ecefY;
		int32_t ecefZ;
		uint8_t ecefXHp;
		uint8_t ecefYHp;
		uint8_t ecefZHp;
		uint8_t reserved2;
		uint32_t pAcc;
	};
	static_assert(sizeof(NavHpposecef) - sizeof(Msg) == 28,
		"size not correct");

	class NavHpposllh :public Nav {

		uint8_t version;
		uint8_t reserved1[3];
		uint32_t iTOW;
		int32_t lon;
		int32_t lat;
		int32_t height;
		int32_t hMSL;
		int8_t lonHp;
		int8_t latHp;
		int8_t heightHp;
		int8_t hMSLHp;
		uint32_t hAcc;
		uint32_t vAcc;
	};
	static_assert(sizeof(NavHpposllh) - sizeof(Msg) == 36,
		"size not correct");

	class NavOdo :public Nav {

		uint8_t version;
		uint8_t reserved1[3];
		uint32_t iTOW;
		uint32_t distance;
		uint32_t totalDistance;
		uint32_t distanceStd;
	};
	static_assert(sizeof(NavOdo) - sizeof(Msg) == 20,
		"size not correct");

	class NavOrbRpt {
		uint8_t gnssId;
		uint8_t svId;
		uint8_t svFlag;
		union {
			uint8_t eph;
			struct {
				uint8_t ephUsability : 5;
				uint8_t ephSource : 3;
			};
		};
		union {
			uint8_t alm;
			struct {
				uint8_t almUsability : 5;
				uint8_t almSource : 3;
			};
		};
		uint8_t otherOrb;
	};
	static_assert(sizeof(NavOrbRpt) == 6, "size not correct");

	class NavOrb :public Nav {
	public:
		uint32_t iTOW;
		uint8_t version;
		uint8_t numSv;
		uint8_t reserved1[2];
		NavOrbRpt rpt[SV_NUM_MAX];
	};
	static_assert(offsetof(NavOrb, rpt) - sizeof(Msg) == 8,
		"size not correct");

	class NavPosecef :public Nav {
		uint32_t iTOW;
		int32_t ecefX;
		int32_t ecefY;
		int32_t ecefZ;
		uint32_t pAcc;
	};
	static_assert(sizeof(NavPosecef) - sizeof(Msg) == 20,
		"size not correct");

	class NavPosllh :public Nav {

		uint32_t iTOW;
		int32_t lon;
		int32_t lat;
		int32_t height;
		int32_t hMSL;
		uint32_t hAcc;
		uint32_t vAcc;
	};
	static_assert(sizeof(NavPosllh) - sizeof(Msg) == 28,
		"size not correct");

	class NavPvt :public Nav {

		uint32_t iTOW;
		uint16_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t min;
		uint8_t sec;
		union {
			uint8_t valid;
			struct {
				uint8_t validDate : 1;
				uint8_t validTime : 1;
				uint8_t fullyResolved : 1;
				uint8_t validMag : 1;
			};
		};
		uint32_t tAcc;
		int32_t nano;
		uint8_t fixType;
		union {
			uint8_t flags;
			struct {
				uint8_t gnssFixOK : 1;
				uint8_t diffSoln : 1;
				uint8_t psmState : 3;
				uint8_t headVehValid : 1;
				uint8_t carrSoln : 2;
			};
		};
		union {
			uint8_t flags2;
			struct {
				uint8_t : 5;
						  uint8_t confirmedAvai : 1;
						  uint8_t confirmedDate : 1;
						  uint8_t confirmedTime : 1;
			};
		};
		uint8_t numSv;
		int32_t lon;
		int32_t lat;
		int32_t height;
		int32_t hMSL;
		uint32_t hAcc;
		uint32_t vAcc;
		int32_t velN;
		int32_t velE;
		int32_t velD;
		int32_t gSpeed;
		int32_t headMot;
		uint32_t sAcc;
		uint32_t headAcc;
		uint16_t pDOP;
		uint8_t reserved1[6];
		int32_t headVeh;
		int16_t magDec;
		uint16_t magAcc;
	};
	static_assert(sizeof(NavPvt) - sizeof(Msg) == 92,
		"size not correct");

	class NavRelposned :public Nav {
		uint8_t version;
		uint8_t reserved1;
		uint16_t refStationId;
		uint32_t iTOW;
		int32_t relPosN;
		int32_t relPosE;
		int32_t relPosD;
		int8_t relPosHPN;
		int8_t relPosHPE;
		int8_t relPosHPD;
		uint8_t reserved2;
		uint32_t accN;
		uint32_t accE;
		uint32_t accD;
		uint32_t flags;
	};
	static_assert(sizeof(NavRelposned) - sizeof(Msg) == 40,
		"size not correct");

	class NavResetodo :public Nav {

	};
	static_assert(sizeof(NavResetodo) - sizeof(Msg) == 0,
		"size not correct");

	class NavSatRpt {
		uint8_t gnssId;
		uint8_t svId;
		uint8_t cno;
		int8_t elev;
		int16_t azim;
		int16_t prRes;
		uint32_t flags;
	};
	static_assert(sizeof(NavSatRpt) == 12, "size not correct");

	class NavSat :public Nav {
	public:
		uint32_t iTOW;
		uint8_t version;
		uint8_t numSvs;
		uint8_t reserved1[2];
		NavSatRpt rpt[SV_NUM_MAX];
	};
	static_assert(offsetof(NavSat, rpt) - sizeof(Msg) == 8,
		"size not correct");

	class NavSbasRpt {
		uint8_t svid;
		uint8_t flags;
		uint8_t udre;
		uint8_t svSys;
		uint8_t svService;
		uint8_t reserved2;
		int16_t prc;
		uint8_t reserved3[2];
		int16_t ic;
	};

	static_assert(sizeof(NavSbasRpt) == 12, "size not correct");
	class NavSbas :public Nav {
	public:
		uint32_t iTOW;
		uint8_t geo;
		uint8_t mode;
		int8_t sys;
		uint8_t service;
		uint8_t cnt;
		uint8_t reserved1[3];
		NavSbasRpt rpt[SV_NUM_MAX];
	};
	static_assert(offsetof(NavSbas, rpt) - sizeof(Msg) == 12,
		"size not correct");

	class NavSol :public Nav {
		uint32_t iTOW;
		int32_t fTOW;
		int16_t week;
		uint8_t gpsFix;
		uint8_t flags;
		int32_t ecefX;
		int32_t ecefY;
		int32_t ecefZ;
		uint32_t pAcc;
		int32_t ecefVX;
		int32_t ecefVY;
		int32_t ecefVZ;
		uint32_t sAcc;
		uint16_t pDOP;
		uint8_t reserved1;
		uint8_t numSV;
		uint8_t reserved2[4];
	};
	static_assert(sizeof(NavSol) - sizeof(Msg) == 52,
		"size not correct");

	class NavStatus :public Nav {
		uint32_t iTOW;
		uint8_t gpsFix;
		uint8_t flags;
		uint8_t fixStat;
		uint8_t flags2;
		uint32_t ttff;
		uint32_t msss;
	};
	static_assert(sizeof(NavStatus) - sizeof(Msg) == 16,
		"size not correct");

	class NavSvinfoRpt {
		uint8_t chn;
		uint8_t svid;
		uint8_t flags;
		uint8_t quality;
		uint8_t cno;
		int8_t elev;
		int16_t azim;
		int32_t prRes;
	};
	static_assert(sizeof(NavSvinfoRpt) == 12, "size not correct");

	class NavSvinfo :public Nav {
	public:
		uint32_t iTOW;
		uint8_t numCh;
		uint8_t globalFlags;
		uint8_t reserved1[2];
		NavSvinfoRpt rpt[SV_NUM_MAX];
		NavSvinfo() :Nav(NavId::SVINFO) {}
	};
	static_assert(offsetof(NavSvinfo, rpt) - sizeof(Msg) == 8,
		"size not correct");

	class NavSvin :public Nav {

		uint8_t version;
		uint8_t reserved1[3];
		uint32_t iTOW;
		uint32_t dur;
		int32_t meanX;
		int32_t meanY;
		int32_t meanZ;
		int8_t meanXHP;
		int8_t meanYHP;
		int8_t meanZHP;
		uint8_t reserved2;
		uint32_t meanAcc;
		uint32_t obs;
		uint8_t valid;
		uint8_t active;
		uint8_t reserved3[2];
	};
	static_assert(sizeof(NavSvin) - sizeof(Msg) == 40,
		"size not correct");

	class NavTimebds :public Nav {

		uint32_t iTOW;
		uint32_t SOW;
		int32_t fSOW;
		int16_t week;
		int8_t leapS;
		uint8_t valid;
		uint32_t tAcc;
	};
	static_assert(sizeof(NavTimebds) - sizeof(Msg) == 20,
		"size not correct");

	class NavTimegal :public Nav {

		uint32_t iTOW;
		uint32_t galTow;
		int32_t fGalTow;
		int16_t galWno;
		int8_t leapS;
		uint8_t valid;
		uint32_t tAcc;
	};
	static_assert(sizeof(NavTimegal) - sizeof(Msg) == 20,
		"size not correct");

	class NavTimeglo :public Nav {

		uint32_t iTOW;
		uint32_t TOD;
		int32_t fTOD;
		uint16_t Nt;
		uint8_t N4;
		uint8_t valid;
		uint32_t tAcc;
	};
	static_assert(sizeof(NavTimeglo) - sizeof(Msg) == 20,
		"size not correct");

	class NavTimegps :public Nav {
		uint32_t iTOW;
		int32_t fTOW;
		int16_t week;
		int8_t leapS;
		uint8_t valid;
		uint32_t tAcc;
		NavTimegps() :Nav(NavId::TIMEGPS) {}
	};
	static_assert(sizeof(NavTimegps) - sizeof(Msg) == 16,
		"size not correct");

	class NavTimels :public Nav {

		uint32_t iTOW;
		uint8_t version;
		uint8_t reserved1[3];
		uint8_t srcOfCurrLs;
		uint8_t currLs;
		uint8_t srcOfLsChange;
		int8_t lsChange;
		int32_t timeToLsEvent;
		uint16_t dateOfLsGpsWn;
		uint16_t dateOfLsGpsDn;
		uint8_t reserved2[3];
		uint8_t valid;
	};
	static_assert(sizeof(NavTimels) - sizeof(Msg) == 24,
		"size not correct");

	class NavTimeutc :public Nav {

		uint32_t iTOW;
		uint32_t tAcc;
		int32_t nano;
		uint16_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t min;
		uint8_t sec;
		uint8_t valid;
	};
	static_assert(sizeof(NavTimeutc) - sizeof(Msg) == 20,
		"size not correct");

	class NavVelecef :public Nav {
		uint32_t iTOW;
		int32_t ecefVX;
		int32_t ecefVY;
		int32_t ecefVZ;
		uint32_t sAcc;
	};
	static_assert(sizeof(NavVelecef) - sizeof(Msg) == 20,
		"size not correct");

	class NavVelned :public Nav {

		uint32_t iTOW;
		int32_t velN;
		int32_t velE;
		int32_t velD;
		uint32_t speed;
		uint32_t gSpeed;
		int32_t heading;
		uint32_t sAcc;
		uint32_t cAcc;
	};
	static_assert(sizeof(NavVelned) - sizeof(Msg) == 36,
		"size not correct");

#pragma pack(pop)              
} // namespace ublox


