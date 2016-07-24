OBJECTS= \
	parser.o \
	searcher.o \
	filereader.o \
	worker.o \
	codemodel.o
PY_OBJECTS= \
	py-exposer.o

CC=gcc
CXX=g++
INCLUDE= -I/usr/include/python3.5m
CFLAGS= -g -O2 -fPIC  -DUSE_OOURA $(INCLUDE)
CXXFLAGS= -g -O2 -fPIC -std=c++11 -DUSE_OOURA $(INCLUDE)
LDFLAGS= -shared   -L/usr/lib/python3.5/config-3.5m-x86_64-linux-gnu/ -pthread 
LIBNAME_PY = fastparser.so

$(LIBNAME_PY): $(OBJECTS)  $(PY_OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(PY_OBJECTS)  -o $@ -lboost_system  -lboost_filesystem  -lboost_iostreams    -lpython3.5m    -lboost_python3 -lstdc++ 
all: $(EXECUTABLE)
	