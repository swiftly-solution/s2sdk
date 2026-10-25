local pb_dir = path.join(os.scriptdir(), "protobuf-3.21.8")

target("protobuf")
    set_kind("static")
    set_languages("cxx14")
    set_basename("libprotobuf")

    on_load(function (target)
        local pb_dir = path.join(os.scriptdir(), "protobuf-3.21.8")
        -- Only what the generated protos link against (found with a /WHOLEARCHIVE link of libprotos + /MAP).
        -- Everything else in the cmake lists (compiler/, util/, gzip, file streams, ...) is left out to keep the lib small.
        local keep = {}
        for _, name in ipairs({
            "any.cc", "any_lite.cc", "arena.cc", "arenastring.cc", "coded_stream.cc", "common.cc",
            "descriptor.cc", "descriptor.pb.cc", "descriptor_database.cc", "dynamic_message.cc",
            "extension_set.cc", "extension_set_heavy.cc", "generated_message_bases.cc",
            "generated_message_reflection.cc", "generated_message_util.cc", "implicit_weak_message.cc",
            "int128.cc", "io_win32.cc", "map.cc", "map_field.cc", "message.cc", "message_lite.cc", "parse_context.cc",
            "reflection_ops.cc", "repeated_field.cc", "repeated_ptr_field.cc", "status.cc", "stringpiece.cc",
            "stringprintf.cc", "strtod.cc", "structurally_valid.cc", "strutil.cc", "substitute.cc",
            "text_format.cc", "tokenizer.cc", "unknown_field_set.cc", "wire_format.cc", "wire_format_lite.cc",
            "zero_copy_stream.cc", "zero_copy_stream_impl.cc", "zero_copy_stream_impl_lite.cc"
        }) do
            keep[name] = true
        end
        for _, spec in ipairs({
            {"libprotobuf-lite.cmake", "libprotobuf_lite_files"},
            {"libprotobuf.cmake", "libprotobuf_files"}
        }) do
            local content = io.readfile(path.join(pb_dir, "cmake", spec[1]))
            local files = content:match("set%(" .. spec[2] .. "%s(.-)%)")
            for src in files:gmatch("%${protobuf_SOURCE_DIR}/(%S+%.cc)") do
                if keep[path.filename(src)] then
                    target:add("files", path.join(pb_dir, src))
                end
            end
        end
    end)
    add_includedirs(path.join(pb_dir, "src"), {public = true})

    if is_plat("windows") then
        add_cxflags("/w", "/EHsc", "/utf-8", "/bigobj", {force = true})
    else
        add_cxflags("-w", "-fPIC", {force = true})
        add_defines("_GLIBCXX_USE_CXX11_ABI=0", {public = true})
    end

    set_optimize("fastest")
    set_strip("all")
    set_symbols("none")
target_end()

target("protos")
    set_kind("static")
    set_languages("cxx14")
    set_basename("libprotos")
    add_deps("protobuf", {public = true})

    on_load(function (target)
        local root = os.scriptdir()
        local proto_dir = path.join(root, "protos")
        local out_dir = path.join(target:autogendir(), "proto")
        local exclude = {["enums_clientserver"] = true}

        local stale = false
        local names = {}
        for _, file in ipairs(os.files(path.join(proto_dir, "*.proto"))) do
            local name = path.basename(file)
            if not exclude[name] then
                table.insert(names, name)
                local cc = path.join(out_dir, name .. ".pb.cc")
                if not os.isfile(cc) or os.mtime(file) > os.mtime(cc) then
                    stale = true
                end
            end
        end

        if stale then
            local protoc = path.join(root, "bin", is_host("windows") and "protoc.exe" or "protoc")
            local args = {
                "--proto_path=" .. path.join(root, "protobuf-3.21.8", "src"),
                "--proto_path=" .. proto_dir,
                "--cpp_out=" .. out_dir
            }
            for _, name in ipairs(names) do
                table.insert(args, path.join(proto_dir, name .. ".proto"))
            end
            os.mkdir(out_dir)
            os.vrunv(protoc, args)
        end

        for _, name in ipairs(names) do
            target:add("files", path.join(out_dir, name .. ".pb.cc"))
        end
        target:add("includedirs", out_dir, {public = true})
    end)

    if is_plat("windows") then
        add_cxflags("/w", "/EHsc", "/utf-8", "/bigobj", {force = true})
    else
        add_cxflags("-w", "-fPIC", {force = true})
    end
target_end()
