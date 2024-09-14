#pragma once
#include <vector>
#include <cstdint>
#include <string>
#include <string_view>


namespace ZQF::ReVN::RxISM
{
	class ISA
	{
	public:
		static auto Export(const std::string_view msPackPath, const std::string_view msDir) -> void;
		static auto Import(const std::string_view msDir, const std::string_view msPackPath) -> void;
	};
} // namespace ZQF::ReVN::RxISM
