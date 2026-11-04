<!--
# SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
# SPDX-License-Identifier: GPL-3.0-or-later

# SPDX-FileCopyrightText: 2018 yuzu Emulator Project
# SPDX-License-Identifier: GPL-2.0-or-later
-->
<!-- lang: en-GB -->

<h1 align="center">
  <br>
  <img src="dist/Orchid.png" alt="Orchid" width="200">
  <br>
  <b>Orchid</b>
  <br>
</h1>

<h4 align="center">A very experimental Switch 2 emulator. Orchid started from
<a href="https://github.com/zortos293/Eglantine">Eglantine</a> and has grown into its own project, with fixes aimed at
Mario Tennis Fever first, plus the shader, kernel and graphics work other games need.</h4>

> [!NOTE]
> Orchid is an independent project. Problems with these builds are not caused by the Eglantine
> or Eden developers, so please do not report them there.

<p align="center">
  <a href="#branches-and-builds">Branches and builds</a> |
  <a href="#what-orchid-adds">What Orchid adds</a> |
  <a href="#download">Download</a> |
  <a href="#build-the-newest-commit-yourself">Build it yourself</a> |
  <a href="#documentation">Documentation</a> |
  <a href="#license">License</a>
</p>

## Branches and builds

| Branch | Channel | Release name |
|---|---|---|
| `main` | stable | `v0.<minor>.<patch>`, regular release |

`.github/version` sets the minor version; CI counts the patch up. In 0.5 all branches were merged
into `main` (formerly `mario-tennis-fever`), including the experimental `mario-tennis-fever-perf`.

Commits land often, but they do not start a build. Release builds are started by hand
(Actions > release-binaries > Run workflow), so the newest release can be several commits behind
its branch. To run the newest commit, [build it yourself](#build-the-newest-commit-yourself).

## What Orchid adds

Compared with the Eglantine it started from:

- **Mario Tennis Fever:** startup crash fixed, the NVIDIA crash at match start fixed, and per-game
  defaults (low GPU accuracy, balanced fences).
- **Shaders:** many more Ampere instructions lowered (IMAD.WIDE, IADD3 carries, LEA.HI, F2I.U64,
  I2F from 64-bit pairs, bindless TEX modes and others), mainly from the Fire Emblem: Fortune's
  Weave bring-up.
- **System:** Switch 2 built-in fonts served from their own shared memory; kernel thread start and
  finalize races fixed.
- **Graphics:** query reports flushed before MME DMA reads, layered cross-format texture copies,
  separate surface and texture descriptor buffers, geometry shaders and framebuffers kept within
  device limits.
- **Experimental branch only:** a performance pass (buffer cache, puller, memory trap and kernel
  locking changes). Faster on some systems, less tested.
- **Updates:** release builds check this repository for updates and only offer builds of their
  own channel.

## Download

Releases are on the [releases page](https://github.com/MythosOfs/Orchid/releases). Each one has:

| File | For |
|---|---|
| `orchid-<version>-linux-x86_64-pgo.AppImage` | Linux, profile-guided build (preferred) |
| `orchid-<version>-linux-x86_64.AppImage` | Linux |
| `orchid-<version>-windows-x86_64-pgo.zip` | Windows, profile-guided MinGW build (preferred) |
| `orchid-<version>-windows-x86_64.zip` | Windows |
| `orchid-<version>-macos-arm64.zip` | macOS 15+ on Apple Silicon, experimental |

Linux: `chmod +x` the AppImage and run it; it needs glibc 2.39 or newer. Windows: extract the zip
and run `orchid.exe`.

## Build the newest commit yourself

Linux. Install the dependencies once:

```sh
# Arch / CachyOS
sudo pacman -Syu --needed base-devel boost catch2 cmake enet ffmpeg fmt git glslang libzip lz4 \
  ninja nlohmann-json openssl opus qt6-base qt6-multimedia qt6-charts qt6-wayland sdl3 zlib zstd \
  zip unzip vulkan-headers vulkan-utility-libraries libusb spirv-tools spirv-headers
```

Ubuntu 26.04, Debian 13 and other distributions: see [docs/Deps.md](docs/Deps.md).

Clone `main`, configure and build:

```sh
git clone https://github.com/MythosOfs/Orchid.git
cd Orchid
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DYUZU_TESTS=OFF \
  -DVulkanHeaders_FORCE_BUNDLED=ON -DVulkanUtilityLibraries_FORCE_BUNDLED=ON
cmake --build build
./build/bin/orchid
```

Later, to move to the newest commit (only changed files get rebuilt):

```sh
git pull
cmake --build build
```

The first configure downloads further dependencies into `.cache/cpm`; the two Vulkan flags use the
bundled headers, as the release builds do, so a newer system Vulkan package cannot break the build. A self-built binary has no
update checker; `git pull` is the update. Windows (MSVC) and macOS builds are described in the
[build guide](docs/Build.md).

## Documentation

- [User handbook](docs/user/README.md)
- [Build guide](docs/Build.md) and [dependencies](docs/Deps.md)
- [Fire Emblem: Fortune's Weave bring-up notes](docs/FE_FORTUNES_WEAVE.md)
- [Switch 2 loading notes](NX2_NOTES.md)
- [Development tools](tools)
- [Fork scan: taking fixes from Eglantine forks](docs/ForkScan.md)

## Credits

Orchid takes fixes from Eglantine and its forks; after every release
[tools/fork-scan.sh](docs/ForkScan.md) lists what they fixed since. Each ported commit names its
source and author in its message.

- [zortos293/Eglantine](https://github.com/zortos293/Eglantine) (Zortos), the project Orchid
  started from: paged scalar guest memory access, SPSCQueue value lifetime, timed Event waits on
  Windows, GPU teardown order, buffer cache and query fixes, the separate pipeline cache file names.
  Radu Ursache: no mesh pipelines without `VK_EXT_mesh_shader`.
- [Djihads80/Eglantine](https://github.com/Djihads80/Eglantine) and its contributors: merged
  regularly. Idle staging buffers freed (Takoroka), the RangeSet empty-query guard (néo Luigi),
  test fixes (kraibse).
- [Selevet Eglantine](https://github.com/sylvieistaken/selevet-eglantine) (sylvieistaken): the
  windowed GPU mapping table diff, hot loop shortcuts, F2I, local spill and texture query lowering,
  device memory rebuild on layout changes, decoder end of stream, Windows commit retries, and the
  GPU wait knobs (`EDEN_NX2_GPU_WAIT_SPIN_US`, `EDEN_NX2_BACK_SPIN`, `EDEN_NX2_ACQUIRE_QUEUED`,
  `EDEN_NX2_SWAP_STABLE`, `EDEN_NX2_AVX512`).
- [PokeDinner](https://git.eden-emu.dev/Franseven/PokeDinner-a-Eglantine-Fork) (Franseven,
  [mirror](https://github.com/Apollo422/PokeDinner)), partly from Eden-NS2: any cache hint on
  native LDG, SUST with a partial channel mask, `EDEN_AUTO_BUTTONS`.
- Forks of those: Dee1, Greencus and exploitizm (audio manager lifetime, macro binds past the
  start table), LucyferDev (A64 saturating shift and multiply-accumulate instructions), FZout (page
  table block ids, game list titles), sagaopc (NUP directory cycles), Quixomatic (eden-cli, Switch 2
  module names), YLTai (SDL motion after reconnect), Aidan Kinzett and oledz-i (NUP detection,
  geometry stream 0), NicholasBly and ominixisboss (dynarmic), kipavy (storage CAS loop),
  adamhahnHU and arisqz (sparse layered images), Pavel Barabanov and andrewleeog (F2I bits, host1x
  syncpoint actions), MFS-code (Switch 2 GOB order), CarlosKeese and Dennys Kaluzny Pereira
  (lowering), failaip12 (memory manager bounds).
- rdenno: the Mario Tennis Fever fixes in 0.5.1.

## License

GPLv3 or any later version, see [LICENSE.txt](LICENSE.txt). Started from
[Eglantine](https://github.com/zortos293/Eglantine), which builds on
[Eden](https://git.eden-emu.dev/eden-emu/eden), yuzu and Sudachi.
