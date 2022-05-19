#pragma once
#define DllExport   __declspec( dllexport )
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <io.h>
#include <string.h>

int64_t DllExport VecProduct(const std::vector<int64_t>& vec);

class DllExport ColaPathUtils {
    char path[512];
    int32_t len = 0;
public:
    ColaPathUtils(const char* _path, const char* _postfix = "") : len(strlen(_path) - strlen(_postfix)) {
        //strcpy(path, _path);
        strcpy_s(path, 512, _path);
        path[len] = 0;
    }
    ColaPathUtils() {};
    const char* Postfix(const char* fix);
    void Update(const char* _path, const char* _postfix);
    void Copy2Wchart(wchar_t* dst);
};



// 用于简单的解析命令行
struct DllExport InputParser {
    // data
    std::vector<const char*> filenames;
    int n_src;
    bool is_noisy;

    // constructor to parser parameters
    InputParser(int argc, char** argv);

    // functions
    size_t _str2int(const char*);
    void ShowOptions();

};



#define COLA_MEASER_PER_TIME(do_something, rep_times)                          \
    {                                                                          \
        unsigned int times = rep_times;                                        \
        std::chrono::steady_clock::time_point begin =                          \
            std::chrono::steady_clock::now();                                  \
        for (int i = 0; i < rep_times; ++i) {                                  \
            do_something                                                       \
        }                                                                      \
        std::chrono::steady_clock::time_point end =                            \
            std::chrono::steady_clock::now();                                  \
        auto time_ms =                                                         \
            std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) \
                .count() /                                                     \
            static_cast<float>(rep_times);                                     \
        std::cout << "Duration = " << time_ms << " (ms)" << std::endl;         \
    }

#define COLA_MEASER_TIME(do_something, str_info)                               \
    {                                                                          \
        std::chrono::steady_clock::time_point begin =                          \
            std::chrono::steady_clock::now();                                  \
        do_something                                                           \
        std::chrono::steady_clock::time_point end =                            \
            std::chrono::steady_clock::now();                                  \
        auto time_ms =                                                         \
            std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) \
                .count();                                                      \
        std::cout << str_info << time_ms << " (ms)" << std::endl;         \
}

#define COLA_RECORD_TIME(do_something, time_var)                               \
    std::chrono::steady_clock::time_point begin =                          \
        std::chrono::steady_clock::now();                                  \
    do_something                                                           \
    std::chrono::steady_clock::time_point end =                            \
        std::chrono::steady_clock::now();                                  \
    auto time_var =                                                         \
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) \
            .count();                                                      \


/*  版本2号，用于指定输出信息(其实只是在前者的基础上添加了一个str_info作为对输出信息进行指示而已)

*/
#define COLA_MEASER_PER_TIME_WITH_INFO(do_something, rep_times, str_info)      \
    {                                                                          \
        unsigned int times = rep_times;                                        \
        std::chrono::steady_clock::time_point begin =                          \
            std::chrono::steady_clock::now();                                  \
        for (int i = 0; i < rep_times; ++i) {                                  \
            do_something                                                       \
        }                                                                      \
        std::chrono::steady_clock::time_point end =                            \
            std::chrono::steady_clock::now();                                  \
        auto time_ms =                                                         \
            std::chrono::duration_cast<std::chrono::milliseconds>(end - begin) \
                .count() /                                                     \
            static_cast<float>(rep_times);                                     \
        std::cout << str_info << time_ms << std::endl;                         \
    }

#define COLA_SHOW_INFO(info1, info2) \
    { std::cout << info1 << info2 << std::endl; }

#define COLA_SHOW_VEC_INFO(c_StrInfo, begin, end)     \
    {                                            \
        std::cout << c_StrInfo << std::endl;     \
        for (auto it = begin; it != end; ++it) { \
            std::cout << *it << '\t';            \
        }                                        \
        putchar('\n');                           \
    }