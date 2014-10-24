import os
import sys
input_path = '\\\\phx.gbl\\public\\IPELabs\\AudioRecoPlatform\\Data\\iSSData\\final_092514\\final\\meetings'
folders = os.listdir(input_path)
print(folders)
label_folder = 'labels_acoustic_singletag'
wav_folder = 'wav16000'
vad_folder = sys.argv[1]
output_path = 'E:\\data\\audio\\lync\\meetings'
for folder in folders:
	input_folder = os.path.join(input_path, folder, wav_folder)
	output_folder = os.path.join(output_path, folder, vad_folder)
	try:
		os.makedirs(output_folder)
	except OSError as exc:
		pass
	wav_files = os.listdir(input_folder)
	for wav_file in wav_files:
		input_file = os.path.join(input_folder, wav_file)
		txt_file = wav_file.replace('.wav', '.txt')
		output_file = os.path.join(output_folder, txt_file)
		cmd = 'C:\\Users\\danwa\\Documents\\Github\\beam2\\bin\\Release\\' + sys.argv[1] + '.exe ' + input_file + ' ' + output_file
		print(cmd)
		os.system(cmd)

	