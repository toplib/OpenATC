#pragma once
#include <filesystem>
#include <future>
#include <mutex>
#include <queue>
#include <vector>
#include <thread>

namespace Net {
	struct DownloadTask;

	class DownloadManager {
	public:
		DownloadManager(int threadCount = 4);
		~DownloadManager();

		std::future<std::filesystem::path> enqueue(const DownloadTask& task);
		void waitAll();

	private:
		void workerLoop();
		int m_threadCount;
		bool m_stop = false;

		std::vector<std::thread> m_threads;
		std::queue<std::packaged_task<std::filesystem::path()>> m_queue;
		std::mutex m_mutex;
		std::condition_variable m_cv;

	};

	struct DownloadTask {
		std::string url;
		std::filesystem::path destination;
		std::vector<std::string> headers;
		std::atomic<size_t>* progress;
	};
}
