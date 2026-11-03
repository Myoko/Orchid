## What's new in 0.5.1

0.5.1 brings in the main Eglantine project (Djihads80/Eglantine, 261 commits since build 104) and
rdenno's fixes on top of it, merged with this fork's Mario Tennis Fever and Hyrule Warriors work.
It is a big merge: if a game runs worse than in 0.5.0, please report it and keep 0.5.0 around.

### Faster -pgo builds

- The `-pgo` downloads are now built against Eglantine's own profile instead of Eden's Switch 1
  profile. With Eden's, Mario Kart World ran at about half the speed of the normal build;
  if you switched away from the `-pgo` build because it was slower, try it again.

### Mario Tennis Fever (from rdenno)

- **Scenery around the court is drawn again:** the game samples its depth buffer in a format that
  wasn't known (X8Z24 with UNORM components), so the arch and the castle stadium were missing.
- **No crash right at boot:** the client's core now runs before an HLE service reply wakes it,
  as on hardware. One of the game's worker threads relied on that window.
- Framebuffers use the fewest layers of their attachments (this fork already had that fix).

### Hyrule Warriors: Age of Imprisonment

- A texture buffer at an unmapped address no longer aborts the emulator (rdenno; a crash from a
  user's log).
- Tested again on this build: boot, first-time setup and the opening cutscene at a steady 60 fps,
  no rejected shaders. Combat is still untested.

### From the main Eglantine project

- **Linux and AMD:** BC3 textures no longer flash on RADV, fewer ASTC decode stalls, lower RAM
  use, a buffer cache crash fixed.
- **Memory:** 16 MiB Vulkan memory blocks, a texture cache that keeps within the usable VRAM and
  frees the emptiest blocks first, smaller JIT block tables, unused pipelines and shader modules
  freed.
- **Graphics:** mesh and task shaders, a stand-in upscale for Fire Emblem: Fortune's Weave's DLSS,
  sparse texture fixes, presents cropped to their frame size, RGB10A2 presents.
- **Shaders:** upstream's Ampere translator is kept as a fallback. It runs when this fork's
  translator rejects a program, so forms that games like Splatoon Raiders and Kirby Air Riders use
  are covered too.
- **Video:** AV1 and newer-SDK video decoding.
- **Services:** camera session, more vi2/am/audio/nsd/ssl commands, software keyboard text for
  Pokopia and Fire Emblem.
- **NUP packages:** load NUP containers; Convert to NUP in the Qt app and `eden-cli --pack-nup`.
- Frame generation settings are saved.

### Not taken over, please report if these games got worse

- Upstream's lazy GPU-only memory regions (`EDEN_NX2_GPU_ONLY_LAZY`, a RAM saving): they didn't
  fit this fork's sparse memory design.
- Upstream's decoder image path for video: Fire Emblem's movies use this fork's path.
- The reply to graphics-system command 115 is this fork's, not upstream's.
