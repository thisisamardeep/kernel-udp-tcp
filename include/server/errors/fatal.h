
#ifndef ASIOUTILS_ERRORS_FATAL_H
#define ASIOUTILS_ERRORS_FATAL_H

#include "source_location.h"
#include "stack_trace.h"
#include "errors/system_error.h"

#include <string>


namespace AsioUtils {


    void fatal(const SourceLocation& location, const StackTrace& trace, const std::string& message, int error = SystemError::GetLast()) noexcept;

    void fatal(const SourceLocation& location, const StackTrace& trace, const std::exception& fatal) noexcept;


}

#endif
