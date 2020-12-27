#ifndef THREADPOOL_TASKQUEUE_H
#define THREADPOOL_TASKQUEUE_H

#include "common.h"
#include <windows.h>
#include <queue>
#include "Task.h"

class TaskQueue {
public:

    TaskQueue() {
        InitializeCriticalSection(&critSect);
        InitializeConditionVariable(&condVar);
    }
    //securing access of only 1 thread to Read\write
    FutureTask *waitNext() {
        EnterCriticalSection(&critSect);
        FutureTask *task;

        try {
            while (queue.empty()) {
                SleepConditionVariableCS(&condVar, &critSect, 1000);
            }
            task = queue.front();
            queue.pop();

        } catch (...) {
            LeaveCriticalSection(&critSect);
            throw;
        }

        LeaveCriticalSection(&critSect);
        return task;
    }

    //securing access of only 1 thread to Read\write
    void add(FutureTask *task) {
        EnterCriticalSection(&critSect);

        try {
            queue.push(task);
            WakeConditionVariable(&condVar);

        } catch (...) {
            LeaveCriticalSection(&critSect);
            throw;
        }
        LeaveCriticalSection(&critSect);
    }

private:
    CRITICAL_SECTION critSect;
    CONDITION_VARIABLE condVar;
    std::queue<FutureTask *> queue;
};


#endif
