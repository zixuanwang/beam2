#ifndef MVDRBEAMFORMER_H_
#define MVDRBEAMFORMER_H_

#include <Eigen/Dense>
#include <Eigen/LU>
#include <vector>
#include "GlobalConfig.h"
#include "KinectConfig.h"

namespace Beam{
	class MVDRBeamformer {
	public:
		MVDRBeamformer();
		~MVDRBeamformer();
		void compute(float input[MAX_MICROPHONES][TWO_FRAME_SIZE], float output[TWO_FRAME_SIZE], float angle, bool voice = false);
	private:
		std::vector<Eigen::Matrix<std::complex<float>, MAX_MICROPHONES, MAX_MICROPHONES> > m_nn;
	};
}

#endif /* MVDRBEAMFORMER_H_ */
