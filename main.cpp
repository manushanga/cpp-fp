#include <iostream>
#include <parser.h>
using namespace std;

int main(int argc, char *argv[])
{
    Parser p;
    std::string s;
    s = "void main(pair<int, int > a, int<ii*> b, map<ss>* ss) ;"
        "pair<int, jj> ss;"
        "enum  ENUM  {A=0,V=  450, Hed=2};"
        "namespace  aa { namespace bb{ "
        "class jj<s,f> : private A, public gB<j,df> "
        "{"
            "enum cc{cc, g,h,f};"
            "jj();"
            "hh<tt,uuu,dd > as;"
            "void a();"
            "signed long int a;"
        "};"
        "class XX { class D : public A<B>, private C { float a; class V{ int c; }; }; int s; };"
        "}}";
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
    p.tokenize(s);
    p.parse();
    return 0;
}
