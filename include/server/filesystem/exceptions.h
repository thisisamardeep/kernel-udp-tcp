

#ifndef ASIOUTILS_FILESYSTEM_EXCEPTIONS_H
#define ASIOUTILS_FILESYSTEM_EXCEPTIONS_H

#include "errors/exceptions.h"
#include "filesystem/path.h"

namespace AsioUtils {

    class FileSystemException : public SystemException
    {
    public:
        using SystemException::SystemException;

        const Path& path() const noexcept { return _path; }

        std::string string() const override;

        FileSystemException& Attach(const Path& path)
        { _path = path; return *this; }

        FileSystemException& Attach(const Path& src, const Path& dst)
        { _src = src; _dst = dst; return *this; }

    protected:
        Path _path;
        Path _src;
        Path _dst;
    };

}

#endif
