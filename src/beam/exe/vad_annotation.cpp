#include "beam/lib/Pipeline.h"
#include <iostream>

std::string input_file;
std::string output_file;

void exit_with_help() {
	std::cout << "Usage: vad_annotation input_file output_file\n";
	exit(1);
}

void parse_command_line(int argc, char* argv[]) {
	if (argc != 3)
		exit_with_help();
	input_file = argv[1];
	output_file = argv[2];
}

int main(int argc, char* argv[]) {
	parse_command_line(argc, argv);
	Beam::WavReader reader(input_file);
	std::ofstream writer(output_file);
	Beam::MsrVAD vad;
	Beam::Tracker noise_floor(20.0, 0.04, 30000.0, 0.0);
	int channels = 1; // mono input
	int bytes_per_sample = reader.get_bit_per_sample() / 8;
	int buf_size = FRAME_SIZE * channels * bytes_per_sample;
	char* buf = new char[buf_size];
	float input[TWO_FRAME_SIZE] = { 0.f };
	float input_prev[FRAME_SIZE] = { 0.f };
	short* ptr = (short*)buf;
	double time = 0.0;
	while (true) {
		int buf_filled = 0;
		reader.read(buf, buf_size, &buf_filled);
		if (buf_filled < buf_size)
			break;
		// compute energy
		for (int i = 0; i < FRAME_SIZE; ++i) {
			input[i] = input_prev[i];
		}
		for (int i = FRAME_SIZE; i < TWO_FRAME_SIZE; ++i) {
			input[i] = ptr[i - FRAME_SIZE] / (float)SHRT_MAX;
		}
		std::copy(input + FRAME_SIZE, input + TWO_FRAME_SIZE, input_prev);
		float input_fft[TWO_FRAME_SIZE];
		Beam::MCLT::AecCcsFwdMclt(input, input_fft, true);
		float energy = 0.f;
		for (int i = 6; i < 236; ++i){
			energy += input_fft[i] * input_fft[i] + input_fft[TWO_FRAME_SIZE - i] * input_fft[TWO_FRAME_SIZE - i];
		}
		// absolute energy
		if (energy > 5e-3f){
			writer << 1 << ", ";
		}
		else{
			writer << 0 << ", ";
		}
		// simple VAD
		double floor = noise_floor.nextLevel(time, energy);
		if (energy > 5.290792 * floor){
			writer << 1 << ", ";
		}
		else{
			writer << 0 << ", ";
		}
		time += 0.016;
		// SDK1 VAD
		vad.process(input_fft);
		float prob = vad.GetSpeechPresenceProb();
		writer << prob << std::endl;
	}
	delete[] buf;
	std::cout << "annotation done..." << std::endl;
	return 0;
}
