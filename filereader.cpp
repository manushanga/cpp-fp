#include "filereader.h"

void File::open(std::string filename, int flags)
{
    m_req.data = (void*) this;
    uv_fs_open(uv_default_loop(), &m_req, filename.c_str(), flags, 0, File::on_open);
}

void File::on_open(uv_fs_t *req)
{
//uv_buf_init();

    static_cast<File*>(req->data)->onOpen();
 //   uv_fs_read(uv_default_loop(), req, req->result,
}
