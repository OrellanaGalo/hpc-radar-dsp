#pragma once
#include <vector>
#include <cstddef>

namespace dsp {

    // Estructura de arrglos SOA para representar una señal IQ
    struct SignalIQ {
        std::vector<float> i; // In-phase
        std::vector<float> q; // Quadrature

        // un constructor helper para reservar memoria contigua rapido
        explicit SignalIQ(std::size_t size) : i(size, 0.0f), q(size, 0.0f) {}

        // para obtener el tamaño de la señal IQ
        [[nodiscard]] std::size_t size() const noexcept {
            return i.size();
        }
    };
}