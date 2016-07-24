#pragma once

#include<boost/filesystem.hpp>
#include <boost/python.hpp>
#include <boost/python/list.hpp>

#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "common.h"
class Parser;

namespace bf = boost::filesystem;

class FileReader
{
public:
    FileReader(boost::python::list projects, boost::python::list  dependencies);
    FileReader(StringList projects, StringListList dependencies);
    void start();
    void join();
    void stop();
    Parser* take();
protected:
    void onRun();

    std::atomic<bool> m_work;
    std::thread* m_thread;
    std::queue<bf::path> m_queueIn;
    std::queue<Parser*> m_queueOut;
    std::mutex m_outmt;

};
