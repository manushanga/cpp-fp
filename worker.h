#pragma once
#include <thread>
#include <mutex>
#include <atomic>

#include "common.h"

class FileReader;
class Searcher;

class Worker
{
public:
    Worker(FileReader* reader, Searcher* searcher);
    void start();
    void stop();
    void join();
private:
    void onRun();

    FileReader* m_reader;
    Searcher* m_searcher;
    std::thread *m_thread;
    std::mutex m_mutex;
    std::atomic<bool> m_work;

};
