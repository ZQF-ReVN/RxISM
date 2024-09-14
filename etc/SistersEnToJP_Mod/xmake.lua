-- project
set_project("ism-sjis-mod")
set_xmakever("2.8.9")

-- language
set_warnings("all")
set_languages("c++17")

-- allows
set_allowedplats("windows")
set_allowedarchs("windows|x86")
set_allowedmodes("debug", "release")

-- rules
add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.debug", "mode.release")

-- lto
if is_mode("release") then
    set_policy("build.optimization.lto", true)
end

-- pkg
add_requires("microsoft-detours")


target("ism_sjis_hook")
    set_kind("shared")
    add_files("src/ism_sjis_hook.cpp")
    add_packages("microsoft-detours")

target("ism_sjis_loader")
    set_kind("binary")
    add_files("src/ism_sjis_loader.cpp")
    add_syslinks("user32")
    add_packages("microsoft-detours")
