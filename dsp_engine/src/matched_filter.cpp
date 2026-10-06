#include "dsp/matched_filter.hpp"
#include <fftw3.h>
#include <algorithm>

namespace dsp {
    MatchedFilter::MatchedFilter(const SignalIQ& tx_pulse, std::size_t listening_window): 
        N(listening_window),
        rx_work(N),
        rx_freq(N),
        tx_freq(N),
        match_freq(N),
        output(N)

    {
        fftwf_iodim dim[1];
        dim[0].n = static_cast<int>(N);
        dim[0].is = 1;
        dim[0].os = 1;

        // planes permamentes con FFTW_MEASURE para mas velocidad
        p_rx = fftwf_plan_guru_split_dft(
            1, dim, 0, nullptr,
            rx_work.i.data(), rx_work.q.data(),
            rx_freq.i.data(), rx_freq.q.data(),
            FFTW_MEASURE
        );

        p_inv = fftwf_plan_guru_split_dft(
            1, dim, 0, nullptr,
            match_freq.i.data(), match_freq.q.data(),
            output.i.data(), output.q.data(),
            FFTW_MEASURE
        );

        // Pre-calculo del pulso
        SignalIQ tx_work(N);
        std::copy(tx_pulse.i.begin(), tx_pulse.i.end(), tx_work.i.begin());
        std::copy(tx_pulse.q.begin(), tx_pulse.q.end(), tx_work.q.begin());

        fftwf_plan p_tx = fftwf_plan_guru_split_dft(
            1, dim, 0, nullptr,
            tx_work.i.data(), tx_work.q.data(),
            tx_freq.i.data(), tx_freq.q.data(),
            FFTW_ESTIMATE
        );

        fftwf_execute(p_tx);
        fftwf_destroy_plan(p_tx);
    }

    MatchedFilter::~MatchedFilter() {
        fftwf_destroy_plan(p_rx);
        fftwf_destroy_plan(p_inv);
    }

    const SignalIQ& MatchedFilter::compress_pulse(const SignalIQ& rx_signal) {

        std::copy(rx_signal.i.begin(), rx_signal.i.end(), rx_work.i.begin());
        std::copy(rx_signal.q.begin(), rx_signal.q.end(), rx_work.q.begin());

        fftwf_execute(p_rx);

        for (std::size_t i = 0; i < N; ++i) {
            float a_real = rx_freq.i[i];
            float a_imag = rx_freq.q[i];
            float b_real = tx_freq.i[i];
            float b_imag = -tx_freq.q[i]; // conjugado

            match_freq.i[i] = (a_real * b_real) - (a_imag * b_imag);
            match_freq.q[i] = -((a_real * b_imag) + (a_imag * b_real));
        }

        fftwf_execute(p_inv);

        for (std::size_t i = 0; i < N; ++i) {
            output.i[i] /= static_cast<float>(N);
            output.q[i] /= -output.q[i] / static_cast<float>(N); 
        }

        return output;
    }
} // namespace dsp