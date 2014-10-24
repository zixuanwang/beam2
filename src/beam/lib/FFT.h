#ifndef FFT_H_
#define FFT_H_

#include "GlobalConfig.h"

namespace Beam{
	class FFT {
	public:
		FFT();
		~FFT();
		void analyze(float input[FRAME_SIZE], float output[TWO_FRAME_SIZE]);
		void synthesize(float input[TWO_FRAME_SIZE], float output[FRAME_SIZE]);
		/// implementation of fft. copy from aecfft.c
		static void fft(float* xin, float* xout, unsigned int n);
		/// implementation of ifft. copy from aecfft.c
		static void ifft(float* xin, float* xout, unsigned int n);
	private:
		float m_input_prev[FRAME_SIZE];
		float m_output_prev[FRAME_SIZE];
		float m_input[TWO_FRAME_SIZE];
		float m_output[TWO_FRAME_SIZE];
		float m_ha[TWO_FRAME_SIZE];
		// data structures to compute FFT.
		static float wr_15[15];
		static float wi_15[15];
		static void FwdFFT_base15(float* xin, float* xout);
		static void InvFFT_base15(float* xin, float* xout);
	};
}

#endif /* FFT_H_ */