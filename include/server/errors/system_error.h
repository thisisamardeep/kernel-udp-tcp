
#ifndef ASIOUTILS_ERRORS_SYSTEM_ERROR_H
#define ASIOUTILS_ERRORS_SYSTEM_ERROR_H

#include <string>

namespace AsioUtils {


    class SystemError
    {
    public:
        SystemError() = delete;
        SystemError(const SystemError&) = delete;
        SystemError(SystemError&&) = delete;
        ~SystemError() = delete;

        SystemError& operator=(const SystemError&) = delete;
        SystemError& operator=(SystemError&&) = delete;


        static int GetLast() noexcept;


        static void SetLast(int error) noexcept;

        static void ClearLast() noexcept;


        static std::string Description() { return Description(GetLast()); }

        static std::string Description(int error);
    };


}

#endif
