
#include <boost/python.hpp>
#include <boost/python/class.hpp>
#include <boost/python/module_init.hpp>
#include <boost/python/def.hpp>
#include <boost/python/call_method.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "common.h"
#include "codemodel.h"
#include "searcher.h"
#include "filereader.h"
#include "worker.h"

using namespace boost::python;
/*
class GILLock
{
public:
    GILLock()  { state_ = PyGILState_Ensure(); }
    ~GILLock() { PyGILState_Release(state_);   }
private:
    PyGILState_STATE state_;
};

class PlayerEventsImpl : public Vrok::Player::Events, public wrapper<Vrok::Player::Events>
{
public:
    void QueueNext()
    {
        GILLock lock;
        this->get_override("QueueNext")();
    }
};

class ScopedGILRelease
{
public:
    inline ScopedGILRelease()
    {
        m_thread_state = PyEval_SaveThread();
    }

    inline ~ScopedGILRelease()
    {
        PyEval_RestoreThread(m_thread_state);
        m_thread_state = NULL;
    }

private:
    PyThreadState * m_thread_state;
};

class ThreadPool_PyWrap : public Vrok::ThreadPool
{
public:
    ThreadPool_PyWrap(size_t size) :
        Vrok::ThreadPool(size)
    {}

    void JoinThreads()
    {
        std::cout<<"542352"<<std::endl;
        ScopedGILRelease scope;
        Vrok::ThreadPool::JoinThreads();
    }

};
*/
BOOST_PYTHON_MODULE(fastparser)
{
    PyEval_InitThreads();

    class_<NodeList>("NodeList")
            .def(vector_indexing_suite<NodeList>());
    class_<StringListList>("StringListList")
            .def(vector_indexing_suite<std::vector<std::string>>());
    class_<StringList>("StringList")
            .def(vector_indexing_suite<std::string>());

    class_<Searcher, boost::noncopyable>("Searcher",boost::python::init<>())
            .def("search", &Searcher::search);

    class_<Worker, boost::noncopyable>("Worker",boost::python::init<FileReader*, Searcher*>())
            .def("start", &Worker::start)
            .def("stop",&Worker::stop)
            .def("join",&Worker::join);

    class_<FileReader, boost::noncopyable>("FileReader",boost::python::init<list, list>())
            .def("start", &FileReader::start)
            .def("stop", &FileReader::stop)
            .def("join",&FileReader::join);

    class_<Node, boost::noncopyable>("Node",boost::python::no_init)
            .def("getName", &Node::getNameByVal)
            .def("__str__",&Node::printToStr)
            .def("getType", &Node::getType)
            .def("find", &Node::find);

}
