set_project("s2sdk")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

includes("protobufs")
includes("cpp_properties.lua")

target("s2sdk")
    set_kind("static")
    add_rules("cpp_properties")
    set_languages("cxx23")
    add_files("src/**.cpp")
    add_includedirs("src", {public = true})
    add_deps("protos", {public = true})

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
