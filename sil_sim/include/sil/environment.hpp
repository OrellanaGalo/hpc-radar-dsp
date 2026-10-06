#pragma once
#include "dsp/types.hpp"

namespace sil {
    class Environment {
        public:
            // genera el pulso transmitido (chirp lfm - linear frequency modulation)
            static dsp::SignalIQ generate_tx_pulse(
                float pulse_width_s, // Duracion del pulso en segundos
                float bandwidth_hz, // Andho de banda
                float sample_rate_hz // Frecuencia de muestreo
            );

            // simula el rebote en un objetivo (retardo, doppler y ruido gaussiano)
            static dsp::SignalIQ simulate_target_echo(
                const dsp::SignalIQ& tx_pulse,
                std::size_t total_listening_samples,
                float target_distance_m,
                float sample_rate_hz,
                float noise_amplitude
            );
    };
}