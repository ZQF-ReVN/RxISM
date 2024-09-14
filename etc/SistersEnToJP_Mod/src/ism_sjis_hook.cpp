#include <Windows.h>
#include <detours.h>
#include <cstdint>


static uint32_t SJISToUTF16(uint32_t nChar, bool isSwap)
{
    uint32_t char_u16{};
    uint32_t char_sjis = ((nChar >> 8) | (nChar << 8)) & 0xFFFF;
    ::MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, (char*)&char_sjis, 2, (wchar_t*)&char_u16, 2);
    return isSwap ? ((char_u16 >> 8) | (char_u16 << 8)) & 0xFFFF : char_u16;
}


typedef void(__fastcall* Fn_DrawCharWDBCS_t)(uint32_t nThis, uint32_t nEDX, uint32_t nChar, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7);
static Fn_DrawCharWDBCS_t sg_fnDrawCharWDBCS;
static void __fastcall DrawCharWDBCS_Hook(uint32_t nThis, uint32_t nEDX, uint32_t nChar, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7)
{
    return sg_fnDrawCharWDBCS(nThis, nEDX, ::SJISToUTF16(nChar, true), a3, a4, a5, a6, a7);
}

typedef void(__fastcall* Fn_DrawCharWSBCS_t)(uint32_t nThis, uint32_t nEDX, uint32_t usChar, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7);
static Fn_DrawCharWSBCS_t sg_fnDrawCharWSBCS;
static void __fastcall DrawCharWSBCS_Hook(uint32_t nThis, uint32_t nEDX, uint32_t ucChar, uint32_t a3, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7)
{
    return sg_fnDrawCharWSBCS(nThis, nEDX, (ucChar >= 0x81) ? ::SJISToUTF16(ucChar << 8, false) : (ucChar & 0xFF), a3, a4, a5, a6, a7);
}

using Fn_IsDBCSLeadByte_t = decltype(&::IsDBCSLeadByte);
static Fn_IsDBCSLeadByte_t sg_fnIsDBCSLeadByte = ::IsDBCSLeadByte;
static BOOL __stdcall IsDBCSLeadByte_Hook(BYTE TestChar)
{
    if ((TestChar >= 0x81) && (TestChar <= 0x9F)) { return TRUE; }
    if ((TestChar >= 0xE0) && (TestChar <= 0xEF)) { return TRUE; }
    return FALSE;
}


static void PatchLens(HMODULE hISMDLL)
{
    // movzx ecx, byte ptr ss : [ebp + 8] -> movzx ecx, word ptr ss : [ebp + 8]
    LPVOID lens_limit_0_addr = (LPVOID)((size_t)(hISMDLL)+0x3012E);
    LPVOID lens_limit_1_addr = (LPVOID)((size_t)(hISMDLL)+0x301D3);

    DWORD old_protect{};
    DWORD bytes_written{};
    ::VirtualProtect(lens_limit_0_addr, 1, PAGE_EXECUTE_READWRITE, &old_protect);
    ::VirtualProtect(lens_limit_1_addr, 1, PAGE_EXECUTE_READWRITE, &old_protect);
    ::WriteProcessMemory(::GetCurrentProcess(), lens_limit_0_addr, "\xB7", 1, &bytes_written);
    ::WriteProcessMemory(::GetCurrentProcess(), lens_limit_1_addr, "\xB7", 1, &bytes_written);
}

static void PatchEncoding(HMODULE hISMDLL)
{
    sg_fnDrawCharWDBCS = (Fn_DrawCharWDBCS_t)((size_t)(hISMDLL)+0x301F9);
    sg_fnDrawCharWSBCS = (Fn_DrawCharWSBCS_t)((size_t)(hISMDLL)+0x300D6);

    ::DetourTransactionBegin();
    ::DetourUpdateThread(GetCurrentThread());
    ::DetourAttach(&sg_fnIsDBCSLeadByte, IsDBCSLeadByte_Hook);
    ::DetourAttach(&sg_fnDrawCharWDBCS, DrawCharWDBCS_Hook);
    ::DetourAttach(&sg_fnDrawCharWSBCS, DrawCharWSBCS_Hook);
    ::DetourTransactionCommit();
}

static DWORD WINAPI WaitISMDLLLoadThread(LPVOID lpThreadParameter)
{
    while (true)
    {
        // waiting for ism.dll load
        ::Sleep(1000);
        HMODULE ism_dll_handle = ::GetModuleHandleW(L"ism.dll");
        if (ism_dll_handle == NULL) { continue; }

        // patch ism.dll
        ::PatchLens(ism_dll_handle);
        ::PatchEncoding(ism_dll_handle);
        break;
    }

    return 0;
}

static void StartHook(HMODULE hDllBase)
{
    ::CreateThread(nullptr, 0, ::WaitISMDLLLoadThread, nullptr, 0, nullptr);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        ::DetourRestoreAfterWith();
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
