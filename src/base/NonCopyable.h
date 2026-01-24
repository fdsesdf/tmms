#pragma once
namespace tmms
{
    namespace base
    {
        class NonCopyable
        {
        protected:
            NonCopyable() = default;
            ~NonCopyable() = default;
            NonCopyable(const NonCopyable&) = delete;
            NonCopyable& operator=(const NonCopyable&) = delete;
        };
    }
}
