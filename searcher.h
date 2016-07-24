#pragma once

#include <map>
#include <mutex>
#include <thread>

#include "common.h"

class Node;
class Parser;
class Worker;

class Searcher
{
public:
    void add(Parser* parser);
    NodeList search(std::string name);
private:
    std::map<std::string, Node*> m_files;
    std::mutex m_filesmt;
    Worker* m_worker;
};
