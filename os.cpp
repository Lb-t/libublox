#include "os.h"

namespace osal
{

	Sem::Sem(unsigned int value)
	{
		hd = CreateSemaphore(NULL, value, SHRT_MAX, NULL);
	}

	int Sem::post()
	{
		if (ReleaseSemaphore(hd, 1, NULL))
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}

	int Sem::timedwait(unsigned int timeout)
	{
		DWORD dwWaitResult;
		dwWaitResult = WaitForSingleObject(hd,       // handle to semaphore
			timeout); // zero-second time-out interval

		switch (dwWaitResult)
		{
			// The semaphore object was signaled.
		case WAIT_OBJECT_0:
			return 0;
			break;

			// The semaphore was nonsignaled, so a time-out occurred.
		case WAIT_TIMEOUT:
			return -1;
			break;
		default:
			return -1;
			break;
		}
		return -1;
	}

	int Sem::wait()
	{
		return timedwait(INFINITE);
	}

	int Sem::trywait()
	{
		return timedwait(0);
	}

	Mutex::Mutex() {

	}

	void Mutex::lock()
	{
	}

	bool Mutex::try_lock()
	{
		return false;
	}

	bool Mutex::try_lock_for(unsigned int ms)
	{
		return false;
	}

	void Mutex::unlock()
	{
	}

	LockGuard::LockGuard(Mutex &mtx)
	{
		mtx.lock();
		this->mtx = &mtx;
	}

	LockGuard::~LockGuard() { mtx->unlock(); }

	Timer::Timer() { timer_queue = CreateTimerQueue(); }

	bool Timer::start(unsigned int ms, TimerRoutine routine)
	{
		if (!timer_queue)
		{
			return false;
		}
		//  return CreateTimerQueueTimer(&timer, ontimer, ontimer, NULL, 500, 500,
		 //                              WT_EXECUTEDEFAULT);
		return true;
	}
	bool Thread::start(ThreadRoutine routine, void*arg)
	{
		this->arg.arg = arg;
		this->arg.routine = routine;
		handle = CreateThread(NULL, 0, thread_proc, &this->arg, 0, NULL);
		if (handle == NULL) {
			return false;
		}
		else {
			return true;
		}
	}
	unsigned long Thread::thread_proc(void * arg)
	{
		ThreadArg*thread_arg = (ThreadArg*)arg;
		return thread_arg->routine(thread_arg->arg);
	}
} // namespace osal