# Honda Rush: Career Rider

A 2D side-scrolling motorcycle game written in C++ with the **iGraphics** library.
Ride a Honda through three levels — clear ramps, dodge obstacles, collect coins and fuel,
fight off robbers, and get the passenger to AUST.

> CSE-1200 — Software Development I · Section C2 · Dept. of CSE, AUST

---

## Levels

| Level | Route | What's in it |
|---|---|---|
| **1** | 8 backgrounds | 2 ramps, 4 obstacles, coins, fuel. Early hits only cost speed — from BG5 a crash ends the run. |
| **2** | 10 backgrounds | 5 ramps each paired with an obstacle, coin arcs over the jumps, fuel cans. |
| **3** | 13 backgrounds | Robber fights on foot, cars, tanks, a 360° loop, Volt Speed, a story with dialogue and a timed payment. The world reverses direction halfway through. |

Every level has a fixed layout — same route, same object positions, every run.

## Controls

| Key | Action |
|---|---|
| `D` / `A` | throttle / brake |
| `W` | jump |
| `S` | drop faster while airborne |
| `ENTER` | get on / off the bike (Level 3) |
| `SPACE` | shoot (on foot, Level 3) |
| `V` | Volt Speed (Level 3) |
| `P` · `R` · `ESC` · `M` | pause · restart · menu · mute |

## Cheat codes

Typed during play in Level 3:

| Code | Effect |
|---|---|
| `MACHINEGUNON` | unlocks the gun |
| `GETONTHEBIKE` | the girl boards the bike |
| `GIVEPERMISSION` | clears the army checkpoint |
| `COMPLETEYOURPAYMENTQUICKLY` | pays the semester fee (20s limit) |

## Build & run

1. Open `demo2/demo/demo/demo.sln` in **Visual Studio 2013** (v120 toolset, Win32).
2. Build in Debug, then run from Visual Studio.

The working directory must be the project folder (`demo2/demo/demo/demo/`) so the
relative asset paths resolve. `GLUT32.DLL` sits next to the project file.

Start-up takes around 12 seconds while all three levels' artwork is decoded and uploaded.

## Tech

C++ · iGraphics (OpenGL + GLUT) · `stb_image` for PNG/JPEG · Windows MCI (`winmm`) for audio ·
Visual Studio 2013 · fixed 1280 × 720 window.

The whole game is one translation unit: `iMain.cpp` is the only `.cpp` compiled, and every
system is a header it includes — `Level01.hpp`, `Level02.hpp`, `Level03.hpp`, `Config.h`,
`Audio.hpp`, `Input.hpp`, and the menu screens.

## Team

| ID | Name |
|---|---|
| 00725105101164 | Mujahidul Islam Ansary Niloy |
| 00725105101149 | Foysal Sami |
| 00725105101152 | Parijat Sarker |
