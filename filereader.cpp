#include <chrono>
#include <boost/iostreams/device/mapped_file.hpp>

#include "filereader.h"
#include "parser.h"

namespace bio = boost::iostreams;
FileReader::FileReader() :
    m_work(true),
    m_thread(nullptr)
{
}

void FileReader::start()
{
    m_thread = new std::thread(&FileReader::onRun, this);
}

void FileReader::join()
{
    m_work.store(true);
    m_thread->join();
}

void FileReader::stop()
{
    m_work.store(false);
}

void FileReader::onRun()
{
    bio::mapped_file file;

    while (m_work.load())
    {
        bf::path thisPath;
        m_mutex.lock();

        if (m_queue.empty())
        {
            m_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        else
        {
            thisPath = m_queue.front();
            m_queue.pop();
            m_mutex.unlock();
        }

        file.open(thisPath, bio::mapped_file::readonly);
        int len = bf::file_size(thisPath);

        if (len > 0)
        {
            char* buf = new char[len + 2];
            memcpy(buf, file.const_data(), len);
            file.close();
            buf[len] = '\0';
            buf[len + 1] = '\0';
            Parser* p = new Parser(buf, len);
            m_mutOut.lock();
            m_queueOut.push(p);
            m_mutOut.unlock();
        }
    }
}

Parser* FileReader::take()
{
    Parser* p = nullptr;
    m_mutOut.lock();

    if (!m_queueOut.empty())
    {
        p = m_queueOut.front();
        m_queueOut.pop();
    }
    else
    {
        p = nullptr;
    }

    m_mutOut.unlock();
    return p;
}

void FileReader::submit(bf::path filename)
{
    m_mutex.lock();
    m_queue.push(filename);
    m_mutex.unlock();
}


