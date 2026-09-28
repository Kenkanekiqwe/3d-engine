#include "audio.hpp"
#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#endif
namespace cry {
Audio::~Audio(){stop();}
bool Audio::playAmbient(const std::string&path,bool loop){
#ifdef _WIN32
 stop();std::wstring wide(path.begin(),path.end());std::wstring cmd=L"open \""+wide+L"\" type waveaudio alias cryambient";
 if(mciSendStringW(cmd.c_str(),nullptr,0,nullptr)!=0)return false;
 mciSendStringW(L"setaudio cryambient volume to 220",nullptr,0,nullptr);
 mciSendStringW(loop?L"play cryambient repeat":L"play cryambient",nullptr,0,nullptr);currentPath_=path;return true;
#else
 (void)path;(void)loop;return false;
#endif
}
void Audio::stop(){
#ifdef _WIN32
 mciSendStringW(L"stop cryambient",nullptr,0,nullptr);mciSendStringW(L"close cryambient",nullptr,0,nullptr);
#endif
 currentPath_.clear();
}
}