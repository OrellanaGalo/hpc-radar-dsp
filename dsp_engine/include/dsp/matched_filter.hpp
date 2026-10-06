#pragma once
#include "dsp/types.hpp"
#include <fftw3.h>

namespace dsp {
    class MatchedFilter {
        private:
            std::size_t N;

            // buffers persistentes reservados
            SignalIQ rx_work;
            SignalIQ rx_freq;
            SignalIQ tx_freq;
            SignalIQ match_freq;
            SignalIQ output;

            // planes de FFTW persistentes
            fftwf_plan p_rx;
            fftwf_plan p_inv;

        public:
            MatchedFilter(const SignalIQ& tx_pulse, std::size_t listening_window);

            // para limpiar la memoria
            ~MatchedFilter();

            // se desactiva la copia del objeto por seguridad de los punteros
            MatchedFilter(const MatchedFilter&) = delete;
            MatchedFilter& operator=(const MatchedFilter&) = delete;

            const SignalIQ& compress_pulse(const SignalIQ& rx_signal);
    };
} // namespace dsp