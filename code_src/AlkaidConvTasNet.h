#pragma once
#include <iostream>
#include <onnxruntime_cxx_api.h>
#include "AudioUtils.h"
#include "ColaUtils.h"
#include <direct.h>
#include <io.h>
#include <thread>


class AlkaidConvTasNet {
private:
    // 环境
    Ort::Env env;
    Ort::SessionOptions session_options;
    // 推理会话
    Ort::Session sess1, sess2;
    // 模型参数 两个模型共用
    std::vector<const char*> input_tensor_names;
    std::vector<const char*> output_tensor_names;
    // 需要使用的ort
    Ort::MemoryInfo mem_info;
    // 文件path管理器
    ColaPathUtils cpu;
public:
    AlkaidConvTasNet();
    AlkaidConvTasNet(wchar_t* model_1, wchar_t* model_2);
    void Run(const char* filename, int32_t n_src, bool is_noisy);
    long long RunModel(
        Ort::Session& sess,
        std::vector<float>& input_vec,
        std::vector<float>& output_vec,
        std::vector<int64_t>& input_tensor_shape,
        std::vector<int64_t>& output_tensor_shape);
    Ort::SessionOptions _GetSessionOptions();
    void TestLatencyWithTimelong(int timelong);

};