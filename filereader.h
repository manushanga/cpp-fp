#pragma once

#include <string>

#include <uv.h>

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
    static void on_open(uv_fs_t* req);
    static void on_read(uv_fs_t* req);
    static void on_write(uv_fs_t* req);
    static void on_close(uv_fs_t* req);

    uv_fs_t m_req;
};
