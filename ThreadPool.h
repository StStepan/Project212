#ifndef THREADPOOL_THREADPOOL_H
#define THREADPOOL_THREADPOOL_H

#include "common.h"
#include <iostream>
#include "TaskQueue.h"

class ThreadPool {
public:
	ThreadPool(int threadCount) : threadCount(threadCount) {
		threads = (HANDLE*)malloc(sizeof(HANDLE) * threadCount);
		//create amount of set threads
		for (int i = 0; i < threadCount; i++) {
			threads[i] = CreateThread(
				nullptr, //security attribute
				0, //default stack size
				ThreadPool::RunThreadLoop, //point to function for the thread to do
				this, //pointer to the object of threadpool
				0, //immideate start up
				nullptr //ID of created thread
			);
		}
	}

	Future* schedule(Task task) {
		auto* future = new FutureTask(task);
		taskQueue.add(future);
		return future;
	}

	virtual ~ThreadPool() {
		free(threads);
	}

private:
	int threadCount;
	HANDLE* threads;

	TaskQueue taskQueue;

	[[noreturn]] static DWORD WINAPI RunThreadLoop(LPVOID param) {
		auto* thiz = (ThreadPool*)param;//pointer to threadpool
		while (true) {
			FutureTask* future = thiz->taskQueue.waitNext(); //get future from task queue
			Task task = future->getTask();//get task from future
			task(); //delegate
			future->complete(); //we've completed task
		}
	}
};


#endif
