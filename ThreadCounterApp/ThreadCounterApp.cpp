// ThreadCounterApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <sstream>

using namespace std;


mutex g_logMutex;

void Log(const char* function, const char* msg)
{
    unique_lock<mutex> lock(g_logMutex);
    cout << function << " : " << msg << endl;
}

void Log(const char* function, const stringstream& msg)
{
    Log(function, msg.str().c_str());
}

class ThreadData
{
public:
    ThreadData() : _sum(0) {}
    int _sum;
    mutex _mutex;
};

void ThreadSum(ThreadData& data, int from, int to)
{
    stringstream msg;
    msg << " started - from:" << from << " to:" << to;
    Log(__FUNCTION__, msg);
    for (int val = from; val <= to; val++)
    {
        unique_lock<mutex> lock(data._mutex);
        data._sum += val;
    }

    msg.str("");
    msg.clear() ;
    msg << " done - from:" << from << " to:" << to;
    Log(__FUNCTION__, msg);
}

//struct RangeS {
//    RangeS() : _from(0), _to(0) {}
//    int _from;
//    int _to;
//} Range;

int main()
{
    ThreadData data;
    const int minVal = 0;
    const int maxVal = 1000000;
    cout << "main thread will sum from " << minVal << " to " << maxVal << endl;
    int sum = 0;
    for (int i = minVal; i <= maxVal; i++)
    {
        sum += i;
    }

    cout << "main thread calculated a sum of " << sum << endl;

    const int threadCount = 3;
    int range = (int)((maxVal - minVal) / 3);    
    //cout << "range:" << range << endl;
    int from = 0;
    thread threads[threadCount];
    Log(__FUNCTION__, "starting threads");
    for (int i = 0; i < threadCount; i++)
    {
        int to = (i == (threadCount - 1)) ? maxVal : from + range;
        stringstream msg;
        msg << "threads[" << i << "] will sum from " << from << " to " << to;
        Log(__FUNCTION__, msg);
        threads[i] = thread(ThreadSum, ref(data), from, to);
        from = to + 1;
    }

    Log(__FUNCTION__, "waiting for threads to stop");
    for (int i = 0; i < threadCount; i++)
    {
        threads[i].join();
    }

    Log(__FUNCTION__, "threads have stopped");
    if (sum == data._sum)
    {
        cout << "thread sum returned the correct value of " << data._sum;
    }
    else 
    {
        cout << "thread sum returned the INCORRECT value of " << data._sum;
    }

    int c = getchar();
}
