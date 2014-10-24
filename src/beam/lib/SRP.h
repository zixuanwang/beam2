#ifndef SRP_H_
#define SRP_H_

#include <algorithm>
#include <complex>
#include <vector>
#include "GlobalConfig.h"
#include "KinectConfig.h"

namespace Beam{
	class SRP {
	public:
		SRP();
		~SRP();
		float process(std::vector<std::complex<float> >* input);
		float process_phat(float input[MAX_MICROPHONES][TWO_FRAME_SIZE]);
		float process_ml(std::vector<std::complex<float> >* input, bool voice_found);
	private:
		float m_noise_power[AVALABLE_MICROPHONES][FRAME_SIZE];
		int frame_count;
	};
}

#endif /* SRP_H_ */
