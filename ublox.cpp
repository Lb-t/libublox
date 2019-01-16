#include "ublox.h"
#include <stddef.h>
namespace ubloxns {
	const unsigned char Ublox::sync[2] = { 0xb5, 0x62 };
	void Ublox::input(uint8_t c)
	{
		static CheckSum chk; // received
		// static ubx_msg_t *msg = NULL;
		switch (state) {
		case InputState::SYNC_0:
			if (c == sync[0]) {
				state = InputState::SYNC_1;
			}
			break;
		case InputState::SYNC_1:
			if (c == sync[1]) {
				state = InputState::HEAD;
			}
			break;
		case InputState::HEAD:
			if (buf_is_full()) {
				wpos = current_msg_pos;
				state = InputState::SYNC_0;
			}
			else {
				recv_buf[(wpos++) & (RECV_BUF_LEN - 1)] = c;
				if (wpos - current_msg_pos == sizeof(Msg)) {
					if (len_of_msg_at(current_msg_pos)) {
						state = InputState::CONTENT;
					}
					else {
						state = InputState::CHECK_0;
					}
				}
			}
			break;
		case InputState::CONTENT:
			if (buf_is_full()) {
				wpos = current_msg_pos;
				state = InputState::SYNC_0;
			}
			else {
				recv_buf[(wpos++) & (RECV_BUF_LEN - 1)] = c;
				if (current_msg_pos +
					len_of_msg_at(current_msg_pos) ==
					wpos) {
					state = InputState::CHECK_0;
				}
			}
			break;
		case InputState::CHECK_0:
			chk.a = c;
			state = InputState::CHECK_1;
			break;
		case InputState::CHECK_1:
			chk.b = c;
			if (chk == checksum_at(current_msg_pos)) {
				current_msg_pos = wpos;
				on_msg_received();
			}
			else {
				// check fail
				wpos = current_msg_pos;
			}
			state = InputState::SYNC_0;
			break;
		default:
			break;
		}
	}

	int Ublox::poll(Msg & msg, unsigned int len)
	{
		int res;
		MsgClass cls = msg.CLASS;
		uint8_t msg_id = msg.ID;
		res = write_msg(msg);
		if (res < 0) {
			return res;
		}
		res = read_msg(msg, len, false);
		if (res < 0) {
			return res;
		}
		return 0;
	}

	int Ublox::get(Msg & msg, unsigned int len)
	{
		int res;
		MsgClass cls = msg.CLASS;
		uint8_t msg_id = msg.ID;
		res = write_msg(msg);
		if (res < 0) {
			return res;
		}
		res = read_msg(msg, len, true);
		if (res < 0) {
			return res;
		}
		return 0;
	}

	int Ublox::set(Msg & msg)
	{
		int res;
		MsgClass cls = msg.CLASS;
		uint8_t msg_id = msg.ID;
		res = write_msg(msg);
		if (res < 0) {
			return res;
		}
		return wait_ack(cls, msg_id);
	}

	bool Ublox::start()
	{
		isRun = true;
		arg.isRun = &isRun;
		arg.port = &port;
		arg.ublox = this;
		return back_task.start(task, &arg);
	}

	bool Ublox::buf_is_full()
	{
		return wpos - rpos >= RECV_BUF_LEN;
	}

	int Ublox::len_of_msg_at(unsigned int pos)
	{
		return length_in_msg_at(pos) + sizeof(Msg);
	}

	bool Ublox::buf_is_empty()
	{
		return wpos == rpos;
	}

	int Ublox::read_msg(Msg & msg, uint16_t len, bool ack)
	{
		Wait wait(&read_sem, &msg, len, WaitType(ack, true));
		this->wait = &wait;
		int res;
		res = read_sem.timedwait(1000);
		this->wait = nullptr;
		if (res < 0) {
			return -1;
		}
		return wait.res;
	}

	int Ublox::wait_ack(MsgClass cls, uint8_t id)
	{
		Msg msg(cls, static_cast<uint8_t>(id));
		int res;
		Wait wait(&read_sem, &msg, 0, WaitType(true, false));
		this->wait = &wait;
		res = read_sem.timedwait(1000);
		this->wait = nullptr;
		if (res < 0) {
			return -1;
		}
		return wait.res;
	}

	int Ublox::write_msg(Msg & msg)
	{
		int res = port.write(sync, sizeof(sync));
		if (res < 0)
			return -1;
		res = port.write(&msg, sizeof(Msg) + msg.LENGTH);
		if (res < 0)
			return -1;
		CheckSum ck = msg.check_sum();
		res = port.write(&ck, sizeof(ck));
		if (res < 0)
			return -1;
		return msg.LENGTH + sizeof(Msg);
	}

	uint16_t Ublox::length_in_msg_at(unsigned int pos)
	{
		if (pos_to_offset(pos) + sizeof(Msg) - RECV_BUF_LEN == 1) {
			return recv_buf[RECV_BUF_LEN - 1] + recv_buf[0] * 0xffu;
		}
		else {
			return *(uint16_t *)&recv_buf[pos_to_offset(pos + offsetof(Msg, LENGTH))];
		}
	}

	MsgClass Ublox::cls_in_msg_at(unsigned int pos)
	{
		return static_cast<MsgClass>(recv_buf[pos_to_offset(pos + offsetof(Msg, CLASS))]);
	}

	uint8_t Ublox::id_in_msg_at(unsigned int pos)
	{
		return recv_buf[pos_to_offset(pos + offsetof(Msg, ID))];
	}

	unsigned int Ublox::pos_to_offset(unsigned int pos)
	{
		return ((pos) & (RECV_BUF_LEN - 1));
	}

	uint8_t * Ublox::addr_of_pos(unsigned int pos)
	{
		return recv_buf + pos_to_offset(pos);
	}

	int Ublox::msg_copy_from(unsigned int pos, Msg &msg, unsigned int len)
	{
		int readout = len_of_msg_at(pos);
		if ((int)len < readout) {
			readout = len;
		}
		void *buf = &msg;
		if (pos_to_offset(pos) + readout <= RECV_BUF_LEN) {
			memcpy(buf, addr_of_pos(pos), readout);
		}
		else {
			len = RECV_BUF_LEN - pos_to_offset(rpos);
			memcpy(buf, addr_of_pos(pos), len);
			memcpy((char *)buf + len, recv_buf, readout - len);
		}
		return readout;
	}

	CheckSum Ublox::checksum_at(unsigned int pos)
	{
		int offset = pos_to_offset(pos);
		int len = len_of_msg_at(pos);
		CheckSum ck;
		uint8_t *buf;
		if (offset + len <= RECV_BUF_LEN) {
			buf = addr_of_pos(pos);
			for (int i = 0; i < len; ++i)
			{
				ck.a += buf[i];
				ck.b += ck.a;
			}
		}
		else {
			buf = addr_of_pos(pos);
			for (int i = 0; i < RECV_BUF_LEN - offset; ++i)
			{
				ck.a += buf[i];
				ck.b += ck.a;
			}
			buf = recv_buf;
			for (int i = 0; i < offset + len - RECV_BUF_LEN; ++i)
			{
				ck.a += buf[i];
				ck.b += ck.a;
			}
		}
		return ck;
	}

	void Ublox::on_msg_received()
	{
		MsgClass cls;
		uint8_t id;
		while (rpos != current_msg_pos) {
			cls = cls_in_msg_at(rpos);
			id = id_in_msg_at(rpos);
			if (cls == MsgClass::ACK) {
				if (wait && (wait->type.ACK)) {
					Ack ack;
					msg_copy_from(rpos, ack, sizeof(ack));
					if (ack.clsID == wait->msg->CLASS &&
						ack.msgID == wait->msg->ID) {
						if (id != static_cast<uint8_t>(AckId::ACK)) {
							// not ack
							wait->res = -1;
							wait->status = wait->type;
						}
						else {
							wait->status.ACK = 1;
						}
					}
				}
			}
			else {
				if (wait && (wait->type.MSG)) {
					if (id == wait->msg->ID && cls == wait->msg->CLASS) {
						wait->res = msg_copy_from(rpos, *wait->msg, wait->len);
						wait->status.MSG = 1;
					}
				}
				else {
					//on_periodic_msg(rt->rpos);
				}
			}
			if (wait && wait->type == wait->status) {
				wait->sem->post();
			}
			rpos += len_of_msg_at(rpos);
		}
	}

	int Ublox::task(void *arg)
	{
		TaskArg*task_arg = (TaskArg*)arg;
		uint8_t c;
		int res;
		while (*task_arg->isRun) {
			res = task_arg->port->read(&c, sizeof(c));
			if (res == 1) {
				task_arg->ublox->input(c);
			}
			else {
				//assert(0);
			}
		}
		return 0;
	}
}



