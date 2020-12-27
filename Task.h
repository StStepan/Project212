#ifndef THREADPOOL_TASK_H
#define THREADPOOL_TASK_H

#include "common.h"
#include <functional>
#include <windows.h>


typedef std::function<void()> Task;

class Future {
public:
    virtual bool wait(int millis = INFINITE) = 0;
};

class FutureTask : public Future {
public:
    //win32 syncro
    FutureTask(Task task) : task(task) {
        event = CreateEventW(
                nullptr, //security
                true,    //manual 
                false,   //initial state of event (not happened)
                nullptr  //name
        );
    }

    ~FutureTask() {
        CloseHandle(event);
    }

    void complete() {
        completed = true;
        SetEvent(event); //get event into completed(happened) condition
    }

    Task getTask() {
        return task;
    }

    bool wait(int millis) override {
        if (completed) return true;
        //wait for event to become completed
        auto status = WaitForSingleObject(event, millis);

        switch (status) {
            case WAIT_OBJECT_0:
                return true;
            case WAIT_TIMEOUT:
                return false;
            case WAIT_FAILED:
                throw;
            default:
                throw;
        }
    }

private:
    Task task;
    bool completed = false;
    HANDLE event;
};

#endif
