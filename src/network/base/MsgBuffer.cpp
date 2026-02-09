#include "MsgBuffer.h"
#include <errno.h>
#include <sys/uio.h>

using namespace tmms;
using namespace tmms::network;

const char MsgBuffer::kCRLF[] = "\r\n";

const size_t MsgBuffer::kCheapPrepend;
const size_t MsgBuffer::kInitialSize;

ssize_t MsgBuffer::readFd(int fd, int* savedErrno)
{
    char extrabuf[65536];
    struct iovec vec[2];

    const size_t writable = writableBytes();

    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len  = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len  = sizeof extrabuf;

    const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;

    const ssize_t n = ::readv(fd, vec, iovcnt);   // ⭐改这里

    if (n < 0)
    {
        if (savedErrno)
            *savedErrno = errno;
    }
    else if (static_cast<size_t>(n) <= writable) // ⭐改这里
    {
        writerIndex_ += n;
    }
    else
    {
        writerIndex_ = buffer_.size();
        append(extrabuf, n - writable);
    }

    return n;
}
