# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

The build directory is `build/`.

```bash
# Configure (first time)
cmake -S . -B build

# Build
cmake --build build

# Run (output is PPM to stdout, progress to stderr)
build/rt/rt > output.ppm
```

There are no tests currently — the test scaffolding in `core/CMakeLists.txt` is commented out.

## Architecture

This is a CPU ray tracer following the _Ray Tracing in One Weekend_ series, structured as two CMake targets:

- **`core` (static lib)** — all ray tracing primitives: `Vec3`, `Ray`, `Interval`, `AABB`, `Hittable`, `Material`, `Texture`, `Camera`, `BVH`, `ThreadPool`. Headers in `core/inc/`, minimal `.cpp` files in `core/src/` (only `color.cpp`, `interval.cpp`, `aabb.cpp` have non-inline implementations).
- **`rt` (executable + static lib)** — scene definitions (`rt/src/rt.cpp`) and `main.cpp`. The lib/exe split exists so scenes can eventually be tested independently.

### Key design patterns

**Everything is header-heavy.** Most logic lives in `core/inc/*.hpp`. This is intentional — the classes are inline-heavy enough that splitting would require explicit instantiation.

**`Hittable` is the core abstraction.** Anything that can be intersected by a ray inherits from it and implements `hit()` and `bounding_box()`. The BVH (`bvh.hpp`) is itself a `Hittable` that wraps a `HittableList`, enabling recursive acceleration structure traversal.

**`Color` is a type alias for `Vec3`** (`using Color = Vec3`). There is no type safety between colors and directions.

**`Sphere` uses a `Ray` to represent its center** — a stationary sphere has a zero-direction ray, a moving sphere has a direction equal to `center2 - center1`. `m_center.at(ray.time())` gives the center at any point in time.

**Rendering is multithreaded by row.** `Camera::render()` submits one job per scanline to a `ThreadPool`. Results are written into `m_image_matrix` (a flat `vector<Color>`), then flushed to stdout as PPM after all threads join. The RNG functions in `constants.hpp` use `thread_local` generators to avoid data races.

**Output format is PPM** (P3 text format) written to stdout. Redirect to a file to save: `build/rt/rt > out.ppm`.

### Adding a new scene

1. Add scene and camera factory functions in `rt/src/rt.cpp`
2. Declare them in `rt/inc/rt.hpp`
3. Call them from `main.cpp`
