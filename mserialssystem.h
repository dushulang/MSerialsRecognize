#ifndef MSERIALSSYSTEM_H
#define MSERIALSSYSTEM_H

#include <chrono>
#include <mutex>
#include <process.h>
#include <Windows.h>

#define sleep(ms)	std::this_thread::sleep_for(std::chrono::milliseconds(ms))

class MEvent
{
private:
    std::mutex mtx;
    HANDLE evt;
    int state;
public:
    MEvent():
        state(0)
    {
        evt = CreateEventW(FALSE,TRUE,FALSE,NULL);
        ::ResetEvent(evt);
    }
    const HANDLE &get(){return evt;}
    int SetEvent() {std::lock_guard<std::mutex> lck(mtx);::SetEvent(evt);return (state = 1);}
    int ResetEvent(){std::lock_guard<std::mutex> lck(mtx);::ResetEvent(evt);return (state = 0);}
    int &State() { return state; }
};



#endif // MSERIALSSYSTEM_H
