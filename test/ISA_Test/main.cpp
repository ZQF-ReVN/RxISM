#include <print>
#include <iostream>
#include <ReVN/RxISM/Core/ISA.h>

namespace RxISM { using namespace ZQF::ReVN::RxISM; }


auto main() -> int
{
    try
    {
        RxISM::ISA::Export("script.isa", "script/");
        RxISM::ISA::Import("script/", "script.isa.new", "ISM ENGLISH ");
    }
    catch (const std::exception& err)
    {
        std::println(std::cerr, "std::exception: {}", err.what());
    }
}
