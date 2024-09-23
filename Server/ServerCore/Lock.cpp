#include "pch.h"
#include "Lock.h"
#include "Types.h"



void Lock::Writelock()
{
	const uint32 lockThreadID = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;

	if (LThreadId == lockThreadID)
	{
		_writeCount++;
		return;
	}

	const int64 beginTick = ::GetTickCount64();
	const uint32 desired = ((LThreadId << 16) & WRITE_THREAD_MASK);


	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expect = EMPTY_FLAG;

			if (_lockFlag.compare_exchange_strong(OUT expect, desired))
			{
				_writeCount++;
				return;
			}
		}

		if (::GetTickCount64() - beginTick >= ACCQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::WriteUnlock()
{
	if ((_lockFlag.load() & READ_COUNT_MASK) != 0)
		CRASH("INVALID_WRITE_UNLOCK_ORDER");

	const int32 lockCount = --_writeCount;
	if (lockCount == 0)
		_lockFlag.store(EMPTY_FLAG);
}

void Lock::Readlock()
{
	const uint32 lockThreadID = (_lockFlag.load() & WRITE_THREAD_MASK) >> 16;

	if (LThreadId == lockThreadID)
	{
		_lockFlag.fetch_add(1);
		return;
	}

	const int64 beginTick = ::GetTickCount64();

	while (true)
	{
		for (uint32 spinCount = 0; spinCount < MAX_SPIN_COUNT; spinCount++)
		{
			uint32 expect = EMPTY_FLAG;

			if (_lockFlag.compare_exchange_strong(OUT expect, expect + 1))
				return;
		}

		if (::GetTickCount64() - beginTick >= ACCQUIRE_TIMEOUT_TICK)
			CRASH("LOCK_TIMEOUT");

		this_thread::yield();
	}
}

void Lock::ReadUnlock()
{
	if ((_lockFlag.fetch_sub(1) & READ_COUNT_MASK) == 0)
		CRASH("MULTIPLE_UNLOCK");
}
