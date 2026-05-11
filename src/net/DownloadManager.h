#pragma once
#include <vector>
#include <thread>

namespace Net {
	class DownloadManager {
	public:
		DownloadManager(int threadCount = 4);
		~DownloadManager();

		std::future<std::filesystem::path> enqueue(const DownloadTask& task);
		void waitAll();

	private:
		void workerLoop();
		int m_threadCount;

		std::vector<std::thread> m_threads;
		std::queue<std::packaged_task<std::filesystem::path>> m_queue;
		std::mutex m_mutex;


	}

	struct DownloadTask {
		std::string url;
		std::filesystem::path destination;
		std::vector<std::string> headers;
		std::atomic<size_t>* progress;
	};
}
