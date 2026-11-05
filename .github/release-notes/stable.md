## What's new in 0.7.1

0.7.1 fixes the crashes and hangs of five Switch 2 titles from the 0.7.0 known issues, adds Switch 2
sound mixer audio and brings in a second round of fixes from Eglantine forks. Each ported fix names
its source and author in its commit, and the README lists the forks under Credits.

### Games

- **Fast Fusion:** no more hang when a race loads, and the ground and sky render.
- **Kirby Air Riders:** no abort after the intro movie and no GPU hang when a race loads.
- **Star Fox:** cutscenes render, and the depth-stencil reinterpret crash is gone.
- **Orbitals:** no more abort while waiting for a frame.
- **Drag x Drive:** the controller dialog no longer loops, and a Joy-Con used as a mouse reports a
  resting pose.
- **Yoshi and the Mysterious Book:** surface atomics, buffer loads and 3D surface loads are lowered,
  and programs that were rejected earlier are checked again.
- **Ocarina of Time:** boots past its 64-bit WaitIfEqual arbitration, and its first shaders
  (ISETP.EX with an input predicate, UIMAD.WIDE.U32) are lowered.

### Audio

- Switch 2 sound mixer streams play through the host.

### Shaders and graphics

- More instructions lowered: native BRA.U, register-handle formatted image loads and stores,
  uniform shifts with an immediate high word, H0_NH1 half sources, IMAD.X and IMAD.WIDE.U32 forms,
  signed I2F.F32.S64 and FSET with a uniform source. Global memory helpers no longer alias
  descriptors, and global atomic adds go through them.
- Vulkan: an out-of-date swapchain is recreated instead of ending the game, a bad saved device
  index falls back to a real GPU, tessellation pipelines the driver would refuse are skipped, task
  and mesh stages get the push constant range, and Mesa ANV keeps conditional barriers.
- Clears are clipped to the framebuffer's layers, storage image views are cached per format, a 2D
  blit with an undecodable render target format is skipped, and fence downloads track their ranges
  separately.
- Switch 2 video decoding detects H.264 packet framing per stream, sets the scaler's colour details
  for every frame and handles the end of the stream.

### System

- Loose base dumps whose data is in an update boot, NUP files with a contentless ExeFS or RomFS
  load, and the NUP publisher and source dump names are read.
- `ssl:s` returns a system SSL context, the SSL connection keeps its socket, and socket receive and
  send timeouts reach the host as the guest gives them.
- The standard shared font carries the controller button glyphs.
- **Help > Report a Crash on GitHub** opens a bug report, and the issue templates and the log guide
  name Orchid.

### Known issues

- See the performance and compatibility table in the README. Ocarina of Time is still early: more
  of its shaders are being lowered for the next release.

The PGO profile is the same as in 0.6.2.
