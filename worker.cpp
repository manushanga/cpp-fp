#include <map>
#include <string>

#include "worker.h"
#include "filereader.h"
#include "parser.h"

Worker::Worker(FileReader* reader) :
    m_reader(reader)
{
}

void Worker::start()
{
    m_thread = new std::thread(&Worker::onRun, this);
}

void Worker::join()
{
    m_thread->join();
}


void Worker::onRun()
{
    while (true)
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
                std::map<std::string, std::string> asgn;
                p->tokenize(asgn);
                p->parse();
                p->print();
            }
           //
        }
    }
}
