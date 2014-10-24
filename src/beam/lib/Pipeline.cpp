#include "Pipeline.h"

namespace Beam{
	Pipeline* Pipeline::p_instance = NULL;

	Pipeline::Pipeline() {
		for (int channel = 0; channel < MAX_MICROPHONES; ++channel){
			m_frequency_input[channel].assign(FRAME_SIZE, std::complex<float>(0.f, 0.f));
		}
		m_frequency_output.assign(FRAME_SIZE, std::complex<float>(0.f, 0.f));
		// initialize m_time.
		m_time = 0.0;
		// initialize m_angle.
		m_angle = 0.f;
		// initialize m_voice_found.
		m_voice_found = false;
		// initialize m_frame_number.
		m_frame_number = 0;
		m_gain = 1.f;
	}

	Pipeline* Pipeline::instance(){
		if (p_instance == NULL){
			p_instance = new Pipeline;
		}
		return p_instance;
	}

	void Pipeline::convert_input(std::vector<std::complex<float> >& input, float* fft_ptr){
		input[0].real(fft_ptr[0]);
		input[0].imag(fft_ptr[FRAME_SIZE]);
		for (int i = 1; i < FRAME_SIZE; ++i){
			input[i].real(fft_ptr[i]);
			input[i].imag(fft_ptr[TWO_FRAME_SIZE - i]);
		}
	}

	void Pipeline::convert_output(const std::vector<std::complex<float> >& output, float* fft_ptr){
		fft_ptr[0] = output[0].real();
		fft_ptr[FRAME_SIZE] = output[0].imag();
		for (int i = 1; i < FRAME_SIZE; ++i){
			fft_ptr[i] = output[i].real();
			fft_ptr[TWO_FRAME_SIZE - i] = output[i].imag();
		}
	}

	void Pipeline::process(float input[MAX_MICROPHONES][FRAME_SIZE], float output[FRAME_SIZE]){
		float input_fft[MAX_MICROPHONES][TWO_FRAME_SIZE];
		float output_fft[TWO_FRAME_SIZE];
		for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
			for (int i = 0; i < FRAME_SIZE; ++i){
				input[channel][i] *= m_gain;
			}
			m_fft[channel].analyze(input[channel], input_fft[channel]);
		}
		float energy = input_fft[0][0] * input_fft[0][0] + input_fft[0][FRAME_SIZE] * input_fft[0][FRAME_SIZE];
		for (int i = 1; i < FRAME_SIZE; ++i){
			energy += input_fft[0][i] * input_fft[0][i] + input_fft[0][TWO_FRAME_SIZE - i] * input_fft[0][TWO_FRAME_SIZE - i];
		}
		bool is_voice = m_simple_vad.classify(m_time, energy);
		m_time += (double)FRAME_SIZE / (double)SAMPLE_RATE;
		if (is_voice){
			float srp_angle = m_srp.process_phat(input_fft);
			// smoothing angle
		}
		m_mvdr.compute(input_fft, output_fft, m_angle, m_voice_found);	
		convert_output(m_frequency_output, output_fft);
		suppress_noise(output_fft);
		m_output_fft.synthesize(output_fft, output);
		gain_control(m_voice_found, output);
	}

	void Pipeline::suppress_noise(float* fft_ptr){
		int iSNR = (int)m_vad.GetSNR();
		bool enableNS = true;
		if (iSNR > 130) {
			enableNS = false;
		}
		else if (iSNR < 25) {
			enableNS = true;
		}
		m_vad.process(fft_ptr);
		m_ns.process(fft_ptr, &m_vad, enableNS);
	}

	void Pipeline::gain_control(bool voice, float input[FRAME_SIZE]) {
		float max = *std::max_element(input, input + FRAME_SIZE);
		if (voice){
			if (max > 0.6f){
				m_gain *= 0.95f;
			}
			if (max < 0.001f){
				m_gain *= 1.05f;
			}
		}
	}
}
