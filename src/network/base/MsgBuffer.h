#ifndef TMMS_NETWORK_MSGBUFFER_H
#define TMMS_NETWORK_MSGBUFFER_H

#include <vector>
#include <cstddef>
#include <cstdint>
#include <unistd.h>
namespace tmms {
namespace network {

class MsgBuffer
{
public:
    explicit MsgBuffer(size_t size = 2048);
    const char* Peek() const;   // 查看可读数据起始地址
    void RetrieveAll();         // 清空缓冲区
    size_t ReadableBytes() const;
    size_t WritableBytes() const;

    char* begin();
    const char* begin() const;

    void Append(const char* data, size_t len);

    ssize_t ReadFd(int fd, int* savedErrno);

    void Retrieve(size_t len);
    

    static const char kCRLF[];

private:
    void EnsureWritableBytes(size_t len);

private:
    std::vector<char> buffer_;
    size_t head_;
    size_t tail_;
};

} // namespace network
} // namespace tmms

#endif
