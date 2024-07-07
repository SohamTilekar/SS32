#include <atomic>
#include <condition_variable>
#include <fstream>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

class Logger {
  public:
    Logger(const std::string &filename, bool enable = true)
        : log_file(filename, std::ios::app), done(false),
          logging_enabled(enable) {
        if (!log_file.is_open()) {
            throw std::runtime_error("Unable to open log file");
        }
        logging_thread = std::thread(&Logger::processEntries, this);
        if (!logging_enabled) {
            done = true;
        }
    }

    ~Logger() {
        done = true;
        cv.notify_all();
        if (logging_thread.joinable()) {
            logging_thread.join();
        }
    }

    void log(const std::string &message) {
        if (logging_enabled) {
            std::unique_lock<std::mutex> lock(mtx);
            log_queue.push(message);
            cv.notify_all();
        }
    }

  private:
    void processEntries() {
        while (!done || !log_queue.empty()) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return !log_queue.empty() || done; });

            while (!log_queue.empty()) {
                if (logging_enabled) {
                    log_file << log_queue.front();
                }
                log_queue.pop();
            }
        }
    }

    std::ofstream log_file;
    std::thread logging_thread;
    std::mutex mtx;
    std::condition_variable cv;
    std::queue<std::string> log_queue;
    std::atomic<bool> done;
    bool logging_enabled;
};
