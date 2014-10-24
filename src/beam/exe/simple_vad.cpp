#include "beam/lib/Pipeline.h"
#include "beam/lib/SimpleVAD.h"
#include <iostream>

std::string input_file;
std::string output_file;

void exit_with_help() {
	std::cout << "Usage: simple_vad input_file output_file\n";
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
	std::ofstream out_stream(output_file);
	int channels = reader.get_channels();
	int bytes_per_sample = reader.get_bit_per_sample() / 8;
	int buf_size = FRAME_SIZE * channels * bytes_per_sample;
	char *buf = new char[buf_size];
	short *buf_ptr = (short*)buf;
	float input[FRAME_SIZE] = { 0.f };
	float fft_input[TWO_FRAME_SIZE] = { 0.f };
	Beam::FFT fft;
	Beam::SimpleVAD vad;
	double time = 0.0;
	while (true) {
		int buf_filled = 0;
		reader.read(buf, buf_size, &buf_filled);
		if (buf_filled < buf_size)
			break;
		for (int i = 0; i < FRAME_SIZE; ++i){
			input[i] = (float)(buf_ptr[i]) / SHRT_MAX;
		}
		fft.analyze(input, fft_input);
		float energy = 0.f;
		for (int i = 6; i < TWO_FRAME_SIZE - 5; ++i){
			energy += fft_input[i] * fft_input[i];
		}
		bool voice = vad.classify(time, energy);
		out_stream << time << "\t" << time + (double)FRAME_SIZE / (double)SAMPLE_RATE << "\t" << voice << std::endl;
		time += (double)FRAME_SIZE / (double)SAMPLE_RATE;
	}
	delete[] buf;
	return 0;
}
