#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <cstdio>

#include "parser.h"
using namespace std;
namespace bf = boost::filesystem;

int main(int argc, char *argv[])
{

    std::string s;
    s = "///  dsfafdaf \n"
        "#include <dd.h> \n"
        "#define AA \ daf \ d \n"
        "void main(pair<int, int > a, int<ii*> b, map<ss>* ss) ;\n"
        "pair<int, jj> ss;\n"
        "enum  ENUM  {A=0,V=  450, Hed=2};\n"
        "enum class XXE : int {A, H}; \n"
        "namespace { xx; ff;}\n"
        "namespace  nam1 { namespace nam2 { \n"
        "class jj<s,f> : private A, public gB<j,df> \n"
        "{\n"
            "typedef XX<c,C> dd; \n"
            "class XXv {};\n"
            "enum cf : int {dd,g};\n"
            "enum gc : unsigned int {ff ,g};\n"
            "enum cc{cc, g,h,f};\n"
            "jj(aaa::int g,h g) {}\n"
            "~j1j() {}\n"
            "hh<tt,uuu,dd > as;\n"

            "void a();\n"
            "signed long int a;\n"

            "cc::int aa(h<d,gd> f) {  int g; h;} \n"

        "};"
        "class XX { class D : public A<B>, private C { float a; class V{ int c; }; }; int s; };\n"
        "T<D,t> func(int ads, XC<F,d,f > d) { a= p+1(1,2,3); }\n"
        "}}\n";
    /*
Parser xx;
xx.tokenize(s);
xx.parse();
xx.print();
exit(0);
*/
bf::recursive_directory_iterator it("/home/madura/ogre-master");
    bf::recursive_directory_iterator end;
    for (;it!=end;it++)
    {
        Parser p;
        std::cout<<it->path()<<std::endl;
        //std::string fpath("/home/madura/ogre-master/OgreMain/include/OgreRenderTargetListener.h" /*it->path().c_str()*/);
        std::string fpath(it->path().c_str());

        if (!boost::algorithm::ends_with( fpath, ".cpp")  &&  !boost::algorithm::ends_with(fpath, ".h"))
            continue;

        FILE* f=fopen( fpath.c_str(),"r");

        int len =bf::file_size( fpath);
        char *buf = new char[len+ 2];


        int i=0;
        while (!feof(f))
        {
            buf[i] = fgetc(f);
            i++;
        }

        fclose(f);

        buf[len]  = '\0';
        buf[len+1]  = '\0';

        if (p.preprocess(buf, len) == 0)
        {
            //std::cout<<buf<<std::endl;
            std::map<std::string, std::string> assignment = { {"_OgreExport", ""} };
            p.tokenize(buf, len, assignment);

            p.parse();

            p.print();
        }


        delete []buf;

    }


/*
    s= "int ao_plugin_test(); \
            ao_info *ao_plugin_driver_info(); \
            int ao_plugin_device_init(ao_device *device); \
            int ao_plugin_set_option(ao_device *device, const char *key, const char *value); \
            int ao_plugin_open(ao_device *device, ao_sample_format *format); \
            int ao_plugin_play(ao_device *device, const char *output_samples, \
                    uint_32 num_bytes); \
            int ao_plugin_close(ao_device *device);\
            void ao_plugin_device_clear(ao_device *device);\
            const char *ao_plugin_file_extension();";*/

    //p.tokenize(s);

    return 0;
}
