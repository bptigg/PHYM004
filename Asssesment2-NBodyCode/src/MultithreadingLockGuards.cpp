#include "MultithreadingLockGuards.h"

IntergatorLockGuard::IntergatorLockGuard(int RequiredChecks)
    :m_RequiredChecks(RequiredChecks), m_CurrentChecks(0)
{
    m_CheckLock.unlock();
}

void IntergatorLockGuard::UpdateCurrentChecks()
{
    m_CheckLock.lock();
    m_CurrentChecks++;
    m_CheckLock.unlock();

    //std::thread::
}

bool IntergatorLockGuard::Wait()
{
    if(m_CurrentChecks < m_RequiredChecks)
    {
        return false;
    }
    return true;
}
void IntergatorLockGuard::Reset()
{
    m_CurrentChecks = 0;
}
