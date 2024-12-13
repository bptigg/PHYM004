//Definition of the various lock guard classes required to stop race condtions
#pragma once
#include <thread>
#include <mutex>

class IntergatorLockGuard 
{
private:
    int m_RequiredChecks; //the number of checks that need to be made before it will allow the code to continue i.e. number of intergrators that have update the body position
    int m_CurrentChecks;
    std::mutex m_CheckLock;
public:
    const int& RequiredChecks = m_RequiredChecks;
    const int& CurrentChecks = m_CurrentChecks;
public:
    IntergatorLockGuard(int);
    void UpdateCurrentChecks();
    bool Wait();
    void Reset();
};

class OutputLockGuard
{
private:
    bool m_kill = false;
    int m_WriteLimit = 0;

    std::mutex OutputLock;
public:
    const bool& kill = m_kill;
    const int& writeLimit = m_WriteLimit;
public:
    void UpdateKill(bool input);
    void UpdateWriteLimit(int Limit);
};