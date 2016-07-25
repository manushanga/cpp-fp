#include <chrono>
#include <set>
#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "filereader.h"
#include "parser.h"

namespace bio = boost::iostreams;
FileReader::FileReader(boost::python::list projects, boost::python::list dependencies)
{
    std::set<std::string> paths;
    for (int i=0;i<len(projects);i++)
    {
        std::string sprj =boost::python::extract<std::string>(boost::python::object(projects[i]));
        std::cout<<sprj<<std::endl;
        paths.insert(sprj);
    }

    for (int i=0;i<len(dependencies);i++)
    {
        for (int j=0;j<len(dependencies[i]);j++)
        {
            std::string sprj =boost::python::extract<std::string>(boost::python::object(dependencies[i][j]));
            paths.insert(sprj);
        }

    }

    for (auto& path : paths)
    {
        m_queueIn.push(bf::path(path));
    }

}

FileReader::FileReader(StringList projects, StringListList dependencies) :
    m_work(true),
    m_thread(nullptr)
{
    std::set<std::string> paths;
    for (auto& project : projects)
    {
        paths.insert(project);
    }

    for (auto& project : dependencies)
    {
        for (auto& dep : project)
        {
            paths.insert(dep);
        }
    }

    for (auto& path : paths)
    {
        m_queueIn.push(bf::path(path));
    }

}

void FileReader::start()
{
    m_work.store(true);
    m_thread = new std::thread(&FileReader::onRun, this);
}

void FileReader::join()
{
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

        if (m_queueIn.empty())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        else
        {
            thisPath = m_queueIn.front();
            m_queueIn.pop();
        }

        bf::recursive_directory_iterator it(thisPath);
        bf::recursive_directory_iterator end;

        for (; it != end; it++)
        {
            if (boost::algorithm::ends_with(it->path().c_str(), ".git"))
            {
                it.no_push();
            }

            if (!boost::algorithm::ends_with(it->path().c_str(), ".h"))
                continue;

            if (bf::is_empty(it->path()))
                continue;

            file.open(it->path(), bio::mapped_file::readonly);
            int len = bf::file_size(it->path());

            if (len > 0)
            {
                char* buf = new char[len + 2];
                memcpy(buf, file.const_data(), len);
                file.close();
                buf[len] = '\0';
                buf[len + 1] = '\0';
                Parser* p = new Parser(it->path().generic_string(), buf, len);
                m_outmt.lock();
                m_queueOut.push(p);
                m_outmt.unlock();
            }
        }

    }
}

Parser* FileReader::take()
{
    Parser* p = nullptr;
    m_outmt.lock();

    if (!m_queueOut.empty())
    {
        p = m_queueOut.front();
        m_queueOut.pop();
    }
    else
    {
        p = nullptr;
    }

    m_outmt.unlock();
    return p;
}

