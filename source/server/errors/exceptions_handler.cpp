#include "errors/exceptions_handler.h"

#include "stack_trace.h"
#include "timestamp.h"
#include "validate_aligned_storage.h"
#include "filesystem/path.h"

#include <cstring>
#include <exception>
#include <iostream>
#include <csignal>
#include <unistd.h>
#include "countof.h"
#include "format.h"

namespace AsioUtils {

class ExceptionsHandler::Impl {
public:
    Impl() : _initialized(false), _handler(ExceptionsHandler::Impl::DefaultHandler) {}

    static ExceptionsHandler::Impl &GetInstance() { return ExceptionsHandler::GetInstance().impl(); }

    void SetupHandler(const std::function<void(const SystemException &, const StackTrace &)> &handler) {
        if (!handler)
            return;
        _handler = handler;
    }

    void SetupProcess() {
        if (_initialized)
            return;
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = SignalHandler;
        sa.sa_flags = SA_SIGINFO;
        int signals[] = {SIGABRT, SIGALRM, SIGBUS,  SIGFPE, SIGHUP,  SIGILL,  SIGINT, SIGPIPE,
                         SIGPROF, SIGQUIT, SIGSEGV, SIGSYS, SIGTERM, SIGXCPU, SIGXFSZ};
        for (size_t i = 0; i < countof(signals); ++i) {
            int result = sigaction(signals[i], &sa, nullptr);
            if (result != 0)
                throw AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException(format("Failed to setup signal handler - {}", signals[i]));
        }
        _initialized = true;
    }

    static void SetupThread() {}

private:
    bool _initialized;
    std::function<void(const SystemException &, const StackTrace &)> _handler;

    static void DefaultHandler(const SystemException &exception, const StackTrace &trace) {
        std::cerr << exception;
        std::cerr << "Stack trace:" << std::endl;
        std::cerr << trace;
    }

    static void SignalHandler(int signo, siginfo_t *info, void *context) {
        switch (signo) {
            case SIGABRT:
                GetInstance()._handler(
                        AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught abnormal program termination (SIGABRT) signal"),
                        StackTrace(1));
                break;
            case SIGALRM:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught alarm clock (SIGALRM) signal"),
                                       StackTrace(1));
                break;
            case SIGBUS:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught memory access error (SIGBUS) signal"),
                                       StackTrace(1));
                break;
            case SIGFPE:
                GetInstance()._handler(
                        AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught floating point exception (SIGFPE) signal"),
                        StackTrace(1));
                break;
            case SIGHUP:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught hangup instruction (SIGHUP) signal"),
                                       StackTrace(1));
                break;
            case SIGILL:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught illegal instruction (SIGILL) signal"),
                                       StackTrace(1));
                break;
            case SIGINT:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught terminal interrupt (SIGINT) signal"),
                                       StackTrace(1));
                break;
            case SIGPIPE:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught pipe write error (SIGPIPE) signal"),
                                       StackTrace(1));
                break;
            case SIGPROF:
                GetInstance()._handler(
                        AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught profiling timer expired error (SIGPROF) signal"),
                        StackTrace(1));
                break;
            case SIGQUIT:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught terminal quit (SIGQUIT) signal"),
                                       StackTrace(1));
                break;
            case SIGSEGV:
                GetInstance()._handler(
                        AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught illegal storage access error (SIGSEGV) signal"),
                        StackTrace(1));
                break;
            case SIGSYS:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException("Caught bad system call (SIGSYS) signal"),
                                       StackTrace(1));
                break;
            case SIGTERM:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) +
                                       SystemException("Caught termination request (SIGTERM) signal"),
                                       StackTrace(1));
                break;
            case SIGXCPU:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) +
                                       SystemException("Caught CPU time limit exceeded (SIGXCPU) signal"),
                                       StackTrace(1));
                break;
            case SIGXFSZ:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) +
                                       SystemException("Caught file size limit exceeded (SIGXFSZ) signal"),
                                       StackTrace(1));
                break;
            default:
                GetInstance()._handler(AsioUtils::SourceLocation(__FILE__, __LINE__) + SystemException(format("Caught unknown signal - {}", signo)),
                                       StackTrace(1));
                break;
        }
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = SIG_DFL;
        int result = sigaction(signo, &sa, nullptr);
        if (result == 0)
            raise(signo);
        else
            kill(getpid(), SIGKILL);
    }
};

ExceptionsHandler::ExceptionsHandler() {
    [[maybe_unused]] ValidateAlignedStorage<sizeof(Impl), alignof(Impl), StorageSize, StorageAlign> _;
    new (&_storage) Impl();
}

ExceptionsHandler::~ExceptionsHandler() {
    reinterpret_cast<Impl *>(&_storage)->~Impl();
}

void
ExceptionsHandler::SetupHandler(const std::function<void(const SystemException &, const StackTrace &)> &handler) {
    GetInstance().impl().SetupHandler(handler);
}

void ExceptionsHandler::SetupProcess() { GetInstance().impl().SetupProcess(); }
void ExceptionsHandler::SetupThread() { GetInstance().impl().SetupThread(); }

}