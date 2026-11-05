## What's new in 0.7.0

0.7.0 brings Star Fox to its first rendered frames, recommended settings for four Switch 2 titles,
Donkey Kong Bananza speed work and about 50 fixes from Eglantine forks. Each ported fix names its
source and author in its commit, and the README lists the forks under Credits. The README also has
a new performance and compatibility table.

### Games

- **Recommended settings:** Donkey Kong Bananza, Hyrule Warriors: Age of Imprisonment and Mario
  Tennis Fever now ask at boot, like Mario Kart World already did. Accepted settings apply to that
  game until Orchid is closed; your saved settings are not changed.
- **Donkey Kong Bananza:** runs with GPU accuracy low, no reactive flushing and the Boost GPU clock
  by default, and gets a built-in 30 FPS patch option. The buffer cache skips vertex buffer and
  page checks for ranges the CPU has not written since the last check, and repeated written ranges
  (`EDEN_NX2_DRAW_TUNING=0` turns this off). The indirect draw reset program is found by hash.
- **Star Fox:** boots to steady presents (window alignment, the hwacod audio codec, uniform U16
  I2F, ULDC 8 and 16-bit widths). It still crashes soon.
- **Hyrule Warriors:** no more device loss right after skipping a movie on drivers without
  `nullDescriptor` (the indirect quad index buffer grows before vertex buffers are bound).

### Graphics

- Buffer cache, texture cache and staging buffer guards from the forks: no joins through stale page
  table entries, no binds of deleted storage segments, clamped oversized vertex streams, zero-extent
  render targets rejected, descriptor payloads that grow when a frame overflows them.
- The GPU split thread recovers from a lost wake-up, and command list snapshots survive semaphore
  waits.
- The pipeline cache starts empty when the driver rejects the file on disk, presenting continues
  after the surface cannot be recreated, and the stream buffer fits small BAR heaps.
- A draw that samples its own colour target reads a copy, which fixes the tiled Donkey Kong
  Bananza tutorial video on RADV; alias walks are skipped when no alias is newer.
- Switch 2 video decoding converts YUV420P into NV12 directly, with full-range colour and odd
  widths handled.

### System

- Data races in timing, perf stats, file logging, audio and SDL input fixed, plus undefined
  behaviour found by UBSan.
- Mod folders that do not exist no longer break ExeFS and LayeredFS patching, the cheat engine stops
  before the services shut down, and the system SSL service returns the built-in certificates.
- `EDEN_NX2_PRESENT_STATS=<n>` sets how many presents the present stats line averages.

### Known issues

- See the performance and compatibility table in the README. Yoshi and the Mysterious Book renders
  a black screen in gameplay, Orbitals does not launch, Kirby Air Riders and Fast Fusion crash on
  loading a race, and Drag x Drive has no controller support yet.

The PGO profile is the same as in 0.6.2.
