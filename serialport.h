#pragma once
#include <windows.h>
#include <string>

namespace ubloxns
{
	
enum class Parity
{
	NONE,
	EVEN,
	ODD
};

enum class Stopbits
{
	BITS_1,
	BITS_1_5,
	BITS_2
};

class SerialPort
{
  public:
	SerialPort();
	int open(const char *name);
	int open(const std::string &name);
	int configure(int baudrate, Parity parity, Stopbits stopbits);
	int read(void*buf,int len);
	int read_timed(void*buf,int len,unsigned int timeout);
	int write(const void*buf,int len);
	void close();
	~SerialPort();

  private:
	HANDLE hComm;
};

} // namespace ezc
