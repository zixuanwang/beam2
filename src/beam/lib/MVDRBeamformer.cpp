#include "MVDRBeamformer.h"
#include <iostream>

namespace Beam {
MVDRBeamformer::MVDRBeamformer() {
	m_nn.assign(FRAME_SIZE + 1, Eigen::Matrix<std::complex<float>, MAX_MICROPHONES, MAX_MICROPHONES>::Identity());
}

MVDRBeamformer::~MVDRBeamformer() {

}

void MVDRBeamformer::compute(float input[MAX_MICROPHONES][TWO_FRAME_SIZE], float output[TWO_FRAME_SIZE], float angle, bool voice){
	if (!voice) {
		// noise frame, update noise covariance matrix
		for (int bin = 0; bin <= FRAME_SIZE; ++bin) {
			Eigen::Matrix<std::complex<float>, MAX_MICROPHONES, 1> n;
			for (int i = 0; i < MAX_MICROPHONES; ++i) {
				if (bin == 0 || bin == FRAME_SIZE){
					n(i, 0) = std::complex<float>(input[i][bin], 0);
				}
				else{
					n(i, 0) = std::complex<float>(input[i][bin], input[i][TWO_FRAME_SIZE - bin]);
				}
				n(i, 0) *= 100.0; // to make the matrix invertible.
			}
			Eigen::Matrix<std::complex<float>, MAX_MICROPHONES, MAX_MICROPHONES> nn = n * n.adjoint();
			m_nn[bin] = m_nn[bin] * 0.99f + nn * 0.01f;
		}
	}
	// compute time delay
	for (int bin = 0; bin <= FRAME_SIZE; ++bin) {
		Eigen::Matrix<std::complex<float>, MAX_MICROPHONES, MAX_MICROPHONES> inverse;
		std::complex<float> determinant;
		bool invertible;
		m_nn[bin].computeInverseAndDetWithCheck(inverse, determinant, invertible);
		if (invertible) {
			float time_delay[MAX_MICROPHONES] = { 0.f };
			for (int channel = 0; channel < MAX_MICROPHONES; ++channel) {
				float distance = KinectConfig::kinect_descriptor.mic[channel].y
					* sinf(angle);
				time_delay[channel] = distance / (float)SOUND_SPEED;
			}
			Eigen::Matrix<std::complex<float>, MAX_MICROPHONES, 1> d_h;
			float rad_freq = (float)(-bin * TWO_PI * SAMPLE_RATE / FRAME_SIZE
				/ 2.f);
			for (int i = 0; i < MAX_MICROPHONES; ++i) {
				float w = rad_freq * time_delay[i];
				d_h(i, 0) = std::complex<float>(cosf(w), sinf(w));
			}
			Eigen::Matrix<std::complex<float>, 1, MAX_MICROPHONES> d = d_h.adjoint();
			Eigen::Matrix<std::complex<float>, MAX_MICROPHONES, 1> nn_inv_d_h = inverse * d_h;
			Eigen::Matrix<std::complex<float>, 1, 1> denom_mat = d * nn_inv_d_h;
			float denom = denom_mat(0, 0).real();
			std::complex<float> sum(0.f, 0.f);
			for (int channel = 0; channel < MAX_MICROPHONES; ++channel) {
				if (bin == 0 || bin == FRAME_SIZE){
					sum += std::complex<float>(input[channel][bin], 0.f) * nn_inv_d_h(channel, 0);
				}
				else{
					sum += std::complex<float>(input[channel][bin], input[channel][TWO_FRAME_SIZE - bin]) * nn_inv_d_h(channel, 0);
				}
				
			}
			if (bin == 0 || bin == FRAME_SIZE){
				output[bin] = sum.real() / denom;
			}
			else{
				output[bin] = sum.real() / denom;
				output[TWO_FRAME_SIZE - bin] = sum.imag() / denom;
			}
		}
	}
}
}
