//
// Created by koushiro on 9/7/18.
//

#pragma once

#include <thread>

#include "tento/base/Common.hpp"

///////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32

#else // unix

#include <fcntl.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/syscall.h> //Use gettid() syscall under linux to get thread id

#elif __FreeBSD__
#include <sys/thr.h> //Use thr_self() syscall under FreeBSD to get thread id
#endif

#endif // unix
///////////////////////////////////////////////////////////////////////////////

NAMESPACE_BEGIN(tento)

// Copy from spdlog/include/spdlog/details/os.h
//
// Return current thread id as size_t
// It exists because the std::this_thread::get_id() is much slower(especially
// under VS 2013)
inline size_t thread_id()
{
#ifdef _WIN32
    return static_cast<size_t>(::GetCurrentThreadId());
#elif __linux__
#if defined(__ANDROID__) && defined(__ANDROID_API__) && (__ANDROID_API__ < 21)
#define SYS_gettid __NR_gettid
#endif
    return static_cast<size_t>(syscall(SYS_gettid));
#elif __FreeBSD__
    long tid;
    thr_self(&tid);
    return static_cast<size_t>(tid);
#elif __APPLE__
    uint64_t tid;
    pthread_threadid_np(nullptr, &tid);
    return static_cast<size_t>(tid);
#else // Default to standard C++11 (other Unix)
    return static_cast<size_t>(std::hash<std::thread::id>()(std::this_thread::get_id()));
#endif
}

/// Thread RAII, move-only.
class Thread {
public:
    enum class DtorAction { Join, Detach };

    explicit Thread(std::thread&& t, DtorAction action = DtorAction::Join)
        : action_(action), t_(std::move(t)) {}

    ~Thread() {
        if (t_.joinable()) {
            switch (action_) {
                case DtorAction::Join:   t_.join();   break;
                case DtorAction::Detach: t_.detach(); break;
            }
        }
    }

    Thread(Thread&&) = default;
    Thread& operator=(Thread&&) = default;

    std::thread& Get() { return t_; }

private:
    DtorAction action_;
    std::thread t_;
};

NAMESPACE_END(tento)