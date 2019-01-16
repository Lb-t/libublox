#include <sstream>
#include "serialport.h"
namespace ubloxns
{

	SerialPort::SerialPort() : hComm(INVALID_HANDLE_VALUE) {};

	int SerialPort::open(const char *name)
	{
		std::stringstream ss;
		ss << "\\\\.\\" << name;
		hComm = CreateFile(ss.str().c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_FLAG_OVERLAPPED,
			0);
		if (hComm == INVALID_HANDLE_VALUE)
		{
			return -1;
		}
		return 0;
	}


	int SerialPort::open(const std::string &name) {
		return open(name.c_str());
	}


	int SerialPort::configure(int baudrate, Parity parity, Stopbits stopbits)
	{
		DCB dcb = { 0 };
		if (!GetCommState(hComm, &dcb))
		{
			fprintf(stderr, "GetCommState error:%d\n", GetLastError());
			return -1;
		}
		else
		{
			// DCB is ready for use.
			dcb.BaudRate = baudrate;
			if (parity == Parity::NONE)
			{
				dcb.fParity = FALSE;
				dcb.Parity = NOPARITY;
			}
			else
			{
				dcb.fParity = TRUE;
				if (parity == Parity::EVEN)
				{
					dcb.Parity = EVENPARITY;
				}
				else if (parity == Parity::ODD)
				{
					dcb.Parity = ODDPARITY;
				}
				else
				{
					dcb.Parity = NOPARITY;
				}
			}

			if (stopbits == Stopbits::BITS_1)
			{
				dcb.StopBits = ONESTOPBIT;
			}
			else if (stopbits == Stopbits::BITS_1_5)
			{
				dcb.StopBits = ONE5STOPBITS;
			}
			else
			{
				dcb.StopBits = TWOSTOPBITS;
			}

			dcb.fBinary = TRUE;
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fDtrControl = DTR_CONTROL_DISABLE;
			dcb.fDsrSensitivity = FALSE;
			dcb.fOutX = FALSE;
			dcb.fInX = FALSE;
			dcb.fRtsControl = RTS_CONTROL_DISABLE;
			dcb.ByteSize = 8;

			if (!SetCommState(hComm, &dcb))
			{
				return -1;
			}
			if (!GetCommState(hComm, &dcb))
			{
				return -1;
			}
		}

		// config timeout
		COMMTIMEOUTS timeouts;
		if (!GetCommTimeouts(hComm, &timeouts))
		{
			return -1;
		}
		timeouts.ReadIntervalTimeout = 0;
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.ReadTotalTimeoutConstant = 0;
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 0;

		if (!SetCommTimeouts(hComm, &timeouts))
		{
			return -1;
		}
		return 0;
	}

	int SerialPort::read_timed(void *buf, int len, unsigned int timeout)
	{
		DWORD dwRead;
		OVERLAPPED osReader = { 0 };

		// Create the overlapped event. Must be closed before exiting
		// to avoid a handle leak.
		osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		if (osReader.hEvent == NULL)
		{
			// Error creating overlapped event; abort.
			printf("last error %d\n", GetLastError());
			return -1;
		}

		// Issue read operation.
		if (!ReadFile(hComm, buf, len, NULL,
			&osReader)) // if (!ReadFile(hComm, buf, len, &dwRead,
						// &osReader))
		{
			if (GetLastError() != ERROR_IO_PENDING) // read not delayed?
			{										// Error in communications; report it.
				CloseHandle(osReader.hEvent);
				return -1;
			}
		}
		else
		{
			// read completed immediately
			// HandleASuccessfulRead(buf, dwRead);
			CloseHandle(osReader.hEvent);
			return len;
			//printf("\nlast error %d\n", GetLastError());
			//return SP_ERR_FAIL;
		}

		DWORD dwRes;
		DWORD timeout_ms;
		if (timeout == 0)
		{
			timeout_ms = INFINITE;
		}
		else
		{
			timeout_ms = timeout;
		}
		dwRes = WaitForSingleObject(osReader.hEvent, timeout_ms);
		switch (dwRes)
		{
			// Read completed.
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hComm, &osReader, &dwRead,
				TRUE)) // if (!GetOverlappedResult(hComm,
					   // &osReader, &dwRead, FALSE))
			{								// Error in communications; report it.
			}
			else
			{
				// Read completed successfully.
				// HandleASuccessfulRead(buf, dwRead);
				CloseHandle(osReader.hEvent);
				return dwRead;
			}
			break;

		case WAIT_TIMEOUT:
			CloseHandle(osReader.hEvent);
			// return SP_ERR_TIMEOUT;
			return 0;
			break;

		default:
			// Error in the WaitForSingleObject; abort.
			// This indicates a problem with the OVERLAPPED structure's
			// event handle.
			break;
		}
		CloseHandle(osReader.hEvent);
		return dwRead;
	}

	int SerialPort::read(void *buf, int len)
	{
		return read_timed(buf, len, 0);
	}

	int SerialPort::write(const void *buf, int len)
	{
		OVERLAPPED osWrite = { 0 };
		DWORD dwWritten;
		DWORD dwRes;
		BOOL fRes;

		// Create this write operation's OVERLAPPED structure's hEvent.
		osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		if (osWrite.hEvent == NULL)
		{
			return -1;
		}
		// Issue write.
		if (!WriteFile(hComm, buf, len, &dwWritten, &osWrite))
		{
			if (GetLastError() != ERROR_IO_PENDING)
			{
				// WriteFile failed, but isn't delayed. Report error and abort.
				fRes = FALSE;
			}
			else
			{
				dwRes = WaitForSingleObject(osWrite.hEvent, INFINITE);
			}

			switch (dwRes)
			{
				// OVERLAPPED structure's event has been signaled.
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(hComm, &osWrite, &dwWritten, FALSE))
					fRes = FALSE;
				else
					// Write operation completed successfully.
					fRes = TRUE;
				break;

			default:
				// An error has occurred in WaitForSingleObject.
				// This usually indicates a problem with the
				// OVERLAPPED structure's event handle.
				fRes = FALSE;
				break;
			}
		}
		else
		{
			// WriteFile completed immediately.
			fRes = TRUE;
		}

		CloseHandle(osWrite.hEvent);
		if (fRes)
			return len;
		else
		{
			return 0;
		}
	}

	void SerialPort::close()
	{
		CloseHandle(hComm);
	}

	SerialPort::~SerialPort()
	{
		if (hComm)
		{
			close();
		}
	}

} // namespace ublox

