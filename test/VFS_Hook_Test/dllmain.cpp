#include <Windows.h>
#include <ZxHook/SHooker.h>
#include <ReVN/RxISM/Hook/VFS_Hook.h>

namespace ZxHook { using namespace ZQF::ZxHook; }
namespace RxISM { using namespace ZQF::ReVN::RxISM; }

static auto __stdcall LoadLibraryA_Hook(LPCSTR lpLibFileName) -> HMODULE
{
    if (::strncmp(lpLibFileName, "ISM.DLL", 7) == 0)
    {
        const auto hdll = ZxHook::SHooker<LoadLibraryA_Hook>::FnRaw(lpLibFileName);
        if (hdll != NULL)
        {
            RxISM::VFSHook::Install(L"_Nukige_\\patch\\", reinterpret_cast<std::size_t>(hdll) + 0x1A7DA);
        }
        ZxHook::SHooker<LoadLibraryA_Hook>::DetachAndCommit();
        return hdll;
    }
    return ZxHook::SHooker<LoadLibraryA_Hook>::FnRaw(lpLibFileName);
}

static auto StartHook(HMODULE /* hDllBase */) -> void
{
    ZxHook::SHooker<LoadLibraryA_Hook>::AttachAndCommit(::LoadLibraryA);
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
