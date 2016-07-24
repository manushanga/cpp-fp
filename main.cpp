#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>

#include <cstdio>
#include <string>

#include "parser.h"
#include "filereader.h"
#include "worker.h"
#include "searcher.h"

using namespace std;
namespace bf = boost::filesystem;
namespace bio = boost::iostreams;

void TEST_1();

int main(int argc, char* argv[])
{
    //TEST_1();
    //exit(0);

    bf::recursive_directory_iterator it(argv[1]);
    bf::recursive_directory_iterator end;
    bio::mapped_file file;
    FileReader reader({"/home/madura/ogre-master/OgreMain/include"},{});
    Searcher searcher;
    Worker worker(&reader, &searcher);


    reader.start();
    worker.start();

    sleep(2);
    std::cout<<"ss"<<std::endl;
    auto nlist = searcher.search("SmallVectorTemplateCommon");
    for (auto& n : nlist)
    {
        std::string out;
        n->print(out);
        std::cout<<out<<std::endl;
    }


    reader.join();
    worker.join();

    return 0;
}
