

#include "stack_trace.h"


#include <iomanip>


namespace AsioUtils {

    std::ostream &operator<<(std::ostream &os, const StackTrace::Frame &frame) {
        std::ios_base::fmtflags flags = os.flags();
        os << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2 * sizeof(uintptr_t))
           << (uintptr_t) frame.address << ": ";
        os.flags(flags);
        os << (frame.module.empty() ? "<unknown>" : frame.module) << '!';
        os << (frame.function.empty() ? "??" : frame.function) << ' ';
        os << frame.filename;
        if (frame.line > 0)
            os << '(' << frame.line << ')';
        return os;
    }

    StackTrace::StackTrace(int skip) {
       //pending not needed as of now
    }

    std::ostream &operator<<(std::ostream &os, const StackTrace &stack_trace) {
        for (const auto &frame: stack_trace.frames())
            os << frame << std::endl;
        return os;
    }

}