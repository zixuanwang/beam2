#include "SRP.h"


namespace Beam{
	SRP::SRP(){
		for (int i = 0; i < AVALABLE_MICROPHONES; ++i){
			std::fill(m_noise_power[i], m_noise_power[i] + FRAME_SIZE, 0.f);
		}	
		frame_count = 0;
	}
	SRP::~SRP(){
	
	}

	float SRP::process(std::vector<std::complex<float> >* input){
		float max_angle = 0;
		float max_power = FLT_MIN;
		for (float angle = -90.f; angle <= 90.f; angle += 10.f){
			// compute time delay
			float time_delay[AVALABLE_MICROPHONES] = { 0.f };
			for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
				float distance = KinectConfig::kinect_descriptor.mic[channel].y * sinf(angle);
				time_delay[channel] = distance / (float)SOUND_SPEED;
			}
			float power = 0.f;
			for (int bin = 6; bin < 236; ++bin){
				float rad_freq = (float)(-bin * TWO_PI * SAMPLE_RATE / FRAME_SIZE / 2.f);
				std::complex<float> sum(0.f, 0.f);
				for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
					float v = (float)(rad_freq * time_delay[channel]);
					sum += input[channel][bin] * std::complex<float>(cosf(v), sinf(v));
				}
				power += std::norm(sum);
			}
			if (power > max_power){
				max_power = power;
				max_angle = angle;
			}
		}
		return max_angle;
	}
	float SRP::process_phat(float input[MAX_MICROPHONES][TWO_FRAME_SIZE]){
		float max_angle = 0;
		float max_power = FLT_MIN;
		for (float angle = -90.f; angle <= 90.f; angle += 10.f){
			// compute time delay
			float time_delay[AVALABLE_MICROPHONES] = { 0.f };
			for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
				float distance = KinectConfig::kinect_descriptor.mic[channel].y * sinf(angle);
				time_delay[channel] = distance / (float)SOUND_SPEED;
			}
			float power = 0.f;
			for (int bin = 6; bin < 236; ++bin){
				float rad_freq = (float)(-bin * TWO_PI * SAMPLE_RATE / FRAME_SIZE / 2.f);
				std::complex<float> sum(0.f, 0.f);
				for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
					float v = (float)(rad_freq * time_delay[channel]);
					std::complex<float> cinput(input[channel][bin], input[channel][TWO_FRAME_SIZE - bin]);
					sum += cinput * std::complex<float>(cosf(v), sinf(v)) / std::abs(cinput);
				}
				power += std::norm(sum);
			}
			if (power > max_power){
				max_power = power;
				max_angle = angle;
			}
		}
		return max_angle;
	}

	float SRP::process_ml(std::vector<std::complex<float> >* input, bool voice_found){
		float gamma = 0.1f;
		if (!voice_found){
			for (int i = 0; i < FRAME_SIZE; ++i){
				for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
					if (frame_count == 0){
						m_noise_power[channel][i] = std::norm(input[channel][i]);
					}
					else{
						m_noise_power[channel][i] = m_noise_power[channel][i] * 0.9f + std::norm(input[channel][i]) * 0.1f;
					}
				}
			}
		}
		float k[AVALABLE_MICROPHONES][FRAME_SIZE] = { 0.f };
		for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
			for (int i = 0; i < FRAME_SIZE; ++i){
				k[channel][i] = gamma * std::norm(input[channel][i]) + (1.f - gamma) * m_noise_power[channel][i];
			}
		}
		float max_angle = 0;
		float max_power = FLT_MIN;
		for (float angle = -90.f; angle <= 90.f; angle += 10.f){
			// compute time delay
			float time_delay[AVALABLE_MICROPHONES] = { 0.f };
			for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
				float distance = KinectConfig::kinect_descriptor.mic[channel].y * sinf(angle);
				time_delay[channel] = distance / (float)SOUND_SPEED;
			}
			float power = 0.f;
			for (int bin = 6; bin < 236; ++bin){
				float k_inv = 0.f;
				float rad_freq = (float)(-bin * TWO_PI * SAMPLE_RATE / FRAME_SIZE / 2.f);
				std::complex<float> sum(0.f, 0.f);
				for (int channel = 0; channel < AVALABLE_MICROPHONES; ++channel){
					float v = (float)(rad_freq * time_delay[channel]);
					sum += input[channel][bin] * std::complex<float>(cosf(v), sinf(v)) / k[channel][bin];
					k_inv += 1.f / k[channel][bin];
				}
				power += std::norm(sum) / k_inv;
			}
			if (power > max_power){
				max_power = power;
				max_angle = angle;
			}
		}
		++frame_count;
		return max_angle;
	}
}