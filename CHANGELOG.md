# 📜 Patch Notes: SimonEngine2 // CHUCK SPECIAL EDITION

All notable changes, written the way a live-service game would write them.

---

## v2.0.69-PRO: "The Glow Up" Season Update

> *"We heard your feedback: the robot was a flat blue rectangle. We have taken that personally."*
> — the Chuck Edition dev team (Chuck)

### 🆕 NEW MAP: `de_chuckworld`
- A fully playable third-person arena. Walk, jump, platform, collect 10 power cores per round.
- New playable character **ChuckBot**: gold armor, purple limbs, a blinking antenna, real walk cycle.
- COMBO multiplier for pickups within 3 seconds. Speed bonus for rounds under a minute.
- VICTORY ROYALE screen, best-time tracking, instant restart (`R`).
- Third-person camera with follow lag, orbit, zoom and wall collision.
- Floating `+100` popups, a `[PRO] CHUCK` nametag, and a waypoint to the nearest core.

### 🗺️ Map rotation
- `TAB` cycles `de_robot` → `de_chuckworld` → `de_teapot` → `de_magiccube` with a neon loading wipe.
- The teapot and magic cube maps were in the files the whole time. Now they're in the rotation.

### 💡 Graphics
- **RAY TRACING** (F7). Patch note lawyers require us to clarify: three directional studio lights.
  Key light + cyan rim + magenta kick, all following the camera. Massive visual improvement anyway.
- Every primitive now has surface normals. The `.obj` loader builds smooth vertex normals.
- Neon edge outlines on everything (F6), done with zero shaders and one very sneaky 1D texture.
- Synthwave sunset backdrop with procedural skyline, stars, and embers (F10).
- Tron floor with scrolling grid, spawn pad, shockwave rings and contact shadows.
- The 2D horizon now lines up with the 3D camera's real horizon in perspective maps.
- 6 RGB themes (F2) with live crossfades: SYNTHWAVE, VENOM, INFERNO, SUBZERO, VAPORWAVE,
  RGB OVERDRIVE. Plus one we're not allowed to talk about.

### 📊 HUD
- Live FPS, frame time, 1% lows and a frame-time graph (all real).
- Telemetry panel with an RTX 9090 Ti SUPER and 420 GB of VRAM (none of it real).
- **FRAME GEN x4**. Quadruples your FPS. On the HUD.
- Map, object count, camera, theme and session clock (real again, we promise).
- CHUCK vs BUGS esports scoreboard with a LIVE indicator.
- XP bar, levels, and ranks from BRONZE to GRAND CHAMPION.

### 🏆 Progression
- 18 achievements with Xbox-style toasts. One of them is worth 9001 XP.
- Neon callouts for every mode change, queued so they never stomp each other.
- Killfeed. Some of it even happened.

### 🎬 Quality of life
- Boot splash with a fully honest boot log (it admits it skipped the shaders).
- F1 controls sheet. F5 cinematic mode. F8 CRT mode. F9 screenshots (`.bmp`). F11 fullscreen.
  F12 photo mode. Right-drag orbit and scroll zoom on the homework maps.
- Console now opens with ASCII art. As is tradition.

### 🐛 Bug fixes
- Fixed F4 actually being F1. (Special-key queries used the raw GLUT code against a remapped table.)
- Fixed quick key taps getting dropped if they landed between two update ticks.
- Fixed X11 key auto-repeat making held keys flicker.
- Fixed `Robot::setColor` ignoring all four limbs since the Limb refactor.
- Renamed Sphere's `maxStacks` to `minStacks`, because it is a minimum.
- Fixed the game camera spawning inside a pillar. (It looked like modern art. It was a bug.)

### 🔧 Under the hood
- New CMake build for Linux / anywhere with freeglut. The Visual Studio solution still works.
- GitHub Actions CI on Linux + Windows, with a headless boot-and-screenshot smoke test.
- `.editorconfig`, `.clang-format`, VS Code build/debug configs and recommended extensions.
- Issue and PR templates.
- Everything stays C++14 + OpenGL 1.1, so it still builds with the stock MSVC toolchain.

### ⚠️ Known issues
- The teapot is very bright in outline mode. It's a hologram now. That's the feature.
- Simon's bugs: still 87% patched.

---

## v2.0.0: "Homework 2" (Simon)
- Limbs, the `Limb` class, glad, static freeglut. The foundation all of the above stands on.

## v1.0.0: "SimonEngine but 3D!!!!!!!" (Simon)
- The original engine, scenes, primitives, mesh registry, `.obj` loading, and the robot.
