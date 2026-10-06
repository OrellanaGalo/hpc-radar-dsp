#pragma once
#include "dsp/types.hpp"
#include <string>

namespace dsp {
    class IO {
        public:
            static bool write_bin(const std::string& filename, const SignalIQ& signal);
    };
} // namespace dsp