#pragma once
#include <vector>

namespace Net {
	class DownloadManager {
	public:
		DownloadManager(int threadCount);
		~DownloadManager();

		std::future<std::filesystem::path> enqueue(const DownloadTask& task);

	private:
		int m_threadCount;
	}

	struct DownloadTask {
		std::string url;
		std::filesystem::path destination;
		std::vector<std::string> headers;
		std::atomic<size_t>* progress;
	};
}
