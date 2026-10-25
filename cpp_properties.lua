rule("cpp_properties")
    on_config(function (target)
        import("core.base.json")
        import("core.language.language")

        local sourcefile
        for _, file in ipairs(target:sourcefiles()) do
            if language.sourcekind_of(file) == "cxx" then
                sourcefile = file
                break
            end
        end
        if not sourcefile then
            return
        end

        local root = target:scriptdir()
        local compiler = target:compiler("cxx")
        local program = compiler:program()
        local flags = import("core.tool.compiler").compflags(sourcefile, {target = target})

        local includes, defines = {}, {}
        local cstd, cppstd = "c17", "c++23"
        local std_aliases = {["2a"] = "20", ["2b"] = "23", ["2c"] = "26", latest = "23", preview = "23"}

        local seen = {}
        local function add(list, value)
            if not seen[list] then
                seen[list] = {}
            end
            if not seen[list][value] then
                seen[list][value] = true
                table.insert(list, value)
            end
        end

        local function include(dir)
            local rel = path.relative(path.absolute(dir), root)
            if rel:startswith("..") or path.is_absolute(rel) then
                add(includes, path.translate(path.absolute(dir)):gsub("\\", "/"))
            else
                add(includes, "${workspaceFolder}/" .. rel:gsub("\\", "/"))
            end
        end

        for _, flag in ipairs(flags) do
            local value = flag:match("^[-/]external:I(.+)$") or flag:match("^[-/]isystem(.+)$") or flag:match("^[-/]I(.+)$")
            if value then
                include(value)
            elseif flag:match("^[-/]D.") then
                add(defines, (flag:sub(3):gsub('\\"', '"')))
            else
                local std = flag:match("^[-/]std[:=](.+)$")
                local cpp = std and std:match("c%+%+(%w+)$")
                if cpp then
                    cppstd = "c++" .. (std_aliases[cpp] or cpp:match("^%d+"))
                elseif std and (std:match("^c%d+$") or std:match("^gnu%d+$")) then
                    cstd = "c" .. std:match("%d+")
                end
            end
        end

        local name = path.basename(program):lower()
        local mode = name:startswith("cl") and "windows-msvc-x64"
            or (name:find("clang", 1, true) and (is_host("windows") and "windows" or "linux") .. "-clang-x64")
            or "linux-gcc-x64"

        local content = json.encode({
            version = 4,
            configurations = {{
                name = "xmake",
                includePath = includes,
                defines = defines,
                compilerPath = path.translate(program):gsub("\\", "/"),
                cStandard = cstd,
                cppStandard = cppstd,
                intelliSenseMode = mode
            }}
        }, {pretty = true, orderkeys = true}):gsub("\\/", "/") .. "\n"

        local file = path.join(root, ".vscode", "c_cpp_properties.json")
        if not os.isfile(file) or io.readfile(file) ~= content then
            io.writefile(file, content)
        end
    end)
