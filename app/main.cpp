#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <memory>

#include "sil/environment.hpp"
#include "dsp/matched_filter.hpp"
#include "dsp/io.hpp"

void process_batch(
    std::size_t core_id,
    std::size_t total_cores,
    std::size_t total_pulses,
    dsp::MatchedFilter* filter,
    const std::vector<dsp::SignalIQ>& rx_burst,
    std::vector<dsp::SignalIQ>& output_burst
) {
    for (std::size_t i = core_id; i < total_pulses; i += total_cores) {
        output_burst[i] = filter->compress_pulse(rx_burst[i]);
    }
}

int main () {
    // CONFIGURACION FISICA
    std::cout << "=====================================\n";
    std::cout << "   MOTOR DSP DE RADAR (HPC)    \n";
    std::cout << "=====================================\n\n";

    float pulse_width_s = 10e-6f; // 10 microseconds
    float bandwidth_hz = 5e6f; // 5 MHz
    float sample_rate_hz = 20e6f; // 20 Msps
    std::size_t listening_window = 2000;

    std::size_t num_pulses = 100; // rafaga de 100 ecos
    float target_distance = 300.0f;
    float noise_amplitud = 0.5f;

    std::cout << "[1] Generando pulso de transmision...\n";
    auto tx_pulse = sil::Environment::generate_tx_pulse(pulse_width_s, bandwidth_hz, sample_rate_hz);

    // INICIALIZACION HPC 
    unsigned int num_cores = std::thread::hardware_concurrency();
    if (num_cores == 0) num_cores = 4;

    std::cout << "[2] Inicializando " << num_cores << " motores DSP paralelos (FFTW_MEASURE)...\n";

    std::vector<std::unique_ptr<dsp::MatchedFilter>> filter_pool;
    for (unsigned int i = 0; i < num_cores; ++i) {
        filter_pool.push_back(std::make_unique<dsp::MatchedFilter>(tx_pulse, listening_window));
    }

    // SIMULACION
    std::cout << "[3] Simulando rafaga de " << num_pulses << " ecos de radar con ruido...\n";
    std::vector<dsp::SignalIQ> rx_burst;
    rx_burst.reserve(num_pulses);

    for (std::size_t i = 0; i < num_pulses; ++i) {
        rx_burst.push_back(sil::Environment::simulate_target_echo(
            tx_pulse, listening_window, target_distance, sample_rate_hz, noise_amplitud
        ));
    }

    std::vector<dsp::SignalIQ> output_burst(num_pulses, dsp::SignalIQ(listening_window));

    // PROCESAMIENTO PARALIZADO
    std::cout << "[4] Procesando senial...\n";
    auto start_time = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;

    for (unsigned int core_id = 0; core_id < num_cores; ++core_id) {
        threads.emplace_back(
            process_batch,
            core_id,
            num_cores,
            num_pulses,
            filter_pool[core_id].get(),
            std::ref(rx_burst),
            std::ref(output_burst)
        );
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

    // REPORTES E I/O BINARIO
    std::cout << "-------------------------------------\n";
    std::cout << "Tiempo total de la rafaga (" << num_pulses << " pulsos): " << total_duration.count() << "us\n";
    std::cout << "Velocidad promedio: " << total_duration.count() / num_pulses << "us por pulso!\n";

    float expected_delay_s = (2.0f * target_distance) / 3e8f;
    std::size_t expected_index = static_cast<std::size_t>(expected_delay_s * sample_rate_hz);
    std::cout << "El pulso [0] ubica al objetivo teorico en la muestra: " << expected_index << "\n";

    if (dsp::IO::write_bin("radar_output_0.bin", output_burst[0])) {
        std::cout << "Pulso [0] exportado a radar_output_0.bin\n";
    }
    std::cout << "=====================================\n";

    return 0;
}