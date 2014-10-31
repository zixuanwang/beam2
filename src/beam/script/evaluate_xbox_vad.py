#!/usr/bin/python

import os
import sys

def read_groundtruth(file_path):
    fid = open(file_path)
    voice = []
    for line in fid:
        l = line.split('\t')
        voice.append([float(l[0]), float(l[1])])
    return voice

def read_label(file_path):
    fid = open(file_path)
    voice = []
    for line in fid:
        l = line.split('\t')
        voice.append([float(l[0]), float(l[1]), float(l[2])])
    return voice
        
def compare(groundtruth, result):
    threshold = 0.00001
    true_positive = 0
    true_negative = 0
    false_negative = 0
    false_positive = 0
    denom = 0
    groundtruth_len = len(groundtruth)
    groundtruth_range = range(0, groundtruth_len)
    for r in result:
        start = r[0]
        end = r[1]
        label = r[2]
        speech = 0
        for i in groundtruth_range:
            if start >= groundtruth[i][0] and end < groundtruth[i][1]:
                speech = 1
                break
            if start >= groundtruth[i][0] and start < groundtruth[i][1] and end > groundtruth[i][1]:
                speech = 2
                break
            if start < groundtruth[i][0] and end >= groundtruth[i][1] and end < groundtruth[i][1]:
                speech = 2
                break
        if speech == 0 or speech == 1:
            denom = denom + 1
        if speech == 0:
            if label < threshold:
                true_negative = true_negative + 1
            else:
                false_positive = false_positive + 1
        if speech == 1:
            if label < threshold:
                false_negative = false_negative + 1
            else:
                true_positive = true_positive + 1
    return (float(true_positive + true_negative) / denom, float(false_positive) / denom, float(false_negative) / denom)

input_path = '/Volumes/My Passport/data/audio/lync/meetings'
folders = os.listdir(input_path)
label_folder = 'labels_acoustic_singletag'
vad_folder = 'xbox_vad'
device_list = ['Dell_Laptop', 'Headset_Microsoft', 'Headset_Plantronics', 'Kinect_V1', 'Tablet_Surface_Pro', 
'Main_Laptop_Mic', 'Webcam_LifeCam_5000', 'Phone_Htc_8x', 'Webcam_LifeCam_Studio', 'Phone_Lumia_1520', 'Webcam_Logitech_C920']

count = 0
device_map = dict()
for device in device_list:
    device_map[device] = dict()
    device_map[device]['accuracy'] = 0
    device_map[device]['false_positive'] = 0
    device_map[device]['false_negative'] = 0

for folder in folders:
    groundtruth_folder = os.path.join(input_path, folder, label_folder)
    vad_folder = os.path.join(input_path, folder, vad_folder)
    count = count + 1
    for device in device_list:
        groundtruth_path = groundtruth_folder + '/' + device + '.txt'
        vad_path = vad_folder + '/' + device + '.txt'
        groundtruth = read_groundtruth(groundtruth_path)
        vad_result = read_label(vad_path)
        accuracy, false_positive, false_negative = compare(groundtruth, vad_result)
        device_map[device]['accuracy'] += accuracy
        device_map[device]['false_positive'] += false_positive
        device_map[device]['false_negative'] += false_negative
        print(folder)
        print(device)

for device in device_list:
    print(device)
    print('Accuracy: %f'%(device_map[device]['accuracy'] / float(count)))
    print('False Positive %f'%(device_map[device]['false_positive'] / float(count)))
    print('False Negative: %f'%(device_map[device]['false_negative'] / float(count)))
        
        