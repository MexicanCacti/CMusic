# CMusic — Qt6 Audio Visualizer

## Overview

CMusic is a Qt6-based audio visualizer that performs real-time FFT analysis on audio playback and feeds the results into a fragment shader for rendering.

Application Workflow:

* Plays local audio file using Qt Multimedia
* Extracts decoded audio buffers during playback
* Performs real-time FFT using **KISS FFT**
* Reduces spectral data into a fixed set of bins (default: 64)
* Uploads this data to a GPU shader for visualization

Rendering is done using OpenGL via `QOpenGLWidget`.

---

## Features

* Real-time audio playback + visualization
* Playback-synchronized FFT analysis
* Custom fragment shader support (hot-swappable at runtime)
* GPU-driven rendering pipeline
* Lightweight FFT dependency

---

## Dependencies

### Required

* **Qt 6**

  * Core
  * Widgets
  * Multimedia
  * OpenGLWidgets

* **CMake ≥ 3.29**

* **C++20 compatible compiler**

### Included / Vendored

* **KISS FFT** (no external install required)

### Notes

* Qt Multimedia must support the **FFmpeg backend** for `QAudioBufferOutput`
* On Windows/macOS, Qt deployment tools typically bundle runtime dependencies
* On Linux, ensure FFmpeg backend is available

---

## Build Instructions

### 1. Clone the repository

```
git clone https://github.com/MexicanCacti/CMusic.git
cd CMusic
```

### 2. Configure with CMake

```
cmake -S . -B build
```

If Qt is not auto-detected:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/compiler
```

### 3. Build

```
cmake --build build
```

### 4. Run

```
./build/CMusic
```

---

## Shader System

The visual output is controlled entirely by fragment shaders.

You can:

* Use the default shader
* Load your own `.frag` file at runtime

### Shader Location

Recommended directory:

```
src/resources/fragmentShaders/
```

---

## Shader Uniforms

### `u_resolution`

```glsl
uniform vec2 u_resolution;
```

Window size in pixels.

---

### `u_time`

```glsl
uniform float u_time;
```

Continuous playback time in seconds.

---

### `u_level`

```glsl
uniform float u_level;
```

Normalized volume (0.0 → 1.0).

---

### `u_playing`

```glsl
uniform float u_playing;
```

* `1.0` → playing
* `0.0` → paused

---

### `u_fft`

```glsl
uniform float u_fft[64];
```

FFT spectrum data:

* 64 frequency bins
* `0` = lowest frequencies (bass)
* `63` = highest frequencies

---

## Basic Shader Example

```glsl
#version 330 core

out vec4 FragColor;

uniform vec2 u_resolution;
uniform float u_fft[64];

void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;

    int bin = int(uv.x * 64.0);
    bin = clamp(bin, 0, 63);

    float value = u_fft[bin];

    float bar = step(uv.y, value);

    FragColor = vec4(vec3(bar * value), 1.0);
}
```

---

## Future Improvements

* Do something with file metadata in the shaders
* Pop out the visualizer window
* Allow the queuing up of multiple audio files
* Allow the playing of all audio files detected in a folder

