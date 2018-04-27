#ifndef PT_SYSTEM_THREADPOOL_H
#define PT_SYSTEM_THREADPOOL_H

struct IPTSThreadPool
{
	virtual bool QueueUserWorkItem(void(*WorkCallback)(void *), void *Object) = 0;
};

#endif
