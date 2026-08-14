#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>

struct AEffect;
using AudioMasterCallback = intptr_t (__cdecl*)(AEffect*, int32_t, int32_t, intptr_t, void*, float);
using DispatcherProc = intptr_t (__cdecl*)(AEffect*, int32_t, int32_t, intptr_t, void*, float);
using ProcessProc = void (__cdecl*)(AEffect*, float**, float**, int32_t);

struct AEffect
{
    int32_t magic;
    DispatcherProc dispatcher;
    ProcessProc process;
    void* setParameter;
    void* getParameter;
    int32_t numPrograms, numParams, numInputs, numOutputs;
    int32_t flags;
    void* reserved1;
    void* reserved2;
    int32_t initialDelay;
    int32_t realQualities;
    int32_t offQualities;
    float ioRatio;
    void* object;
    void* user;
    int32_t uniqueID;
    int32_t version;
    ProcessProc processReplacing;
    void* processDoubleReplacing;
    char future[56];
};

static intptr_t __cdecl audioMaster(AEffect*, int32_t opcode, int32_t, intptr_t, void*, float)
{
    // audioMasterVersion
    return opcode == 1 ? 2400 : 0;
}

static void writeResult(const std::filesystem::path& path, const std::string& text)
{
    std::ofstream out(path, std::ios::binary);
    out << text;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR commandLine, int)
{
    const std::filesystem::path dllPath(commandLine);
    const auto resultPath = std::filesystem::temp_directory_path() / "ucg-hypersonic-probe.txt";
    if (dllPath.empty() || !std::filesystem::exists(dllPath))
    {
        writeResult(resultPath, "ERROR: Hypersonic.dll not found");
        return 2;
    }

    const auto module = LoadLibraryW(dllPath.c_str());
    if (module == nullptr)
    {
        writeResult(resultPath, "ERROR: LoadLibrary failed " + std::to_string(GetLastError()));
        return 3;
    }

    using MainProc = AEffect* (__cdecl*)(AudioMasterCallback);
    auto mainProc = reinterpret_cast<MainProc>(GetProcAddress(module, "VSTPluginMain"));
    if (mainProc == nullptr) mainProc = reinterpret_cast<MainProc>(GetProcAddress(module, "main"));
    if (mainProc == nullptr)
    {
        writeResult(resultPath, "ERROR: VST2 entry point not found");
        FreeLibrary(module);
        return 4;
    }

    AEffect* effect = mainProc(audioMaster);
    if (effect == nullptr || effect->magic != 0x56737450 || effect->dispatcher == nullptr)
    {
        writeResult(resultPath, "ERROR: invalid AEffect returned");
        FreeLibrary(module);
        return 5;
    }

    // effOpen, effSetSampleRate, effSetBlockSize, effMainsChanged
    effect->dispatcher(effect, 0, 0, 0, nullptr, 0.0f);
    effect->dispatcher(effect, 10, 0, 0, nullptr, 44100.0f);
    effect->dispatcher(effect, 11, 0, 512, nullptr, 0.0f);
    effect->dispatcher(effect, 12, 0, 1, nullptr, 0.0f);

    writeResult(resultPath,
                "OK: Hypersonic engine opened; outputs=" + std::to_string(effect->numOutputs)
                + "; programs=" + std::to_string(effect->numPrograms)
                + "; id=" + std::to_string(effect->uniqueID));

    effect->dispatcher(effect, 12, 0, 0, nullptr, 0.0f);
    effect->dispatcher(effect, 1, 0, 0, nullptr, 0.0f);
    FreeLibrary(module);
    return 0;
}
