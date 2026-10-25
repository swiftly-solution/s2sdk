set_project("s2sdk")
set_version("1.0.0")

add_rules("mode.debug", "mode.release")

includes("protobufs")

target("s2sdk")
    set_kind("static")
    add_deps("protos", {public = true})
target_end()
