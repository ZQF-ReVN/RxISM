#include "VFS_Patch.h"
#include <Windows.h>
#include <string>
#include <memory>
#include <ZxHook/SHooker.h>


namespace ZQF::ReVN::RxISM
{
    static std::wstring sg_wsPatchDir{};
    static std::size_t sg_vaISMFileOpen{};


    static auto PathSJISToWide(const std::string_view msPath) -> std::pair<std::wstring_view, std::unique_ptr<wchar_t[]>>
    {
        const auto buffer_max_chars = ((msPath.size() * sizeof(char)) + 1) * 2;
        auto buffer = std::make_unique_for_overwrite<wchar_t[]>(buffer_max_chars);
        const auto char_count_real = static_cast<std::size_t>(::MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, msPath.data(), static_cast<int>(msPath.size()), buffer.get(), static_cast<int>(buffer_max_chars)));
        buffer[char_count_real] = {};
        const std::wstring_view cvt_sv{ buffer.get(), char_count_real };
        return { cvt_sv, std::unique_ptr<wchar_t[]>{ std::move(buffer) } };
    }

    static auto __fastcall ISM_File_Open(void* This,void* EDX, const char* cpFileName, const int nFlag) -> HANDLE
    {
        if (nFlag == 0)
        {
            const auto file_name_wide = PathSJISToWide(cpFileName);
            std::wstring file_path{ sg_wsPatchDir };
            file_path.append(file_name_wide.first);
            const auto file_attributes = ::GetFileAttributesW(file_path.c_str());
            if (file_attributes != INVALID_FILE_ATTRIBUTES)
            {
                if ((file_attributes & FILE_ATTRIBUTE_DIRECTORY) == false)
                {
                    return CreateFileW(file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
                }
            }
        }
        return ZxHook::SHooker<ISM_File_Open>::FnRaw(This, EDX, cpFileName, nFlag);
    }

    static auto __stdcall LoadLibraryA_Hook(LPCSTR lpLibFileName) -> HMODULE
    {
        if (::strncmp(lpLibFileName, "ISM.DLL", 7) != 0)
        {
            return ZxHook::SHooker<LoadLibraryA_Hook>::FnRaw(lpLibFileName);
        }

        const auto hdll = ZxHook::SHooker<LoadLibraryA_Hook>::FnRaw(lpLibFileName);
        ZxHook::SHooker<LoadLibraryA_Hook>::DetachAndCommit();
        if (hdll != NULL)
        {
            ZxHook::SHooker<ISM_File_Open>::AttachAndCommit(reinterpret_cast<std::size_t>(hdll) + sg_vaISMFileOpen);
        }
        return hdll;
        
    }

    auto VFSPatch::Install(const std::wstring_view wsPatchDir, const std::size_t vaFnISMFileOpen) -> void
    {
        sg_vaISMFileOpen = vaFnISMFileOpen;
        sg_wsPatchDir.assign(wsPatchDir);
        ZxHook::SHooker<LoadLibraryA_Hook>::AttachAndCommit(::LoadLibraryA);
    }
}
