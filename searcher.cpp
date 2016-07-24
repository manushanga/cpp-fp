#include <map>

#include "searcher.h"
#include "parser.h"

void Searcher::add(Parser *parser)
{
    m_filesmt.lock();
    auto it =m_files.find(parser->getPath()) ;
    if (it != m_files.end())
    {
        delete it->second;
    }
    m_files.insert(std::make_pair(parser->getPath(),parser->takeNodeRoot()));
    m_filesmt.unlock();
    delete parser;
}

NodeList Searcher::search(std::string name)
{
    std::lock_guard<std::mutex> guard(m_filesmt);

    NodeList rlist;
    for (auto& it : m_files)
    {
        NodeList nlist = it.second->find(name,10);
        if (nlist.size() > 0)
        {
            return nlist;
        }
    }
    return rlist;
}

