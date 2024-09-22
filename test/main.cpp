#include <print>
#include <iostream>
#include <ReVN/RxISM/Core/ISA.h>
#include <ReVN/RxISM/Core/ISM.h>

namespace RxISM { using namespace ZQF::ReVN::RxISM; }


auto main() -> int
{
    try
    {
        // RxISM::ISA::Export("script.isa", "script/");
        RxISM::ISM ism;
        ism.BinaryLoad();
        auto mem = ism.BinaryStore();
        mem.Save("1.bin");
        auto json = ism.MeatStore();
        ZxJson::StoreViaFile("1.json", json, true, true);
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
