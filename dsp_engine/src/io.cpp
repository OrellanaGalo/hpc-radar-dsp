#include "dsp/io.hpp"
#include <fstream>
#include <iostream>

namespace dsp {
    bool IO::write_bin(const std::string& filename, const SignalIQ& signal) {
        std::ofstream file(filename, std::ios::binary | std::ios::out);

        if (!file) {
            std::cerr << "Error al abrir el archivo para escritura: " << filename << "\n";
            return false;
        }

        std::size_t bytes_to_write = signal.size() * sizeof(float);

        file.write(reinterpret_cast<const char*>(signal.i.data()), bytes_to_write);
        file.write(reinterpret_cast<const char*>(signal.q.data()), bytes_to_write);

        file.close();

        return true;
    }
} // namespace dsp