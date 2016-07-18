#pragma once
#include <thread>
#include <mutex>

class FileReader;

class Worker
{
public:
    Worker(FileReader* reader);
    void start();
    void join();
private:
    void onRun();

    FileReader* m_reader;
    std::thread *m_thread;
    std::mutex m_mutex;
};