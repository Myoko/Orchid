## What's new in 0.6.3

0.6.3 brings in fixes that Eglantine and its forks made and Orchid did not have yet. Each one names
its source and author in its commit, and the README lists them under Credits. From now on, every
release is followed by a scan of those forks, and the next release starts with their fixes.

### CPU

- **New A64 instructions:** SQRSHL, UQRSHL, SQDMLAL, SQDMLSL and their scalar forms used to be
  skipped with a stale result. Switch 2 system code and several games (Fire Emblem: Fortune's
  Weave above all) use them.
- Guest memory reads and writes that cross a 4 KiB page are split correctly, and page table block
  ids no longer wrap after 65535 mappings.

### Shaders and graphics

- More Ampere instructions lowered: byte and half local spills, constant-buffer FRND and F2I
  (including F2I to 16 bits and its negate and absolute bits), constant LEA, DEPBAR wait lists,
  texture queries with three or four outputs, any cache hint on LDG and SUST with a partial mask.
- Geometry stream 0, storage compare-and-swap loops, sparse layered images, ASTC decode images and
  the Switch 2 tiling order in the software blitter fixed.
- No task and mesh pipelines on GPUs without `VK_EXT_mesh_shader`; quad control only together with
  maximal reconvergence; no occlusion query for a deferred clear.
- Idle staging buffers are freed, and the pipeline cache uses its own file names, so other builds
  no longer delete it.
- New opt-in knobs for GPU waits and presents: `EDEN_NX2_GPU_WAIT_SPIN_US`, `EDEN_NX2_BACK_SPIN`,
  `EDEN_NX2_ACQUIRE_QUEUED`, `EDEN_NX2_SWAP_STABLE` and `EDEN_NX2_AVX512=0`. The defaults are
  unchanged.

### System

- Switch 2 updates (NUP) are found from the ExeFS that boots, and listed only when they can boot.
- Audio sessions keep their managers alive, SDL motion keeps working after a controller
  reconnect, and the game list shows the file name for games without a title.
- Teardown order, timed waits on Windows and guest memory commits while Windows grows its
  pagefile fixed.

The PGO profile is the same as in 0.6.2.

### Known issues

- **Donkey Kong Bananza:** black gameplay on RADV; busy late areas run slowly.
- **Super Mario Galaxy 2 (Switch 2 version)** closes about 5 seconds after start (its own Vulkan
  driver fails to create its device), as in 0.6.0.
