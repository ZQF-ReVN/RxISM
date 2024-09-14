#include "ISA.h"
#include <Zut/ZxFS.h>
#include <Zut/ZxCvt.h>
#include <Zut/ZxMem.h>
#include <Zut/ZxFile.h>
#include <ReVN/RxISM/Core/ISM_Struct.h>
#include <ranges>
#include <array>


namespace ZQF::ReVN::RxISM
{
    auto ISA::Export(const std::string_view msPackPath, const std::string_view msDir) -> void
    {
        ZxFile ifs{ msPackPath, ZxFile::OpenMod::ReadSafe };

        const auto hdr_bin = ifs.Get<Struct::ISA::HDR>();

        const auto file_count = static_cast<std::size_t>(hdr_bin.nFileCount);

        ZxMem index_table_mem;
        {
            index_table_mem.Resize(file_count * Struct::ISA::Entry::SizeBytes(), true, true);
            ifs.Read(index_table_mem.Span());
        }

        ZxCvt cvt;
        ZxMem cache;
        std::string sav_dir{ msDir };
        for (const auto& entry : index_table_mem.Span<Struct::ISA::Entry>())
        {
            ifs.Seek(entry.nFOA, ZxFile::MoveWay::Set);
            cache.Resize(entry.nBytes, true, true);
            ifs.Read(cache.Span());
            cache.Save(sav_dir.append(cvt.MBCSToUTF8(entry.aFileName.data(), 932)));
            sav_dir.resize(msDir.size());
        }
    }

    auto ISA::Import(const std::string_view msDir, const std::string_view msPackPath) -> void
    {
        const auto file_path_vec = ZxFS::Searcher::GetFilePaths(msDir, true, false);

        ZxMem cache_mem{ Struct::ISA::HDR::SizeBytes() + (file_path_vec.size() * Struct::ISA::Entry::SizeBytes()) };

        ZxMem& index_mem = cache_mem;
        {
            index_mem << std::span{ std::string_view{"ISM ENGLISH "} } << static_cast<std::uint16_t>(file_path_vec.size()) << static_cast<std::uint16_t>(0);

            ZxCvt cvt;
            auto foa{ static_cast<std::uint32_t>(index_mem.SizeBytes()) };
            for (const auto& file_path : file_path_vec)
            {
                const auto file_bytes = ZxFS::FileSize(file_path).value();
                const auto file_name_sjis = cvt.UTF8ToMBCS(ZxFS::FileName(file_path), 932);

                index_mem << std::span{ file_name_sjis.data(), file_name_sjis.size() + 1 };
                index_mem.PosInc(sizeof(Struct::ISA::Entry::aFileName) - (file_name_sjis.size() + 1));

                index_mem
                    << static_cast<std::uint32_t>(foa)
                    << static_cast<std::uint32_t>(file_bytes)
                    << static_cast<std::uint32_t>(0);

                foa += static_cast<std::uint32_t>(file_bytes);
            }
        }

        ZxFile ofs_isa{ msPackPath, ZxFile::OpenMod::WriteForce };
        {
            ofs_isa << index_mem.Span();

            ZxMem& data_mem = cache_mem;
            for (const auto& file_path : file_path_vec)
            {
                data_mem.Load(file_path);
                ofs_isa.Write(data_mem.Span());
            }
        }
    }
} // namespace RxISM
