#pragma once
#include <Windows.h>
#include <stdint.h>

namespace osal {

	using TimerRoutine = void(*)(void *);
	using ThreadRoutine = int(*)(void *);

	void sleep_ms(unsigned ms);

	class Thread {
		struct ThreadArg {
			ThreadRoutine routine;
			void*arg;
		};
	public:
		Thread() {};
		~Thread() {};
		bool start(ThreadRoutine routine, void*arg);
	private:
		ThreadArg arg;
		static unsigned long  thread_proc(void *arg);
		HANDLE handle;
	};

	class Mutex {
	public:
		Mutex();
		void lock();
		bool try_lock();
		bool try_lock_for(unsigned int ms);
		void unlock();
	};


	class Sem {
	public:
		Sem() :Sem(0) {}
		Sem(unsigned int value);
		int post();
		int timedwait(unsigned int timeout);
		int wait();
		int trywait();
	private:
		HANDLE hd;
	};


	class LockGuard {
	public:
		LockGuard(Mutex &mtx);
		~LockGuard();

	private:
		Mutex *mtx;
	};

	class Timer {
	public:
		Timer();
		~Timer();
		bool start(unsigned int ms, TimerRoutine routine);
		void join();

	private:
		HANDLE timer_queue;
		HANDLE timer;
		static void on_timer(void *para, uint8_t TimerOrWaitFired);
	};

} // namespace ezc