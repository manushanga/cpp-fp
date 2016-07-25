#include <map>
#include <string>

#include "worker.h"
#include "filereader.h"
#include "parser.h"
#include "searcher.h"

Worker::Worker(FileReader* reader, Searcher *searcher) :
    m_reader(reader),
    m_searcher(searcher)
{
}

void Worker::start()
{
    m_work.store(true);
    m_thread = new std::thread(&Worker::onRun, this);
}

void Worker::join()
{
    m_thread->join();
}

void Worker::stop()
{
    m_work.store(false);
}

void Worker::onRun()
{
    while (m_work.load())
    {
        auto p = m_reader->take();

        if (p == nullptr)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        else
        {
            if (p->preprocess() == 0)
            {
                std::map<std::string, std::string> asgn= {{"_OgreExport", ""}};
                p->tokenize(asgn);
                p->parse();
                //p->print();
                m_searcher->add(p);
            }
           //
        }
    }
}
