#pragma once
#include "ubx_nav.h"
#include "ubx_ack.h"
#include "os.h"
#include "serialport.h"
namespace ubloxns
{

enum class InputState{
    SYNC_0,
    SYNC_1,
    HEAD,
    CONTENT,
    CHECK_0,
    CHECK_1
};

 struct WaitType {
	 uint8_t MSG : 1;
	 uint8_t ACK : 1;
	 WaitType(bool isAck=false,bool isMsg=false):MSG(isMsg),ACK(isAck){}
	 bool operator==(WaitType &wt) {
		 return MSG == wt.MSG&&ACK == wt.ACK;
	 }
};

struct Wait{
	osal::Sem *sem;
	Msg *msg;
	unsigned int len;
	int res;
	WaitType type;
	WaitType status;
	Wait(osal::Sem *sem, Msg *msg, unsigned int len=0,WaitType type= WaitType()):sem(sem),msg(msg),len(len),res(0),type(type),status(){}
};

class Ublox
{
	struct TaskArg {
		SerialPort*port;
		bool *isRun;
		Ublox *ublox;
	};
    public:
	Ublox(SerialPort&port):port(port){}
	void input(uint8_t c);
    int poll(Msg &msg,unsigned int len);
    int get(Msg &msg,unsigned int len);
    int set(Msg &msg);
	bool start();
    private:
		static const unsigned char sync[2];// = { 0xb5, 0x62 };
		SerialPort&port;
		static const int RECV_BUF_LEN = 512;
        uint8_t recv_buf[RECV_BUF_LEN];
    uint32_t rpos;
    uint32_t wpos;
    uint32_t current_msg_pos;
    InputState state;
    Wait *wait;
	bool isRun;
	TaskArg arg;
    union {
        uint8_t flag;
        struct {
            uint8_t got_pvt : 1;
            uint8_t got_hw : 1;
            uint8_t got_timeutc : 1;
        };
    };
    osal::Sem read_sem;
	osal::Thread back_task;
	bool buf_is_full();
	int len_of_msg_at(unsigned int pos);
	CheckSum checksum_at(unsigned int pos);
	void on_msg_received();
	bool buf_is_empty();
	int read_msg(Msg &msg, uint16_t len, bool ack);
	int wait_ack(MsgClass cls, uint8_t id);
	int write_msg(Msg&msg);
	int msg_copy_from(unsigned int pos, Msg &msg, unsigned int len);
	uint16_t length_in_msg_at(unsigned int pos);
	MsgClass cls_in_msg_at(unsigned int pos);
	uint8_t id_in_msg_at(unsigned int pos);
	unsigned int pos_to_offset(unsigned int pos);
	uint8_t* addr_of_pos(unsigned int pos);
	static int task(void *arg);

};

} // namespace ublox
