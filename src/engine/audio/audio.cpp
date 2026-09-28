#include "audio.hpp"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

namespace cry {

Audio::~Audio() {
    stop();
}

bool Audio::playAmbient(const std::string& path, bool loop) {
#ifdef _WIN32
    stop();
    const std::wstring wide(path.begin(), path.end());
    DWORD flags = SND_FILENAME | SND_ASYNC;
    if (loop) flags |= SND_LOOP;

    if (!PlaySoundW(wide.c_str(), nullptr, flags)) {
        return false;
    }

    currentPath_ = path;
    return true;
#else
    (void)path;
    (void)loop;
    return false;
#endif
}

void Audio::stop() {
#ifdef _WIN32
    PlaySoundW(nullptr, nullptr, 0);
#endif
    currentPath_.clear();
}

}
