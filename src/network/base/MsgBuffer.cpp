#include "MsgBuffer.h"
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>
#include <algorithm>
#include <cassert>

namespace tmms {
namespace network {

const char MsgBuffer::kCRLF[] = "\r\n";

MsgBuffer::MsgBuffer(size_t size)
    : buffer_(size), head_(0), tail_(0) {}

char* MsgBuffer::begin() {
    return buffer_.data();
}

const char* MsgBuffer::begin() const {
    return buffer_.data();
}

size_t MsgBuffer::ReadableBytes() const {
    return tail_ - head_;
}

size_t MsgBuffer::WritableBytes() const {
    return buffer_.size() - tail_;
}

void MsgBuffer::EnsureWritableBytes(size_t len) {
    if (WritableBytes() < len) {
        buffer_.resize(tail_ + len);
    }
}

void MsgBuffer::Append(const char* data, size_t len) {
    EnsureWritableBytes(len);
    std::copy(data, data + len, begin() + tail_);
    tail_ += len;
}

void MsgBuffer::Retrieve(size_t len) {
    assert(len <= ReadableBytes());
    head_ += len;
}

void MsgBuffer::RetrieveAll() {
    head_ = tail_ = 0;
}
const char* MsgBuffer::Peek() const {
    return buffer_.data() + head_;
}



ssize_t MsgBuffer::ReadFd(int fd, int* savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];

    size_t writable = WritableBytes();

    vec[0].iov_base = begin() + tail_;
    vec[0].iov_len  = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len  = sizeof extrabuf;

    int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
    ssize_t n = ::readv(fd, vec, iovcnt);

    if (n < 0) {
        *savedErrno = errno;
    } else if (static_cast<size_t>(n) <= writable) {
        tail_ += n;
    } else {
        tail_ = buffer_.size();
        Append(extrabuf, n - writable);
    }

    return n;
}

} // namespace network
} // namespace tmms
