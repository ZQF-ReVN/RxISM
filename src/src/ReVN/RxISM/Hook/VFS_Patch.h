#pragma once
#include <cstddef>
#include <string_view>


namespace ZQF::ReVN::RxISM
{
    class VFSPatch
    {
    public:
        static auto Install(const std::wstring_view wsPatchDir,const std::size_t vaFnISMFileOpen) -> void;
    };
}
