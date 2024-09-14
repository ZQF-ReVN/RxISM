#include <print>
#include <iostream>
#include <ReVN/RxISM/Core/ISA.h>

namespace RxISM { using namespace ZQF::ReVN::RxISM; }


auto main() -> int
{
    try
    {
        RxISM::ISA::Export("script.isa", "script/");
        // RxISM::ISA::Import("system/", "system.isa.new");
        // RxISM::ISA::Export("system.isa.new", "system_new/");
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
