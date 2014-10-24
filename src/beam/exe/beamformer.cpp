#include "beam/lib/Pipeline.h"
#include <iostream>

std::string input_file;
std::string output_file;

void exit_with_help() {
	std::cout << "Usage: beamformer input_file output_file\n";
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
	int channels = reader.get_channels();
	int bytes_per_sample = reader.get_bit_per_sample() / 8;
	Beam::WavWriter writer(output_file, 16000, 1, 16);
	int buf_size = FRAME_SIZE * channels * bytes_per_sample;
	int output_buf_size = TWO_FRAME_SIZE;
	char* buf = new char[buf_size];
	char* output_buf = new char[output_buf_size];
	short* output_ptr = (short*) output_buf;
	float input[MAX_MICROPHONES][FRAME_SIZE] = { 0.f };
	float output[FRAME_SIZE] = { 0.f };
	Beam::FFT fft;
	while (true) {
		int buf_filled = 0;
		reader.read(buf, buf_size, &buf_filled);
		if (buf_filled < buf_size)
			break;
		reader.convert_format(input, buf, buf_size);
		// this is the key step in the beamformer.
		// input are 4 channels. each channel contains 256 float numbers.
		// output is 1 channel. it contains 256 float numbers.
		Beam::Pipeline::instance()->process(input, output); 
		for (int i = 0; i < FRAME_SIZE; ++i) {
			output_ptr[i] = (short) (output[i] * SHRT_MAX);
		}
		writer.write(output_buf, TWO_FRAME_SIZE);
	}
	delete[] buf;
	delete[] output_buf;
	return 0;
}
