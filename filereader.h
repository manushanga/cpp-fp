#pragma once

#include<boost/filesystem.hpp>

#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

class Parser;

namespace bf = boost::filesystem;

class FileReader
{
public:
    FileReader();
    void start();
    void join();
    void stop();
    void submit(bf::path filename);
    Parser* take();
protected:
    void onRun();

    std::atomic<bool> m_work;
    std::thread* m_thread;
    std::queue<bf::path> m_queue;
    std::queue<Parser*> m_queueOut;
    std::mutex m_mutex;
    std::mutex m_mutOut;
};
