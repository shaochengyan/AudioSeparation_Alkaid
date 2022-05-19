#pragma once
#include <assert.h>
#include <string>
#include <vector>
#include "ColaUtils.h"

// wav文件字段说明
struct wav_header_t {
    char chunkID[4];  // 4 byte: ASCII码表示格式(RIFF)
    unsigned long chunkSize;  // 4 byte 整个文件大小 不包括上面的ID和SIZE本身
    char format[4];               // 4 byte 一般是WAVE
    char subchunk1ID[4];          // 4 格式说明块 一般是 "fmt "
    unsigned long subchunk1Size;  // 本数据块大小
    unsigned short audioFormat;   // 音频格式说明
    unsigned short numChannels;   // 声道数
    unsigned long sampleRate;     // 采样率 ..HZ
    unsigned long byteRate;       // 波特率 f * n = f * 8|16
    unsigned short blockAlign;  // 数据对齐单元 -> 双通道 且 每个采样点占2个byte
                                // 则对其长度为4 byte
    unsigned short
        bitsPerSample;  // 采样点量化长度 8 | 16 bits -> 反映分辨率大小
};

// 数据 块
struct chunk_t {
    char ID[4];  // "data" 的ASCII码值 = 0x61746164  <- d(64) a(61) t(74) a(61)
                 // d在最低位
    unsigned long size;  // 该Chunk的总大小
};

const short int POW2_15 = -((short int)(1 << 15));
const short int POW2_8 = -((short int)(1 << 8));

class AudioUtils {
   private:
    // 音频数据，在存储的时候需要使用
    unsigned long samples_count;
    wav_header_t header;
    chunk_t chunk;

   public:
    bool LoadAudio(const char* filename, std::vector<float>& audioVec);
    bool DumpAudio(const char* filename, const std::vector<float>& audioVec, const short int scale=POW2_8);
    bool DumpAudio(const char* filename, const float* ptr, uint32_t len, const short int scale=POW2_8);
    
};
