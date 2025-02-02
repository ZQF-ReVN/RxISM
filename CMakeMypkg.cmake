include(FetchContent)

FetchContent_Declare(
  ZxFile
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFile.git
  GIT_TAG fe4912736343c69bedd77b4ca0c10e9e4b962bec)
FetchContent_MakeAvailable(ZxFile)

FetchContent_Declare(
  ZxMem
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxMem.git
  GIT_TAG 6446f0834bf39196f51bba0974b8fea891785a73)
FetchContent_MakeAvailable(ZxMem)

FetchContent_Declare(
  ZxCvt
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxCvt.git
  GIT_TAG cd5d3967a1c7552fdd054215a2961e3cb43d1312)
FetchContent_MakeAvailable(ZxCvt)

FetchContent_Declare(
  ZxFS
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxFS.git
  GIT_TAG 6bb36bfc28c3cf2bc8ad61a8786b1031c6ffb488)
FetchContent_MakeAvailable(ZxFS)

FetchContent_Declare(
  ZxJson
  GIT_REPOSITORY https://github.com/ZQF-Zut/ZxJson.git
  GIT_TAG 00bcd71d5332b3842573ca41829ae9673b2c2a73)
FetchContent_MakeAvailable(ZxJson)

if((CMAKE_SYSTEM_NAME STREQUAL "Windows") AND MSVC)
    FetchContent_Declare(
        ZxHook
        GIT_REPOSITORY https://github.com/ZQF-Zut/ZxHook.git
        GIT_TAG 46a26cf69e84babced2e6e52be5cc6aee6ec0ef9)
    FetchContent_MakeAvailable(ZxHook)
endif()
