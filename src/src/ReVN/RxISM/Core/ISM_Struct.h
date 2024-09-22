#pragma once
#include <cstdint>
#include <array>


namespace ZQF::ReVN::RxISM
{
#pragma pack(push,1)
    struct Struct
    {
        struct ISA
        {
            struct HDR
            {
                char aSignature[0xC];
                std::uint16_t nFileCount;
                std::uint16_t nFlag;

                static auto SizeBytes() -> std::size_t { return sizeof(aSignature) + sizeof(nFileCount) + sizeof(nFlag); }
            };

            struct Entry
            {
                char aFileName[0x34];
                std::uint32_t nFOA;  // .isa file offset address
                std::uint32_t nBytes;
                std::uint32_t nUn;   // 00

                static auto SizeBytes() -> std::size_t { return sizeof(aFileName) + sizeof(nFOA) + sizeof(nBytes) + sizeof(nUn); }
            };
        };
    };
#pragma pack(pop)
}
