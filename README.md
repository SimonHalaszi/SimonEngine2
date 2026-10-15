# SimonEngine2
SimonEngine but 3D!!!!!!!

## Visual Studio build

Open `SimonEngine2.sln` in Visual Studio 2022 with the Desktop development
with C++ workload and Windows 10 SDK installed.

Build freeglut separately as a **static library** for the same platform and
configuration as the engine, using the DLL CRT (`/MDd` for Debug, `/MD` for
Release). Set its output target names to:

| Configuration | Library filename |
| --- | --- |
| Debug | `freeglut_staticd.lib` |
| Release | `freeglut_static.lib` |

Place the libraries in `SimonEngine2/lib/x64/` or `SimonEngine2/lib/Win32/`
for the corresponding engine platform (the solution calls Win32 **x86**).
`SimonEngine2/lib/` is also searched as a fallback for a single-platform setup.
Do not rename a Release library to use it in Debug or mix x64 and Win32 binaries.
The bundled `freeglut.lib` is an old DLL import library and is not used by this
static setup; `freeglut.dll` is not required.

All four project configurations define `FREEGLUT_STATIC` project-wide. The
bundled header selects the static library name above and the required Windows
OpenGL libraries automatically; no per-source defines or linker edits are needed.
Intermediates are isolated under
`SimonEngine2/obj/SimonEngine2/<Platform>/<Configuration>/`. After switching
from the old setup, use **Clean Solution**, then **Rebuild Solution**.
