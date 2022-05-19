import soundfile as sf
import onnxruntime as ort
import numpy as np
import os
import shutil
from pystoi import stoi


class AlkaidConvTasNet:
    def __init__(self):
        self.sess1 = ort.InferenceSession("../models/Alkaid_M1.onnx")
        self.sess2 = ort.InferenceSession("../models/Alkaid_M2.onnx")
        self.sample_rate = int(16e3);
        
    # 推理相关函数
    def load_audio(self, audio_path):
        wav, sample_rate = sf.read(audio_path)
        assert( sample_rate == self.sample_rate )
        return np.asarray(wav, dtype=np.float32)
    
    def dump_audio(self, audio_path, audio):
        print(audio)
        sf.write(audio_path, audio, self.sample_rate)
    
    def run(self, audio_path: str, n_src: int, is_noisy: bool):
        dir_targets = os.path.splitext(audio_path)[0]
        if not os.path.exists(dir_targets):
            os.mkdir(dir_targets)
        audio_array = self.load_audio(audio_path)
        if n_src == 1 and is_noisy:
            output_array1 = self._run_session(self.sess1, audio_array) * 0.05 # 2 x audio_len
            self.dump_audio(dir_targets + "/noise.wav", output_array1[1])
            self.dump_audio(dir_targets + "/clean.wav", output_array1[0])
        elif n_src == 2 and (not is_noisy):
            output_array2 = self._run_session(self.sess2, audio_array) * 0.005
            self.dump_audio(dir_targets + "/src1.wav", output_array2[0])
            self.dump_audio(dir_targets + "/src2.wav", output_array2[1])
        elif n_src == 2 and is_noisy:
            output_array1 = self._run_session(self.sess1, audio_array) * 0.05 # 2 x audio_len
            self.dump_audio(dir_targets + "/noise.wav", output_array1[1])
            output_array2 = self._run_session(self.sess2, output_array1[0]) * 0.005
            self.dump_audio(dir_targets + "/src1.wav", output_array2[0])
            self.dump_audio(dir_targets + "/src2.wav", output_array2[1])
        
    def _run_session(self, sess, input_array):
        output_array = sess.run(
            None, 
            input_feed={"input": input_array}
        )[0]
        return output_array
        
        
alkaid = AlkaidConvTasNet()
alkaid.run("../data/mix_single.wav", 1, 1)  # singla vocal with noise
alkaid.run("../data/mix_clean.wav", 2, 0)  # two vocal without noise
alkaid.run("../data/mix_both.wav", 2, 1)  # two vocal with noise