#include "DownloadManager.h"

namespace Net {
    DownloadManager::DownloadManager(int threadCount) {
        for (int i = 0; i < threadCount; i++) {
            m_threads.emplace_back(&DownloadManager::workerLoop, this);
        }
        m_threadCount = threadCount;
    }

    DownloadManager::~DownloadManager() {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = true;
        }
        m_cv.notify_all();
        for (auto &t : m_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    std::future<std::filesystem::path> DownloadManager::enqueue(const DownloadTask& task) {
        std::packaged_task<std::filesystem::path()> pt(
            [task]() -> std::filesystem::path {
                // TODO: implement real downloading
                return task.destination;
            }
        );
        std::future<std::filesystem::path> future = pt.get_future();

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(pt));
        }

        m_cv.notify_one();
        return future;
    }

    void DownloadManager::workerLoop() {
        while (true) {
            std::packaged_task<std::filesystem::path()> task;
            {
                std::unique_lock<std::mutex> lock(m_mutex);

                m_cv.wait(lock, [this] {
                    return !m_queue.empty() || m_stop;
                });

                if (m_stop && m_queue.empty()) return;
                task = std::move(m_queue.front());
                m_queue.pop();
            }
            task();
        }
    }

    void DownloadManager::waitAll() {

    }
}
