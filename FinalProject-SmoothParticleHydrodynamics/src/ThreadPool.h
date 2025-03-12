//definition of the thread pool
#ifndef SPH_THREAD_POOL
#define SPH_THREAD_POOL

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <functional>
#include <condition_variable>

class ThreadPool
{
public:
	void start();
	void QueueJob(std::function<void()> job, int id);
	void Stop();
	void Pause();
	void Resume();
	bool Busy();
	bool CheckBusyThreads();

	ThreadPool(int max_threads);
private:
	void ThreadLoop();

	bool Paused = false;
	int m_BusyThreads;

	bool Terminate = false;
	std::mutex m_Queue;
	std::mutex m_Busy;
	std::condition_variable m_MutexCondition;
	std::vector<std::thread> m_threads;
	std::queue<std::pair<std::function<void()>, int>> jobs;

	int m_MaxThreads;

};

#endif