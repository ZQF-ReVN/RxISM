## ISM Engine Version

|    Date    | Developer |                   Title                    | Type | ism.exe Ver | ism.dll Ver |
| :--------: | :-------: | :----------------------------------------: | :--: | :---------: | :---------: |
| 2003-10-10 | Jellyfish |          LOVERS～恋に落ちたら…～           | PKG  |  1.0.2.527  | 1.2.0.6174  |
| 2007-12-28 | Jellyfish |    LOVERS～恋に落ちたら…～PREMIUM PACK     | PKG  |  1.0.5.719  | 1.2.0.6224  |
| 2011-04-28 | Jellyfish | GREEN～秋空のスクリーン～ Standard Edition | PKG  |  1.0.3.188  |  1.3.0.141  |
| 2011-04-28 | Jellyfish |          SISTERS ～夏の最後の日～          | PKG  |  1.0.3.748  |  1.3.0.79   |
| 2013-02-22 | Jellyfish |   SISTERS ～夏の最後の日～ Ultra Edition   | PKG  |  1.1.0.765  |  1.3.0.96   |
| 2023-06-03 | Jellyfish |        SISTERS ～夏の最後の日～ Eng        |  DL  |  1.1.0.765  |   2.0.0.0   |



## Log

- 2024年7月8日02点23分

  从这个网站可以下到一些ISM引擎的工具，其中可以发现ism引擎的原始脚本是为iss后缀的纯文本文件，通过ismc.exe进行编译，ismc.exe 先调用cpp.exe进行预处理，猜测是宏的展开，可以发现它脚本里有一些类似c的.h头文件，但只有宏定义，通过cpp.exe处理后生成.tmp的处理后文件，然后调用ismp.exe对tmp文件进行编译，编译会生成.ext文件，.ext应该是用来描述函数导入导出的，.ism是编译完成的字节码，.sym暂时不知道有什么作用，没正式发布的游戏中只有.ism文件，猜测.sym和.ext都是中间文件，通过对.sym的观察可以发现，里面有一些在iss中定义的变量名的名文字符串
  
- 2024年9月13日

  纯文本的脚本文件.iss通过 ismc.exe进行编译生成.ext、.ism、.sym、.tmp文件

  ismc.exe的作用相当于

  ```shell
  > cpp.exe -E marsys.iss marsys.tmp
  > ismp.exe marsys.tmp marsys.ism
  ```

  ismc.exe主要的作用的调用cpp_win.exe对.iss进行预处理，然后把结果存储在.tmp文件中，接着调用ismp.exe对预处理的.tmp文件进行编译，生成.ext、.ism、.sym，其中只有ism是引擎真正执行的脚本文件，其余用于辅助编译，.ext里主要是记录了脚本中用到的不在本脚本定义的外部函数，.sym应该是ast用的。

  ext文件，%后面跟着的数子表示函数参数个数，主要用于表示本脚本用到的外部函数，和导出的函数

  ```
  CHAP1_A%1 + 1
  CountParagraph%0 + 0
  SysFlag_RefSystemFlag%1 - 1
  Main_GetSkipKeyState%1 - 1
  SysFlag_RefFlag%1 - 1
  Main_StopCharacterEyeAnimation%0 - 0
  Main_ResumeCharacterEyeAnimation%0 - 0
  SysFlag_SetFlag%2 - 2
  Movement%2 - 2
  Main_SetEffectScreen%1 - 1
  Main_SetAnimeScreen%1 - 1
  CreateThreadScroll%5 - 5
  KillThreadScroll%0 - 0
  Main_WaitWithPromoteState%2 - 2
  Main_ClearEffectScreen%0 - 0
  ResetBaseTime%0 - 0
  AnimateScreenOnMemory%5 - 5
  ```

  
## Structures

```c
struct ISA
{
	struct FileEntry
	{
		Array<uint8_t, 0x34> aFileName;
		uint32_t nFOA; // .isa file offset address
		uint32_t nSize;
		uint32_t nFlag; // 可能和版本有关，会影响文件系统的搜索
	};

	Array<uint8_t, 0xC> aSignature; // "ISM ENGLISH " "ISM ARCHIVED"
	uint16_t nFileCount;
	uint16_t nUn;
};

struct ISM
{
    struct HDR
    {
        Array<uint8_t, 0xC> aSignature; // "ISM SCRIPT"
		uint32_t nFileBytes;
		uint32_t nCodeSegFOA;
		uint32_t nLocalGlobalVarCount;
		uint32_t nLocalFunctionCount;
		uint32_t nLocalFunctionCount_;
    };
    
    struct ObjectInfo
    {
        union
        {
        struct Function
		{
			uint32_t nOffset; // CodeSeg Offset
			uint32_t nLocalVarCount;
			uint32_t nArgCount;
		};
    
    	struct GlobalVar
		{
			uint32_t nOffset; // > 0x80000000  StackOffset?
			uint32_t nUn;
			uint32_t nUn;
		};
        }
    }
    
    struct LocalFunctionTable
    {
        Array<ISM::ObjectInfo::Function, HDR::nLocalFunctionCount> aLocalFunctionInfoTable;
    };
    
    struct SymbolName
    {
        uint8_t ucNameBytes;
        Array<uint8_t, nBytes> aName;
        uint8_t ucNullChar;
    };
    
    struct ImportSymbolTable
    {
        uint32_t nCount; // func + galoblvar
        uint32_t nFunctionCount;
        uint32_t nGlobalVarCount;
        Array<ISM::Name, nCount> aImportTable;
    };
    
    struct ExportSymbolTable
    {
        uint32_t nCount; // func + galoblvar
        uint32_t nFunctionCount;
        uint32_t nGlobalVarCount;
        Array<Pair<ISM::ObjectInfo, ISM::SymbolName>, nCount> aExportTable;
    };

	HDR                 m_HDR;
	LocalFunctionTable  m_LocalFunctionTable;
	ExportSymbolTable   m_ExportSymbolTable;
	ImportSymbolTable   m_ImportSymbolTable;
    Array<uint8_t, ???> m_CodeArray;
}
```