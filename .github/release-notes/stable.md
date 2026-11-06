> [!IMPORTANT]
> ## Report crashes from inside Orchid
> If a game crashes, start Orchid again and pick **Help > Report a Crash on GitHub**. It opens a bug report with your build, system, GPU and driver and the end of the crash log already filled in, and opens the log folder so you can attach the full log of the crashed run. Please use it for every crash: reports with a log get fixed much faster, and reports without one are closed.

## What's new in 0.7.5

0.7.5 adds shader and buffer safety corrections, more native instruction support, and AV1
decoding to the Windows PGO package. The fixes ported from Eglantine and its forks retain their
source credits in the commits.

### Graphics and shader stability

- Shader metadata invalidated by the CPU stays alive until renderer bindings have been cleared,
  avoiding stale ShaderInfo pointers during shader refresh.
- Live texture descriptor reads are checked against the pool bounds before guest memory is read.
- Native compute buffers use their mapped extent, including supported fragmented mappings,
  rather than treating a neighbouring shader parameter as a buffer size.
- Routed native loads, stores and atomics check the complete descriptor range before narrowing
  their offsets to 32 bits.
- Native 32-bit compare-and-swap atomics and additional carry, shift and reduction instruction
  forms are supported. High data registers and uniform reduction addresses are preserved.
- Signed-add overflow handling and AMD demote reordering around nested returns are corrected.
- Texture uploads can be deferred when a render-target clear will overwrite them; scissor
  checks preserve uploads needed outside the cleared area.

### Windows video decoding

- The Windows PGO build now ships FFmpeg with an AV1 decoder. CI checks that the decoder opens
  and includes its runtime DLL dependencies. This addresses the missing-decoder error reported
  with Ocarina of Time videos in issue #28.

### System and interface

- Long event-wait deadlines saturate instead of overflowing on non-Windows hosts.
- Stored callbacks own their callable objects, avoiding dangling references to the source.
- The Ocarina of Time 30 FPS preset states that its built-in patch supports specific game builds.

### Confirmed from 0.7.4

- Arionic confirmed that the Kirby Air Riders title-screen crash is fixed in 0.7.4; issue #19 is
  closed. Its Vulkan texture/query corrections remain included in 0.7.5.
- The reporter of Mario Tennis Fever issue #14 confirmed that matches load without crashing
  on 0.7.4 in both uniform-buffer-reuse test configurations; that issue is also closed.

### Known issues

- **Kirby Air Riders:** characters and lighting can still render incorrectly, and some compute
  shaders are still unsupported.
- **DLSS ghosting:** the native buffer changes address an identified sizing problem; the reported
  ghosting cause and a game-level fix remain unconfirmed (issue #27).
- **Ocarina of Time video backgrounds:** the AV1 packaging correction does not establish the
  cause of every black-background report (issue #28).
- **Ocarina of Time:** the exposure/fog adjustment remains a visual workaround and also thins fog
  in interiors. `EDEN_NX2_OOT_VISUAL_WORKAROUND=0` disables it.
- See the performance and compatibility table in the README.

The PGO profile is the same as in 0.6.2.
