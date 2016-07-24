#include <iostream>

#include "parser.h"
char *make_buffer(const std::string& str)
{
    char* buf = new char[str.size() + 2];
    memcpy(buf, str.c_str(), str.size());
    buf[str.size() + 1] = '\0';
    buf[str.size()] = '\0';
    return buf;
}
void TEST_1()
{
    std::string s;
    s = "///  dsfafdaf \n"
        "#include <dd.h> \n"
        "#define AA \\n daf \\n d \n"
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


    std::string v = "namespace Ogre { \
            \
            /** Class for serializing a Skeleton to/from XML.\
            @remarks\
                This class behaves the same way as SkeletonSerializer in the main project,\
                but is here to allow conversions to / from XML. This class is \
                deliberately not included in the main project because <UL>\
                <LI>Dependence on Xerces would unnecessarily bloat the main library</LI>\
                <LI>Runtime use of XML is discouraged because of the parsing overhead</LI></UL>\
                This class gives people the option of saving out a Skeleton as XML for examination\
                and possible editing. It can then be converted back to the native format\
                for maximum runtime efficiency.\
            */\
            class XMLSkeletonSerializer\
            {\
            public:\
        \
                XMLSkeletonSerializer();\
                virtual ~XMLSkeletonSerializer();\
                /** Imports a Skeleton from the given XML file.\
                @param filename The name of the file to import, expected to be in XML format.\
                @param pSkeleton The pre-created Skeleton object to be populated.\
                */\
                void importSkeleton(const String& filename, Skeleton* pSkeleton);\
        \
                /** Exports a skeleton to the named XML file. */\
                void exportSkeleton(const Skeleton* pSkeleton, const String& filename);\
        \
            private:\
                TiXmlDocument* mXMLDoc;\
                Skeleton* mSkel;\
        \
            };\
        \
        \
        }";


    std::string g="template<typename T>\n\
                  class StreamSerialiserTests : public CppUnit::TestFixture\n\
    {\n\
        // CppUnit macros for setting up the test suite\n\
        CPPUNIT_TEST_SUITE( StreamSerialiserTests );\n\
        CPPUNIT_TEST(testWriteBasic);\n\
    \n\
        CPPUNIT_TEST_SUITE_END();\n\
    protected:\n\
    public:\n\
        T setUp();\n\
        void tearDown();\n\
    \n\
        void testWriteBasic();\n\
    \n\
    \n\
    };";

    auto buf = make_buffer(g);
    Parser xx("",buf, g.size());
    std::map<std::string, std::string> asgnmnt = {};
    xx.preprocess();
    std::cout<<buf<<std::endl;
    xx.tokenize(asgnmnt);
    xx.parse();
    xx.print();


}
