#include "SimpleVAD.h"

namespace Beam{
	SimpleVAD::SimpleVAD(float tau_threshold_up, float tau_threshold_down, float level, double time) : m_tau_threshold_up(tau_threshold_up), m_tau_threshold_down(tau_threshold_down), m_level(level), m_time(time){
	}

	SimpleVAD::~SimpleVAD(){

	}

	bool SimpleVAD::classify(double time, float level){
		if (level < m_level){
			m_level += (float)(time - m_time) / m_tau_threshold_down * (level - m_level);
			if (m_level < level){
				m_level = level;
			}
		}
		else{
			m_level += (float)(time - m_time) / m_tau_threshold_up * (level - m_level);
			if (m_level > level){
				m_level = level;
			}
		}
		m_time = time;
		if (level > 5.3f * m_level){
			return true;
		}
		return false;
	}
}



