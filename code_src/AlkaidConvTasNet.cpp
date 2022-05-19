#include "AlkaidConvTasNet.h"
#include <time.h>

// constructor
AlkaidConvTasNet::AlkaidConvTasNet()
    : session_options(_GetSessionOptions()),
      sess1(env, L"./model/Alkaid_M1.onnx", session_options),
      sess2(env, L"./model/Alkaid_M2.onnx", session_options),
      mem_info(Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator,
                                          OrtMemType::OrtMemTypeDefault)),
      input_tensor_names{"input"},
      output_tensor_names{"output"} {

          // 其他优化
          // 1 并行存储
      }


AlkaidConvTasNet::AlkaidConvTasNet(wchar_t* model_1, wchar_t* model_2)
    : session_options(_GetSessionOptions()),
    sess1(env, model_1, session_options),
    sess2(env, model_2, session_options),
    mem_info(Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator,
        OrtMemType::OrtMemTypeDefault)),
    input_tensor_names{ "input" },
    output_tensor_names{ "output" } {
}

// Run1
// 跑model1 input:audio_mix_both output: (audio_mix_clean, audio_noise)
// 这里只对数据进行操作，而不产生和返回新的数据
// 默认：
long long AlkaidConvTasNet::RunModel(Ort::Session& sess,
                                std::vector<float>& input_vec,
                                std::vector<float>& output_vec,
                                std::vector<int64_t>& input_tensor_shape,
                                std::vector<int64_t>& output_tensor_shape) {
    // input tensor
    Ort::Value input_tensor = Ort::Value::CreateTensor<float>(
        mem_info, input_vec.data(), input_vec.size(), input_tensor_shape.data(),
        input_tensor_shape.size());
    // output tensor
    output_vec.resize(VecProduct(output_tensor_shape));
    Ort::Value output_tensor = Ort::Value::CreateTensor<float>(
        mem_info, output_vec.data(), output_vec.size(),
        output_tensor_shape.data(), output_tensor_shape.size());
    // run model 1
    COLA_RECORD_TIME(
        sess.Run(Ort::RunOptions(nullptr), input_tensor_names.data(), &input_tensor,
            1, output_tensor_names.data(), &output_tensor, 1);, time_latency);
    return time_latency;  // 时间 long long 毫秒
}

void AlkaidConvTasNet::TestLatencyWithTimelong(int timelong) {
    // 创建模拟音频
    int audio_len = timelong * 16000;
    std::vector<float> input_vec(audio_len);
    for (uint32_t idx = 0; idx < audio_len; ++idx) {
        input_vec[idx] = (rand() % 100) / 50 - 1;
    }
    std::vector<float> output_vec(audio_len);

    // 输入数据
    std::vector<int64_t> input_tensor_shape = { audio_len };
    std::vector<int64_t> output_tensor_shape = { 2, audio_len };

    // 测试模型1
    COLA_MEASER_TIME(
        RunModel(sess1, input_vec, output_vec, input_tensor_shape,
            output_tensor_shape); , "M1 Latency of " << timelong << " sec = ");

    // 测试模型2
    COLA_MEASER_TIME(
        RunModel(sess2, input_vec, output_vec, input_tensor_shape,
            output_tensor_shape);, "M2 Latency of " << timelong << " sec = ")

}


Ort::SessionOptions AlkaidConvTasNet::_GetSessionOptions() {
    Ort::SessionOptions session_options;
    // 不报警告
    session_options.SetLogSeverityLevel(3);

    // 优化1
    session_options.SetGraphOptimizationLevel(
        GraphOptimizationLevel::ORT_ENABLE_ALL);

    // 优化2：并行执行
    // session_options.SetExecutionMode(ExecutionMode::ORT_PARALLEL);
    return session_options;
}

// Run
// 根据不同的type，来进行不同的分支run
/*
    shape在创建音频之后就可以直接创建

    算法前提
    sess1和sess2的模型输入输出接口完全一致！
*/
void AlkaidConvTasNet::Run(const char* filename, int32_t n_src, bool is_noisy) {
    // debug
    printf("=====Processing:%s=====\n", filename);
    // 路径管理
    cpu.Update(filename, ".wav");
    // 存储音频的向量
    std::vector<float> input_vec;
    std::vector<float> output_vec;
    // 加载音频，更新shape -> todo: 放在if外面
    AudioUtils au;
    au.LoadAudio(filename, input_vec);
    int64_t audio_len = input_vec.size();
    // tensor shape
    std::vector<int64_t> input_tensor_shape = {audio_len};
    std::vector<int64_t> output_tensor_shape = {2, audio_len};
    // 存储音频的文件夹
    int ret = _access(cpu.Postfix("/"), 0);
    _mkdir(cpu.Postfix("/"));
    // 分三种情况
    long long latency = 0;
    if (n_src == 1 && is_noisy) {  // (audio_mix_both) -model1->
                                   // (audio_mix_clean, audio_noise)
        latency = RunModel(sess1, input_vec, output_vec, input_tensor_shape,
                 output_tensor_shape);
        printf("M1 Latency = %d (ms)\n", latency);
        au.DumpAudio(cpu.Postfix("/clean.wav"), output_vec.data(), audio_len,
                     -1024);
        au.DumpAudio(cpu.Postfix("/noise.wav"), output_vec.data() + audio_len,
                     audio_len, -4096);
    } else if (n_src == 2 && !is_noisy) {  // (audio_mix_clean/audio_mix_both)
                                           // -model2-> (audio_src1, audio_src2)
        latency = RunModel(sess2, input_vec, output_vec, input_tensor_shape,
                 output_tensor_shape);
        printf("M2 Latency = %d (ms)\n", latency);
        au.DumpAudio(cpu.Postfix("/src1.wav"), output_vec.data(), audio_len,
                     -64);
        au.DumpAudio(cpu.Postfix("/src2.wav"), output_vec.data() + audio_len,
                     audio_len, -64);
    }
#define test_parall
#ifndef test_parall
    else if (n_src == 2 && is_noisy) {  // 上面两个步骤合起来！
        // model1分离
        std::thread thread_sep_noise([&]() {
            RunModel(sess1, input_vec, output_vec, input_tensor_shape,
                     output_tensor_shape);
            // au.DumpAudio(cpu.Postfix("/clean.wav"), output_vec.data(),
            //              audio_len, -512);
            au.DumpAudio(cpu.Postfix("/noise.wav"),
                         output_vec.data() + audio_len, audio_len, -4096);
        });
        // model2 分离
        std::vector<float> output_vec2;
        std::thread thread_sep_clean([&]() {
            RunModel(sess2, input_vec, output_vec2, input_tensor_shape,
                     output_tensor_shape);
            au.DumpAudio(cpu.Postfix("/src1.wav"), output_vec2.data(),
                         audio_len, -128);
            au.DumpAudio(cpu.Postfix("/src2.wav"),
                         output_vec2.data() + audio_len, audio_len, -128);
        });

        // join
        thread_sep_noise.join();
        thread_sep_clean.join();
        return;
    }
#else
    else if (n_src == 2 && is_noisy) {  // 上面两个步骤合起来！
        // model1分离
        latency = RunModel(sess1, input_vec, output_vec, input_tensor_shape,
                 output_tensor_shape);
        printf("M1 Latency = %d (ms)\n", latency);
        au.DumpAudio(cpu.Postfix("/noise.wav"), output_vec.data() + audio_len,
                     audio_len, -4096);  // 存储噪声
        output_vec.resize(audio_len);    // 仅保留mix_clean
        for(auto it = output_vec.begin(); it != output_vec.end(); ++it) {
            *it /= 128;
        }
        // model2 分离
        std::vector<float>
            output_vec2;  // cola_todo: 可以直接使用input_vec来替代
        latency = RunModel(sess2, output_vec, output_vec2, input_tensor_shape,
                 output_tensor_shape);
        printf("M2 Latency = %d (ms)\n", latency);
        au.DumpAudio(cpu.Postfix("/src1.wav"), output_vec2.data(), audio_len,
                     -64);
        au.DumpAudio(cpu.Postfix("/src2.wav"), output_vec2.data() + audio_len,
                     audio_len, -64);
    }
#endif  // !test_parall

    printf("=====End=====\n");
}