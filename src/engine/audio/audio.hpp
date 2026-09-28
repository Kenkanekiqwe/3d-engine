#pragma once
#include <string>

namespace cry {

class Audio {
public:
    Audio() = default;
    ~Audio();

    bool playAmbient(const std::string& path, bool loop = true);
    void stop();

private:
    std::string currentPath_;
};

}
