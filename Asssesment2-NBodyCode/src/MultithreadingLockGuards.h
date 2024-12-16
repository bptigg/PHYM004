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
    IntergatorLockGuard(int); //constructor 
    void UpdateCurrentChecks();
    bool Wait(); //the barrier function
    void Reset(); //resets the barrier 
};

class OutputLockGuard
{
private:
    bool m_kill = false; //if this is set to true the thread that this has been assigned to will be killed 
    int m_WriteLimit = 0; //sets a limit for how far in a array the thread can go through

    std::mutex OutputLock;
public:
    const bool& kill = m_kill; //const public read only variables
    const int& writeLimit = m_WriteLimit;
public:
    void UpdateKill(bool input); //setting functions 
    void UpdateWriteLimit(int Limit);
};