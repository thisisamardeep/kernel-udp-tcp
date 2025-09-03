#include "thread.h"
#include "timestamp.h"

#include <algorithm>
#include <cerrno>
#include <sched.h>
#include <unistd.h>
#include <pthread.h>

namespace AsioUtils {

uint64_t Thread::CurrentThreadId() noexcept {
    return (uint64_t)syscall(SYS_gettid);
}

uint32_t Thread::CurrentThreadAffinity() noexcept {
    int affinity = sched_getcpu();
    return (affinity < 0) ? 0 : affinity;
}

void Thread::SleepFor(const Timespan &timespan) noexcept {
    if (timespan < 0)
        return;
    if (timespan == 0)
        return Yield();
    struct timespec req, rem;
    req.tv_sec = timespan.seconds();
    req.tv_nsec = timespan.nanoseconds() % 1000000000;
    while (nanosleep(&req, &rem) != 0) {
        if (errno == EINTR)
            req = rem;
        else
            break;
    }
}

void Thread::Yield() noexcept {
    sched_yield();
}

std::bitset<64> Thread::GetAffinity() {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    int result = sched_getaffinity(0, sizeof(cpu_set_t), &cpuset);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to get the current thread CPU affinity!");
    std::bitset<64> affinity;
    for (int i = 0; i < std::min(CPU_SETSIZE, 64); ++i)
        if (CPU_ISSET(i, &cpuset))
            affinity.set(i);
    return affinity;
}

std::bitset<64> Thread::GetAffinity(std::thread &thread) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    int result = pthread_getaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to get the given thread CPU affinity!");
    std::bitset<64> affinity;
    for (int i = 0; i < std::min(CPU_SETSIZE, 64); ++i)
        if (CPU_ISSET(i, &cpuset))
            affinity.set(i);
    return affinity;
}

void Thread::SetAffinity(const std::bitset<64> &affinity) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = 0; i < std::min(CPU_SETSIZE, 64); ++i)
        if (affinity[i])
            CPU_SET(i, &cpuset);
    int result = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to set the current thread CPU affinity!");
}

void Thread::SetAffinity(std::thread &thread, const std::bitset<64> &affinity) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    for (int i = 0; i < std::min(CPU_SETSIZE, 64); ++i)
        if (affinity[i])
            CPU_SET(i, &cpuset);
    int result = pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set_t), &cpuset);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to set the given thread CPU affinity!");
}

ThreadPriority Thread::GetPriority() {
    struct sched_param sched;
    int policy;
    int result = sched_getparam(0, &sched);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to get the current thread priority!");
    policy = sched_getscheduler(0);
    if ((policy == SCHED_FIFO) || (policy == SCHED_RR)) {
        if (sched.sched_priority < 15)
            return ThreadPriority::IDLE;
        else if (sched.sched_priority < 30)
            return ThreadPriority::LOWEST;
        else if (sched.sched_priority < 50)
            return ThreadPriority::LOW;
        else if (sched.sched_priority < 70)
            return ThreadPriority::NORMAL;
        else if (sched.sched_priority < 85)
            return ThreadPriority::HIGH;
        else if (sched.sched_priority < 99)
            return ThreadPriority::HIGHEST;
        else
            return ThreadPriority::REALTIME;
    } else
        return ThreadPriority::NORMAL;
}

ThreadPriority Thread::GetPriority(std::thread &thread) {
    int policy;
    struct sched_param sched;
    int result = pthread_getschedparam(thread.native_handle(), &policy, &sched);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to get the given thread priority!");
    if ((policy == SCHED_FIFO) || (policy == SCHED_RR)) {
        if (sched.sched_priority < 15)
            return ThreadPriority::IDLE;
        else if (sched.sched_priority < 30)
            return ThreadPriority::LOWEST;
        else if (sched.sched_priority < 50)
            return ThreadPriority::LOW;
        else if (sched.sched_priority < 70)
            return ThreadPriority::NORMAL;
        else if (sched.sched_priority < 85)
            return ThreadPriority::HIGH;
        else if (sched.sched_priority < 99)
            return ThreadPriority::HIGHEST;
        else
            return ThreadPriority::REALTIME;
    } else
        return ThreadPriority::NORMAL;
}

void Thread::SetPriority(ThreadPriority priority) {
    int policy = SCHED_RR;
    struct sched_param sched;
    sched.sched_priority = 50;
    switch (priority) {
        case ThreadPriority::IDLE:
            sched.sched_priority = 1;
            break;
        case ThreadPriority::LOWEST:
            sched.sched_priority = 15;
            break;
        case ThreadPriority::LOW:
            sched.sched_priority = 30;
            break;
        case ThreadPriority::NORMAL:
            sched.sched_priority = 50;
            break;
        case ThreadPriority::HIGH:
            sched.sched_priority = 70;
            break;
        case ThreadPriority::HIGHEST:
            sched.sched_priority = 85;
            break;
        case ThreadPriority::REALTIME:
            sched.sched_priority = 99;
            break;
    }
    int result = sched_setparam(0, &sched);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to set the current thread priority!");
}

void Thread::SetPriority(std::thread &thread, ThreadPriority priority) {
    int policy = SCHED_RR;
    struct sched_param sched;
    sched.sched_priority = 50;
    switch (priority) {
        case ThreadPriority::IDLE:
            sched.sched_priority = 1;
            break;
        case ThreadPriority::LOWEST:
            sched.sched_priority = 15;
            break;
        case ThreadPriority::LOW:
            sched.sched_priority = 30;
            break;
        case ThreadPriority::NORMAL:
            sched.sched_priority = 50;
            break;
        case ThreadPriority::HIGH:
            sched.sched_priority = 70;
            break;
        case ThreadPriority::HIGHEST:
            sched.sched_priority = 85;
            break;
        case ThreadPriority::REALTIME:
            sched.sched_priority = 99;
            break;
    }
    int result = pthread_setschedparam(thread.native_handle(), policy, &sched);
    if (result != 0)
        throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Failed to set the given thread priority!");
}

}