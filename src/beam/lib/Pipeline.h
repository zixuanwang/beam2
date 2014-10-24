#ifndef PIPELINE_H_
#define PIPELINE_H_

#include "DelaySumBeamformer.h"
#include "FFT.h"
#include "GlobalConfig.h"
#include "MsrNS.h"
#include "MsrVAD.h"
#include "MVDRBeamformer.h"
#include "SimpleVAD.h"
#include "SRP.h"
#include "WavReader.h"
#include "WavWriter.h"

namespace Beam{
	class Pipeline{
	public:
		static Pipeline* instance();
		/// input should have FRAME_SIZE.
		void convert_input(std::vector<std::complex<float> >& input, float* fft_ptr);
		/// fft_ptr should have 2 * FRAME_SIZE.
		void convert_output(const std::vector<std::complex<float> >& output, float* fft_ptr);
		/// process frame.
		void process(float input[MAX_MICROPHONES][FRAME_SIZE], float output[FRAME_SIZE]);
		/// noise suppression.
		void suppress_noise(float* fft_ptr);
		void gain_control(bool voice, float input[FRAME_SIZE]);
		float get_angle(){ return m_angle; }
		float get_gain(){ return m_gain; }
		bool is_voice(){ return m_voice_found; }
		float get_speech_prob(){ return m_vad.GetSpeechPresenceProb(); }
	private:
		// singleton.
		Pipeline();
		Pipeline(Pipeline&);
		Pipeline& operator=(Pipeline&);
		static Pipeline* p_instance;
		SimpleVAD m_simple_vad;
		MVDRBeamformer m_mvdr;
		float m_angle; // sound source angle
		bool m_voice_found; // result of VAD
		bool m_source_found;
		int m_frame_number;
		std::vector<std::complex<float> > m_frequency_input[MAX_MICROPHONES];
		std::vector<std::complex<float> > m_frequency_output;
		// timer. every time preprocess is called, the time is updated.
		double m_time;
		MsrNS m_ns;
		MsrVAD m_vad;
		float m_gain;
		FFT m_fft[MAX_MICROPHONES];
		FFT m_output_fft;
		SRP m_srp;
	};
}

#endif /* PIPELINE_H_ */
