#include "ColaUtils.h"


int64_t VecProduct(const std::vector<int64_t>& vec) {
    if (vec.size() == 0)
        return 0;
    int64_t len = 1;
    std::vector<int64_t>::const_iterator iter = vec.begin();
    std::vector<int64_t>::const_iterator iter_end = vec.end();
    while (iter != iter_end) {
        len *= (*iter++);
    }
    return len;
}

const char* ColaPathUtils::Postfix(const char* fix) {
    strcpy_s(path + len, 512-len, fix);
    return path;
}
void ColaPathUtils::Update(const char* _path, const char* _postfix = ".wav") {
    len = strlen(_path) - strlen(_postfix);
    strcpy_s(path, 512, _path);
    path[len] = 0;
}

void ColaPathUtils::Copy2Wchart(wchar_t* dst)
{
    char* src = path;
    int len = strlen(path);
    for (int idx = 0; idx < len; ++idx) {
        dst[idx] = path[idx];
    }
    dst[len] = L'\0';
}

std::wstring String2WString(const std::string& s) {
    return std::wstring(s.begin(), s.end());
}

void InputParser::ShowOptions() {
    printf("===== Options =====\n");
    printf("n_src=%d\nis_noisy=%d\n", this->n_src, this->is_noisy);
    for (int idx = 0; idx < filenames.size(); ++idx) {
        printf("%s\n", filenames[idx]);
    }
    printf("===== End =====\n");

}

InputParser::InputParser(int argc, char** argv){
    if (argc < 4) {
        perror("Input parameter formate: n_src is_noisy wav_path1 wav_path2 ...");
    }
    n_src = this->_str2int(argv[1]);
    is_noisy = this->_str2int(argv[2]);
    // ½âÎöÃû×Ö
    int file_num = argc - 3;
    this->filenames.resize(file_num);
    memcpy(filenames.data(), argv + 3, file_num * sizeof(char *));
}

size_t InputParser::_str2int(const char* s) {
    size_t num = 0, idx = 0, tmp;
    while (s[idx]) {
        tmp = (s[idx] - '0');
        num = num * 10 + tmp;
        idx++;
    }
    return num;
}
