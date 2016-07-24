#pragma once
#include <thread>
#include <mutex>

#include "common.h"

class FileReader;
class Searcher;

class Worker
{
public:
    Worker(FileReader* reader, Searcher* searcher);
    void start();
    void join();
private:
    void onRun();

    FileReader* m_reader;
    Searcher* m_searcher;
    std::thread *m_thread;
    std::mutex m_mutex;

};
