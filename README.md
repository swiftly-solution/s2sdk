# S2SDK

[![Sync](https://img.shields.io/github/actions/workflow/status/swiftly-solution/s2sdk/sync.yml?branch=cs2&label=sync)](https://github.com/swiftly-solution/s2sdk/actions)

C++ SDK headers and support code for Source 2 games, built as a static library. Targets Counter-Strike 2.

Public headers live under `src/public/`. Function bodies live in matching `.cpp` files under `src/`, so the library links without inline duplicates.

## Acknowledgement

This repository is created from [https://github.com/alliedmodders/s2sdk/tree/cs2](https://github.com/alliedmodders/s2sdk/tree/cs2). The commit history was not saved because we don't want to carry out hundreds of MB on each checkout.

## Layout

| Folder                                             | Purpose                                                          |
| -------------------------------------------------- | ---------------------------------------------------------------- |
| `src/public/`                                      | Public headers, one subfolder per module.                        |
| `src/tier0`, `tier1`                               | Core utilities: containers, strings, convars, keyvalues, memory. |
| `src/mathlib`                                      | Vectors, matrices, quaternions, color conversion, SIMD math.     |
| `src/entity2`                                      | Entity system: handles, identity, keyvalues, variants.           |
| `src/schemasystem`, `schemalib`                    | Schema classes, types and metadata.                              |
| `src/networksystem`, `networkstrings`              | Network messages and string tables.                              |
| `src/engine2`, `game`                              | Engine and game interfaces.                                      |
| `src/filesystem`, `resourcesystem`, `resourcefile` | Files and resources.                                             |
| `src/steamworks`                                   | Steamworks API headers.                                          |
| `libs/`                                            | Prebuilt `tier0` and `steam_api` import libraries.               |
| `protobufs/`                                       | `protobuf` 3.21.8 runtime and generated `protos` target.         |

## Quick start

### Prerequisites

- **Windows**: Visual Studio with the C++ workload
- **Linux**: GCC with C++23 support
- **XMake**

### Build

```bash
xmake build s2sdk
```

The `s2sdk` target pulls in `protos` and `protobuf` automatically and merges them into one archive (`build_merge_archive`).

### Use as a dependency

```lua
includes("vendor/s2sdk")

target("my-plugin")
    add_deps("s2sdk")
```

`s2sdk` exports its include dir (`src`) and links `tier0` and `steam_api` publicly. Include headers as `public/<module>/<name>.h`.

## Dead-code stripping

Library and consumers are built for section-level garbage collection:

| Platform | Compile                                     | Link                |
| -------- | ------------------------------------------- | ------------------- |
| Windows  | `/Gy /Gw`                                   | `/OPT:REF /OPT:ICF` |
| Linux    | `-fPIC -ffunction-sections -fdata-sections` | `-Wl,--gc-sections` |

Unused functions and data do not reach the final binary.

## Updating binaries

`tracked_files.json` lists Steam depot files (`tier0`, `steam_api`) tracked for updates. `.github/workflows/sync.yml` pulls new versions from Steam depots.

## Architecture

```
s2sdk/
├── src/
│   ├── public/            # Public headers (<module>/<name>.h)
│   ├── <module>/          # Matching .cpp implementations
│   ├── common.cpp         # Shared float helpers
│   └── const.cpp
├── libs/
│   ├── win64/             # tier0.lib, steam_api64.lib
│   └── linuxsteamrt64/    # libtier0.so, libsteam_api.so
├── protobufs/
│   ├── protobuf-3.21.8/   # Protobuf runtime
│   └── protos/            # Source 2 .proto files
├── cpp_properties.lua     # Shared C++ rule
├── tracked_files.json     # Depot files kept in sync
└── xmake.lua
```

## Community

- **Issues**: [Report bugs and request features](https://github.com/swiftly-solution/s2sdk/issues)
