#pragma once
#include <span>
#include <array>
#include <cstdint>
#include <vector>
#include <variant>
#include <Zut/ZxMem.h>
#include <Zut/ZxJson.h>


namespace ZQF::ReVN::RxISM
{
    class ISM
    {
    private:
        struct ObjectInfo
        {
        public:
            std::uint32_t m_nOffset{}; // CodeSeg Offset
            std::uint32_t m_nLocalVarCount{};
            std::uint32_t m_nArgCount{};

        public:
            ObjectInfo()
            {

            }

            ObjectInfo(ZxMem& rfMem)
            {
                this->BinaryLoad(rfMem);
            }

        public:
            auto BinaryLoad(ZxMem& rfMem) -> void
            {
                rfMem >> m_nOffset >> m_nLocalVarCount >> m_nArgCount;
            }

            auto BinaryStore(ZxMem& rfMem) const -> void
            {
                rfMem << m_nOffset << m_nLocalVarCount << m_nArgCount;
            }

            auto MetaStore() const -> ZxJson::JValue
            {
                return ZxJson::JObject_t
                {
                    { "m_nOffset", m_nOffset },
                    { "m_nLocalVarCount", m_nLocalVarCount },
                    { "m_nArgCount", m_nArgCount }
                };
            }

        public:
            static auto SizeBytes() -> std::size_t
            {
                return sizeof(m_nOffset) + sizeof(m_nLocalVarCount) + sizeof(m_nArgCount);
            }
        };

        struct HDR
        {
        public:
            std::array<char,0xC> m_aSignature{}; // "ISM SCRIPT"
            std::uint32_t m_nFileBytes{};
            std::uint32_t m_nCodeSegFOA{};
            std::uint32_t m_nLocalGlobalVarCount{};
            std::uint32_t m_nLocalFunctionCount{};
            std::uint32_t m_nLocalFunctionCount_{};
            std::vector<ObjectInfo> m_aLocalFunctionInfoTable; // size=HDR::nLocalFunctionCount;

        public:
            HDR()
            {

            }

            HDR(ZxMem& rfMem)
            {
                this->BinaryLoad(rfMem);
            }

        public:
            auto BinaryLoad(ZxMem& rfMem) -> void
            {
                rfMem
                    >> m_aSignature
                    >> m_nFileBytes
                    >> m_nCodeSegFOA
                    >> m_nLocalGlobalVarCount
                    >> m_nLocalFunctionCount
                    >> m_nLocalFunctionCount_;

                for (std::size_t idx{}; idx < m_nLocalFunctionCount; idx++)
                {
                    m_aLocalFunctionInfoTable.emplace_back(rfMem);
                }
            }

            auto BinaryStore(ZxMem& rfMem) const -> void
            {
                rfMem
                    << m_aSignature
                    << m_nFileBytes
                    << m_nCodeSegFOA
                    << m_nLocalGlobalVarCount
                    << m_nLocalFunctionCount
                    << m_nLocalFunctionCount_;

                for (const auto& info : m_aLocalFunctionInfoTable)
                {
                    info.BinaryStore(rfMem);
                }
            }

            auto MetaStore() const -> ZxJson::JValue
            {
                ZxJson::JArray_t local_func_info_table;
                for (const auto& info : m_aLocalFunctionInfoTable)
                {
                    local_func_info_table.emplace_back(info.MetaStore());
                }

                return ZxJson::JObject_t
                {
                    { "m_aSignature", std::string_view{ m_aSignature.data() } },
                    { "m_nFileBytes", m_nFileBytes },
                    { "m_nCodeSegFOA", m_nCodeSegFOA },
                    { "m_nLocalGlobalVarCount", m_nLocalGlobalVarCount },
                    { "m_nLocalFunctionCount", m_nLocalFunctionCount },
                    { "m_nLocalFunctionCount_", m_nLocalFunctionCount_ },
                    { "m_aLocalFunctionInfoTable", std::move(local_func_info_table) },
                };
            }

        public:
            auto SizeBytes() const -> std::size_t
            {
                std::size_t bytes{ sizeof(m_aSignature) + sizeof(m_nFileBytes) + sizeof(m_nCodeSegFOA) + sizeof(m_nLocalGlobalVarCount) + sizeof(m_nLocalFunctionCount) + sizeof(m_nLocalFunctionCount_) };
                bytes += m_aLocalFunctionInfoTable.size() * ObjectInfo::SizeBytes();
                return bytes;
            }
        };

        struct SymbolName
        {
        public:
            std::string m_msName;

        public:
            SymbolName()
            {

            }

            SymbolName(ZxMem& rfMem)
            {
                this->BinaryLoad(rfMem);
            }

        public:
            auto BinaryLoad(ZxMem& rfMem) -> void
            {
                m_msName.resize(rfMem.Get<std::uint8_t>());
                rfMem >> std::span{ m_msName };
                rfMem.Get<std::uint8_t>(); // null char
            }

            auto BinaryStore(ZxMem& rfMem) const -> void
            {
                rfMem << static_cast<std::uint8_t>(m_msName.size());
                rfMem << std::span{ m_msName };
                rfMem << std::uint8_t{ '\0' };
            }

            auto MetaStore() const -> ZxJson::JValue
            {
                return m_msName;
            }

        public:
            auto SizeBytes() const -> std::size_t
            {
                return 1 + m_msName.size() + 1;
            }
        };

        struct ImportSymbolTable
        {
        public:
            using ImportSymbolEntry = SymbolName;

        public:
            std::uint32_t m_nCount{};                 // func + galoblvar
            std::uint32_t m_nFunctionCount{};
            std::uint32_t m_nGlobalVarCount{};
            std::vector<ImportSymbolEntry> m_vcImportTable; // size=nCount

        public:
            ImportSymbolTable()
            {

            }

            ImportSymbolTable(ZxMem& rfMem)
            {
                this->BinaryLoad(rfMem);
            }

        public:
            auto BinaryLoad(ZxMem& rfMem) -> void
            {
                rfMem >> m_nCount >> m_nFunctionCount >> m_nGlobalVarCount;

                for (std::size_t idx{}; idx < m_nCount; idx++)
                {
                    m_vcImportTable.emplace_back(rfMem);
                }
            }

            auto BinaryStore(ZxMem& rfMem) const -> void
            {
                rfMem << m_nCount << m_nFunctionCount << m_nGlobalVarCount;

                for (const auto& entry : m_vcImportTable)
                {
                    entry.BinaryStore(rfMem);
                }
            }

            auto MetaStore() const -> ZxJson::JValue
            {
                ZxJson::JArray_t import_table;
                for (const auto& entry : m_vcImportTable)
                {
                    import_table.emplace_back(entry.MetaStore());
                }

                return ZxJson::JObject_t
                {
                    { "m_nCount", m_nCount },
                    { "m_nFunctionCount", m_nFunctionCount },
                    { "m_nGlobalVarCount", m_nGlobalVarCount },
                    { "m_vcImportTable", std::move(import_table) }
                };
            }

        public:
            auto SizeBytes() const->std::size_t
            {
                std::size_t bytes{ sizeof(m_nCount) + sizeof(m_nFunctionCount) + sizeof(m_nGlobalVarCount) };
                for (const auto& entry : m_vcImportTable)
                {
                    bytes += entry.SizeBytes();
                }

                return bytes;
            }
        };

        struct ExportSymbolTable
        {
        public:
            struct ExportSymbolEntry
            {
            public:
                ISM::ObjectInfo m_ObjectInfo;
                ISM::SymbolName m_SymbolName;

            public:
                ExportSymbolEntry()
                {

                }

                ExportSymbolEntry(ZxMem& rfMem)
                {
                    this->BinaryLoad(rfMem);
                }

            public:
                auto BinaryLoad(ZxMem& rfMem) -> void
                {
                    m_ObjectInfo.BinaryLoad(rfMem);
                    m_SymbolName.BinaryLoad(rfMem);
                }

                auto BinaryStore(ZxMem& rfMem) const -> void
                {
                    m_ObjectInfo.BinaryStore(rfMem);
                    m_SymbolName.BinaryStore(rfMem);
                }

                auto MetaStore() const -> ZxJson::JValue
                {
                    return ZxJson::JObject_t{ { m_SymbolName.m_msName, m_ObjectInfo.MetaStore() } };
                }

            public:
                auto SizeBytes() const -> std::size_t
                {
                    return m_ObjectInfo.SizeBytes() + m_SymbolName.SizeBytes();
                }
            };

        public:
            std::uint32_t m_nCount{};                       // func + galoblvar
            std::uint32_t m_nFunctionCount{};
            std::uint32_t m_nGlobalVarCount{};
            std::vector<ExportSymbolEntry> m_aExportTable;  // size=nCount

        public:
            ExportSymbolTable()
            {

            }

            ExportSymbolTable(ZxMem& rfMem)
            {
                this->BinaryLoad(rfMem);
            }

        public:
            auto BinaryLoad(ZxMem& rfMem) -> void
            {
                rfMem >> m_nCount >> m_nFunctionCount >> m_nGlobalVarCount;

                for (std::size_t idx{}; idx < m_nCount; idx++)
                {
                    m_aExportTable.emplace_back(rfMem);
                }
            }

            auto BinaryStore(ZxMem& rfMem) const -> void
            {
                rfMem << m_nCount << m_nFunctionCount << m_nGlobalVarCount;

                for (const auto& entry : m_aExportTable)
                {
                    entry.BinaryStore(rfMem);
                }
            }

            auto MetaStore() const -> ZxJson::JValue
            {
                ZxJson::JArray_t export_table;
                for (const auto& entry : m_aExportTable)
                {
                    export_table.emplace_back(entry.MetaStore());
                }

                return ZxJson::JObject_t
                {
                    { "m_nCount", m_nCount },
                    { "m_nFunctionCount", m_nFunctionCount },
                    { "m_nGlobalVarCount", m_nGlobalVarCount },
                    { "m_aExportTable", std::move(export_table) }
                };
            }

        public:
            auto SizeBytes() const->std::size_t
            {
                std::size_t bytes{ sizeof(m_nCount) + sizeof(m_nFunctionCount) + sizeof(m_nGlobalVarCount) };
                for (const auto& entry : m_aExportTable)
                {
                    bytes += entry.SizeBytes();
                }

                return bytes;
            }
        };

    public:
        HDR m_HDR;
        ExportSymbolTable m_ExportSymbolTable;
        ImportSymbolTable m_ImportSymbolTable;
        std::span<std::uint8_t> m_spCode;

    private:
        ZxMem m_mISM;

    public:
        ISM()
        {

        };

        auto BinaryLoad() -> void
        {
            m_mISM.Load("CHAP1_A.ISM");

            m_HDR.BinaryLoad(m_mISM);
            m_ExportSymbolTable.BinaryLoad(m_mISM);
            m_ImportSymbolTable.BinaryLoad(m_mISM);
            m_spCode = m_mISM.SpanCur();
        }

        auto BinaryStore() const -> ZxMem
        {
            ZxMem ism_mem{ this->SizeBytes() };
            m_HDR.BinaryStore(ism_mem);
            m_ExportSymbolTable.BinaryStore(ism_mem);
            m_ImportSymbolTable.BinaryStore(ism_mem);
            ism_mem << m_spCode;
            return ism_mem;
        }

        auto MeatStore() -> ZxJson::JValue
        {
            return ZxJson::JObject_t
            {
                { "m_HDR", m_HDR.MetaStore() },
                { "m_ExportSymbolTable", m_ExportSymbolTable.MetaStore() },
                { "m_ImportSymbolTable", m_ImportSymbolTable.MetaStore() }
            };
        }

        auto SizeBytes() const -> std::size_t
        {
            std::size_t bytes{};
            bytes += m_HDR.SizeBytes();
            bytes += m_ExportSymbolTable.SizeBytes();
            bytes += m_ImportSymbolTable.SizeBytes();
            bytes += m_spCode.size_bytes();
            return bytes;
        }
    };
} // namespace ZQF::ReVN::RxISM
