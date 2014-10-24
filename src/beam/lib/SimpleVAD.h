#ifndef SIMPLEVAD_H_
#define SIMPLEVAD_H_

namespace Beam{
	class SimpleVAD {
	public:
		SimpleVAD(float tau_threshold_up = 20.f, float tau_threshold_down = 0.04f, float level = 0.f, double time = 0.0);
		~SimpleVAD();
		// if voice is detected, return true. otherwise, return false.
		bool classify(double time, float level);
	private:
		float m_tau_threshold_up;
		float m_tau_threshold_down;
		float m_level;
		double m_time;
	};
}

#endif /* SIMPLEVAD_H_ */
