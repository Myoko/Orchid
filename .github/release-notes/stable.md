## What's new in 0.6.0

0.6.0 is about the two games this fork cares about most, Mario Tennis Fever and Hyrule Warriors:
Age of Imprisonment, plus fixes for other Switch 2 exclusives.

### Mario Tennis Fever

- **No more stray shadow spots:** dark blobs lay on floors and courts, most of all in Adventure
  mode, and moved with the camera instead of with anything that casts a shadow. The game draws its
  four shadow cascades in one pass and asks the GPU (register 0x11F0) to put each cascade into its
  own layer of the shadow map; that request was ignored, so every cascade landed in the first
  layer. Fixed on AMD and NVIDIA alike, at the same frame rate. Far shadows that were missing
  (the lobby's stairs, for example) are drawn now too.
- **Grass courts have their grass blades again:** the turf is drawn with tessellation shaders,
  which were rejected, so the grass court rendered flat.

### Hyrule Warriors: Age of Imprisonment

- **No freeze at the end of a movie:** the game froze on the last frame of any movie that played
  to its end (the opening movie, and story movies later on). The video decoder now hands the
  player's frames back after the end of the stream as the game expects.
- Logs stay small: a graphics command the game sends about 50 times a second filled most of the
  log; it is logged once now.

### Other games

- **Donkey Kong Bananza:** the intro after New Game is no longer a flat lavender screen (its
  upscaler reads and writes the frame through other slots). Gameplay still renders black.
- **Mario Kart World:** a possible crash in a race ("access violation" in the syncpoint manager,
  Djihads80/Eglantine#74) is guarded against: a syncpoint id past the table is dropped and logged.
- **Welcome Tour and Mario Kart World:** the Switch 2 scale, chat and camera commands they send at
  boot are answered, so they log no unimplemented commands at startup.

### Tested on this build

Headless on Linux with an AMD GPU (RADV):

- Mario Tennis Fever: the Adventure lobby and a grass-court match at 1x and 2x resolution, 60 fps,
  no stray shadows.
- Hyrule Warriors: the opening movie into the cutscene after it, also when skipping with A.
- All 19 games in the test set boot to their title or menu, except Super Mario Galaxy 2 and the
  Duskbloods network test (see below).

### Known issues

- **Super Mario Galaxy 2 (Switch 2 version)** closes about 5 seconds after start: the game's own
  Vulkan driver fails to create its device. The same happens in 0.5.1.
- **Donkey Kong Bananza:** gameplay still renders black.
- The Duskbloods network test isn't part of this release.
