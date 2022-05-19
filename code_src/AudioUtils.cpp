#include "AudioUtils.h"

bool AudioUtils::LoadAudio(const char* filename, std::vector<float>& audioVec) {
    // 打开文件
    FILE* fin;
    errno_t is_OpenError = fopen_s(&fin, filename, "rb");
    if (is_OpenError != 0) {
        throw "error: Wav file open error.";
    }

    // 读取header
    fread(&header, sizeof(wav_header_t), 1, fin);

    // 对文件时wav格式且频率为16kHz的要求
    if (!strcmp(header.format, "WAVE") || header.sampleRate != 16e3) {
        throw "Audio file must be .wav format and sample rate equal to 16kHz";
    }


    // 读取 chunk 并确定数据块的开始区域
    while (true) {
        fread(&chunk, sizeof(chunk_t), 1, fin);  // 先读取数据块
        // "data"字符串 以及整个数据块的大小
        // printf("%c%c%c%c\t" "%li\n", chunk.ID[0], chunk.ID[1], chunk.ID[2],
        // chunk.ID[3], chunk.size);
        if (*(unsigned int*)&chunk.ID == 0x61746164)
            break;
        // 如果读取到的data.ID != 'data' 则继续尝试读
        fseek(fin, chunk.size, SEEK_CUR);
    }

    int byte_per_sample = 2;  // 默认是16位数据 -> todo可以按照不同情况来修改
    int sample_cnt = chunk.size / byte_per_sample;  // 总共采样点数
    audioVec.resize(sample_cnt);

    std::vector<float>::iterator it = audioVec.begin();
    short int tmp_16bits = 0;  // 存储一个数据点
    for (unsigned int i = 0; i < sample_cnt; ++i, ++it) {
        fread(&tmp_16bits, sizeof(tmp_16bits), 1, fin);
        *it = (tmp_16bits) * -1.0f / POW2_15;
        //printf("%f \t %d\n", *it, tmp_16bits);
    }

    return true;
}

bool AudioUtils::DumpAudio(const char* filename,
                           const std::vector<float>& audioVec, 
                           const short int scale) {
    return this->DumpAudio(filename, audioVec.data(), audioVec.size(), scale);
}

bool AudioUtils::DumpAudio(const char* filename,
                           const float* ptr,
                           uint32_t len, 
                           const short int scale) {
    FILE* fout;
    errno_t isOpenError = fopen_s(&fout, filename, "wb");  // 0 则没有错误

    if (isOpenError) {
        throw "error: Audio file open error when dummping!";
    }
    COLA_SHOW_INFO("Output filename=", filename);

    fwrite(&header, sizeof(header), 1, fout);
    fwrite(&chunk, sizeof(chunk), 1, fout);

    // 写出数据
    short int tmp;
    for (uint32_t idx = 0; idx < len; ++idx) {
        //tmp = ptr[idx] / 100 * -POW2_15;
        tmp = ptr[idx] * scale;  // 由于存在超过1的数据，所以如果乘以1 << 15则会导致溢出
        //printf("%f \t %d\n", ptr[idx], tmp);
        fwrite(&tmp, sizeof(tmp), 1, fout);
    }

    fclose(fout);
    return true;
}
