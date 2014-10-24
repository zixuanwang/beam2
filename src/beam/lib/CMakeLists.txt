SET(libbeam_sources
	Coords.h
	DelaySumBeamformer.h DelaySumBeamformer.cpp
	FFT.h FFT.cpp
	GlobalConfig.h
	KinectConfig.h KinectConfig.cpp
	MicArrayDescriptor.h
	MicArrayWeights.h
	Microphone.h Microphone.cpp
	MsrNS.h MsrNS.cpp
	MsrVAD.h MsrVAD.cpp
	MVDRBeamformer.h MVDRBeamformer.cpp
	Pipeline.h Pipeline.cpp
	SimpleVAD.h SimpleVAD.cpp
	SRP.h SRP.cpp
	WavReader.h WavReader.cpp
	WavWriter.h WavWriter.cpp
)
ADD_LIBRARY(libbeam ${libbeam_sources})

TARGET_LINK_LIBRARIES(libbeam) 

