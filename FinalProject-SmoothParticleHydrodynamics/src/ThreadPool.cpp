#include "ThreadPool.h"

void ThreadPool::start()
{
	Terminate = false;
	for (int i = 0; i < m_MaxThreads; i++)
	{
		m_threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
	}
}

void ThreadPool::QueueJob(std::function<void()> job, int id)
{
	{
		std::unique_lock<std::mutex> lock(m_Queue);
		jobs.push({ job,id });
	}
	m_MutexCondition.notify_one();
}

void ThreadPool::Stop()
{
	{
		std::unique_lock<std::mutex> lock(m_Queue);
		Terminate = true;
	}
	m_MutexCondition.notify_all();
	for (std::thread& active_thread : m_threads) {
		active_thread.join();
	}
	m_threads.clear();
}

void ThreadPool::Pause()
{
	Paused = true;
	//m_MutexCondition.notify_all();
}

void ThreadPool::Resume()
{
	Paused = false;
	//m_MutexCondition.notify_all();
}

bool ThreadPool::Busy()
{
	bool poolbusy;
	{
		std::unique_lock<std::mutex> lock(m_Queue);
		poolbusy = !jobs.empty();
	}
	return poolbusy;
}

bool ThreadPool::CheckBusyThreads()
{
	if(m_BusyThreads == 0)
		return false;
    return true;
}

ThreadPool::ThreadPool(int max_threads)
	:m_MaxThreads(max_threads)
{
	m_BusyThreads = 0;
}

void ThreadPool::ThreadLoop()
{
	while (true)
	{
		if(Paused) { continue; }
		std::pair<std::function<void()>, int> job;
		{
			std::unique_lock<std::mutex> lock(m_Queue);
			m_MutexCondition.wait(lock, [this] {
				return !jobs.empty() || Terminate;
				});
			if (Terminate) {
				return;
			}
			job = jobs.front();
			jobs.pop();
		}
		//job.first(job.second);
		{
			m_Busy.lock();
			m_BusyThreads++;
			m_Busy.unlock();
		}
		job.first();
		{
			m_Busy.lock();
			m_BusyThreads--;
			m_Busy.unlock();
		}
	}
}