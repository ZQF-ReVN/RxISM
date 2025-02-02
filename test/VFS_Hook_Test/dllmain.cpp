#include <Windows.h>
#include <memory>
#include <string>
#include <ReVN/RxISM/Hook/VFS_Patch.h>

namespace RxISM { using namespace ZQF::ReVN::RxISM; }


static auto GetModuleFileDir(const HMODULE hModule) -> std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>
{
    std::uint32_t written_chars{};
    std::uint32_t buffer_max_chars = MAX_PATH;
    std::unique_ptr<wchar_t[]> buffer;
    do
    {
        buffer_max_chars *= 2;
        buffer = std::make_unique_for_overwrite<wchar_t[]>(buffer_max_chars);
        written_chars = ::GetModuleFileNameW(hModule, buffer.get(), buffer_max_chars);
    } while (written_chars >= buffer_max_chars);

    std::wstring_view path_str{ buffer.get() , written_chars };
    const auto pos = path_str.rfind(L'\\');
    if (pos != std::wstring_view::npos)
    {
        path_str = path_str.substr(0, pos + 1);
    }
    else
    {
        path_str = L"";
    }

    return { path_str, std::move(buffer) };
}

static auto StartHook(const HMODULE hDllBase) -> void
{
    const auto dll_dir = ::GetModuleFileDir(hDllBase);
    RxISM::VFSPatch::Install(std::wstring{ dll_dir.first }.append(L"VFS\\"), 108506);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID /* lpReserved */)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::StartHook(hModule);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

extern "C" VOID __declspec(dllexport) Dir_A() {}
