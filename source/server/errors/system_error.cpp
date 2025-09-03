

#include "errors/system_error.h"

#include <cerrno>
#include "encoding.h"
#include "format.h"


namespace AsioUtils {

    int SystemError::GetLast() noexcept
    {
        return errno;


    }

    void SystemError::SetLast(int error) noexcept
    {
        errno = error;

    }

    void SystemError::ClearLast() noexcept
    {
        SetLast(0);
    }

    std::string SystemError::Description(int error)
    {
        const int capacity = 1024;
        char buffer[capacity];
        char* result = strerror_r(error, buffer, capacity);
        if (result == nullptr)
            return format("Cannot convert the given system error code to the system message - {}", error);
        else
            return std::string(buffer);

    }

}
