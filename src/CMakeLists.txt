# RxISM
cmake_minimum_required(VERSION 3.25)

# Project Name
project(ReVN_RxISM)

# Export Symbols
if(BUILD_SHARED_LIBS)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# Set Source File Path
list(APPEND ReVN_RxISM_Core_Src_Files
    "src/ReVN/RxISM/Core/ISA.cpp")

# Set Library
add_library(ReVN_RxISM_Core ${ReVN_RxISM_Core_Src_Files})
add_library(ReVN::RxISM::Core ALIAS ReVN_RxISM_Core)
target_include_directories(ReVN_RxISM_Core PUBLIC "src")
target_compile_features(ReVN_RxISM_Core PRIVATE cxx_std_23)
target_compile_options(ReVN_RxISM_Core PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
target_link_libraries(ReVN_RxISM_Core PRIVATE Zut::ZxFile Zut::ZxFS Zut::ZxCvt Zut::ZxMem)

# Warning
if(MSVC)
    target_compile_options(ReVN_RxISM_Core PRIVATE /W4)
else()
    target_compile_options(ReVN_RxISM_Core PRIVATE -Wall -Wextra)
endif()

