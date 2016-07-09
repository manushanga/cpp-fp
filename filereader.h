#pragma once

#include <string>

class File
{
public:
    virtual void onOpen();
    virtual void onChange();
    virtual void onRead();
    virtual void onWrite();
    virtual void onClose();

    void open(std::string filename, int flags);

protected:

};
