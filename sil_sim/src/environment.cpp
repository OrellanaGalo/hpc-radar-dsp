#include "sil/environment.hpp"
#include <cmath>
#include <numbers>
#include <random>
#include <algorithm>

namespace sil {
    dsp::SignalIQ Environment::generate_tx_pulse(
        float pulse_width_s, 
        float bandwidth_hz,
        float sample_rate_hz
    ) 
{
    // calculamos las muestras
    std::size_t num_samples = static_cast<std::size_t>(std::round(pulse_width_s * sample_rate_hz));

    // Reservamos la memoria RAM golpe y la llenamos de ceros
    dsp::SignalIQ signal(num_samples);

    // preparamos los parametros para generar el pulso LFM
    float k = bandwidth_hz / pulse_width_s; // chrip rate (Hz/s)
    float dt = 1.0f / sample_rate_hz; // delta temporal entre muestras
    float half_pulse = pulse_width_s / 2.0f; // para centrar en 0
    const float pi = std::numbers::pi_v<float>;

    // llenmaos rapido la señal con el pulso LFM
    for (std::size_t i = 0; i < num_samples; ++i) {
        float t = i * dt - half_pulse;
        float phase = pi * k * t * t;
        signal.i[i] = std::cos(phase);
        signal.q[i] = std::sin(phase);
    }

    return signal;
}

    dsp::SignalIQ Environment::simulate_target_echo(
        const dsp::SignalIQ& tx_pulse,
        std::size_t total_listening_samples,
        float target_distance_m,
        float sample_rate_hz,
        float noise_amplitude
    ) {
        const float c = 3e8f; // velocidad de la luz en m/s

        dsp::SignalIQ rx_signal(total_listening_samples);

        std::mt19937 gen{std::random_device{}()};
        std::normal_distribution<float> noise_dist(0.0f, noise_amplitude);

        std::size_t start_idx = static_cast<std::size_t>((2.0f * target_distance_m / c) * sample_rate_hz);

        for (std::size_t i = 0; i < tx_pulse.size() && (start_idx + i) < rx_signal.size(); ++i) {
            rx_signal.i[start_idx + i] = tx_pulse.i[i];
            rx_signal.q[start_idx + i] = tx_pulse.q[i];
        }

        for (std::size_t i = 0; i < rx_signal.size(); ++i) {
            rx_signal.i[i] += noise_dist(gen);
            rx_signal.q[i] += noise_dist(gen);
        }

        return rx_signal;
    }

} // namespace sil

