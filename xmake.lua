set_project("s2sdk")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

includes("protobufs")
includes("cpp_properties.lua")

target("s2sdk")
    set_kind("static")
    set_runtimes("MT", {public = true})
    add_rules("cpp_properties")
    set_languages("cxx23")
    set_policy("build.merge_archive", true)
    add_files("src/**.cpp")
    add_includedirs("src", {public = true})
    add_deps("protos", {public = true})

    if is_plat("windows") then
        add_cxflags("/Gy", "/Gw", {force = true})
        add_cxflags("/utf-8", {public = true})
        add_ldflags("/OPT:REF", "/OPT:ICF", {public = true, force = true})
        add_defines({
            "COMPILER_MSVC",
            "COMPILER_MSVC64",
            "WIN32",
            "_WIN32",
            "WINDOWS",
            "_WINDOWS",
            "CRT_SECURE_NO_WARNINGS",
            "_CRT_SECURE_NO_WARNINGS",
            "CRT_SECURE_NO_DEPRECATE",
            "_CRT_SECURE_NO_DEPRECATE",
            "CRT_NONSTDC_NO_DEPRECATE",
            "_CRT_NONSTDC_NO_DEPRECATE",
            "_MBCS",
            "PLATFORM_64BITS",
            "NDEBUG",
            "_LIBCPP_STD_VER"
        }, {public = true})
        add_syslinks({
            "psapi",
            "winmm",
            "ws2_32",
            "wldap32",
            "advapi32",
            "kernel32",
            "comdlg32",
            "crypt32",
            "normaliz",
            "wsock32",
            "legacy_stdio_definitions",
            "legacy_stdio_wide_specifiers",
            "user32",
            "gdi32",
            "winspool",
            "shell32",
            "ole32",
            "oleaut32",
            "uuid",
            "odbc32",
            "odbccp32",
            "dbghelp",
            "ntdll"
        }, {public = true})
    else
        add_defines({
            "_LINUX",
            "LINUX",
            "POSIX",
            "GNUC",
            "COMPILER_GCC",
            "PLATFORM_64BITS",
            "_GLIBCXX_USE_CXX11_ABI=0",
            "_vsnprintf=vsnprintf",
            "_alloca=alloca",
            "strcmpi=strcasecmp",
            "strnicmp=strncasecmp",
            "_snprintf=snprintf",
            "_stricmp=strcasecmp",
            "_strnicmp=strncasecmp",
            "stricmp=strcasecmp"
        }, {public = true})
        add_cxflags("-fPIC", "-ffunction-sections", "-fdata-sections", {force = true})
        add_ldflags("-Wl,--gc-sections", {public = true, force = true})
        add_syslinks({
            "gnutls",
            "z",
            "pthread",
            "ssl",
            "crypto",
            "m",
            "dl",
            "readline",
            "rt",
            "idn2",
            "psl",
            "brotlidec",
            "backtrace",
            "stdc++"
        }, {public = true})
    end

    add_cxxflags("gcc::-Wno-invalid-offsetof", {public = true})
    add_cxxflags("gcc::-Wno-return-local-addr", {public = true})
    add_cxxflags("gcc::-Wno-overloaded-virtual", {public = true})
    add_cxxflags("gcc::-Wno-unknown-pragmas", {public = true})
    add_cxxflags("gcc::-Wno-non-virtual-dtor", {public = true})
    add_cxxflags("gcc::-Wno-attributes", {public = true})
    add_cxxflags("gcc::-Wno-array-bounds", {public = true})
    add_cxxflags("gcc::-Wno-int-to-pointer-cast", {public = true})
    add_cxxflags("gcc::-Wno-sign-compare", {public = true})
    add_cxxflags("gcc::-Wno-write-strings", {public = true})
    add_cxxflags("gcc::-Wno-class-memaccess", {public = true})
    add_cxxflags("gcc::-fexceptions", {public = true})
    add_cxxflags("gcc::-fPIC", {public = true})
    add_cflags("gcc::-Wno-return-local-addr", {public = true})
    add_cflags("gcc::-Wno-unknown-pragmas", {public = true})
    add_cflags("gcc::-Wno-attributes", {public = true})
    add_cflags("gcc::-Wno-array-bounds", {public = true})
    add_cflags("gcc::-Wno-int-to-pointer-cast", {public = true})
    add_cflags("gcc::-Wno-sign-compare", {public = true})
    add_cflags("gcc::-Wno-write-strings", {public = true})
    add_cflags("gcc::-fexceptions", {public = true})
    add_cflags("gcc::-fPIC", {public = true})
    add_cflags("gcc::-pipe", {public = true})
    add_cflags("gcc::-fno-strict-aliasing", {public = true})
    add_cflags("gcc::-Wall", {public = true})
    add_cflags("gcc::-Wno-uninitialized", {public = true})
    add_cflags("gcc::-Wno-unused", {public = true})
    add_cflags("gcc::-Wno-switch", {public = true})
    add_cflags("gcc::-msse", {public = true})
    add_cflags("gcc::-fvisibility=hidden", {public = true})
    add_cflags("gcc::-mfpmath=sse", {public = true})
    add_cflags("gcc::-fno-omit-frame-pointer", {public = true})
    add_cflags("gcc::-fvisibility-inlines-hidden", {public = true})
    add_cflags("gcc::-fno-exceptions", {public = true})
    add_cflags("gcc::-fno-threadsafe-statics", {public = true})
    add_cflags("gcc::-Wno-register", {public = true})
    add_cflags("gcc::-Wno-delete-non-virtual-dtor", {public = true})
    add_cxxflags("cl::/Zc:__cplusplus", {public = true})
    add_cxxflags("cl::/Ox", {public = true})
    add_cxxflags("cl::/Zo", {public = true})
    add_cxxflags("cl::/Oy-", {public = true})
    add_cxxflags("cl::/TP", {public = true})
    add_cxxflags("cl::/W3", {public = true})
    add_cxxflags("cl::/EHsc", {public = true})
    add_cxxflags("cl::/wd4003", {public = true})
    add_cxxflags("cl::/wd4005", {public = true})
    add_cxxflags("cl::/wd4018", {public = true})
    add_cxxflags("cl::/wd4101", {public = true})
    add_cxxflags("cl::/wd4146", {public = true})
    add_cxxflags("cl::/wd4267", {public = true})
    add_cxxflags("cl::/wd4244", {public = true})
    add_cxxflags("cl::/wd4530", {public = true})
    add_cxxflags("cl::/wd4828", {public = true})
    add_cxxflags("cl::/wd4834", {public = true})

    local libs = path.join(os.scriptdir(), "libs")
    if is_plat("windows") then
        add_links({
            path.join(libs, "win64", "tier0.lib"),
            path.join(libs, "win64", "steam_api64.lib")
        }, {public = true})
    else
        add_links({
            path.join(libs, "linuxsteamrt64", "libtier0.so"),
            path.join(libs, "linuxsteamrt64", "libsteam_api.so")
        }, {public = true})
    end
target_end()
