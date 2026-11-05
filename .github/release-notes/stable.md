> [!IMPORTANT]
> ## Report crashes from inside Orchid
> If a game crashes, start Orchid again and pick **Help > Report a Crash on GitHub**. It opens a bug report with your build, system, GPU and driver and the end of the crash log already filled in, and opens the log folder so you can attach the full log of the crashed run. Please use it for every crash: reports with a log get fixed much faster, and reports without one are closed.

## What's new in 0.7.3

0.7.3 is a small fix release for Ocarina of Time: its lighting is no longer overexposed. Nothing
else is new. Two of the fixes sit in shared code (shader translation and uniform uploads), so the
shader cache of every game is rebuilt once on first boot. The fixes are ported from
zortos293/Eglantine and name their authors in their commits.

### Ocarina of Time

- **Exposure and haze:** outdoor scenes are no longer washed out. Orchid scales the game's exposure
  constant before its filmic curve and thins the outdoor fog. This is a visual workaround while
  the root cause is still open; set `EDEN_NX2_OOT_VISUAL_WORKAROUND=0` to turn it off.
- **Sky ambient light:** the compute pass that gathers the sky's ambient light now gets its buffer,
  so rocks and shadows in Kokiri Forest are lit by the sky instead of looking yellow (Duck912).
- The 64 KiB uniform bank of native programs is uploaded again once per frame, so constants the
  memory tracker misses no longer go stale (kraibse, from Franseven's port).

### Known issues

- **Ocarina of Time:** grass, bushes and some trees can still be missing, and the fog workaround
  also thins the fog in interiors. Missing vegetation is the next fix.
- **Mario Tennis Fever on NVIDIA:** the GPU can be lost when a match starts (Windows, RTX 30
  series). We are still looking for the cause; until then, try switching the console mode to
  **Docked** and tell us in issue #14 whether that helps.
- See the performance and compatibility table in the README.

The PGO profile is the same as in 0.6.2.
