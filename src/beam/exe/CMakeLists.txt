ADD_EXECUTABLE(beamformer beamformer.cpp)
ADD_EXECUTABLE(simple_vad simple_vad.cpp)
ADD_EXECUTABLE(xbox_vad xbox_vad.cpp)

TARGET_LINK_LIBRARIES(beamformer libbeam)
TARGET_LINK_LIBRARIES(simple_vad libbeam)
TARGET_LINK_LIBRARIES(xbox_vad libbeam)
