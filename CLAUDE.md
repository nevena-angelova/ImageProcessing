# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

A collection of independent image-processing homework programs in C++ with Qt6 (`QImage` for pixel access, `QLabel` to display the result). Each program is a self-contained single-file qmake project; there is no top-level build, no shared library code, and no test suite. The `.pdf`/`.odt` files are the written homework reports.

| Project | Source | What it does |
|---|---|---|
| `HighFrequenciesFilter/` | `main.cpp` (target `fftw`) | 2D DFT via FFTW, low-pass filter (keeps a centered box of `keep_ratio` of the spectrum), inverse DFT |
| `AdaptiveOtsu/otsu/` | `otsu.cpp` | Global Otsu thresholding from a normalized histogram |
| `AdaptiveOtsu/adaptive_otsu/` | `adaptive_otsu.cpp` | Recursive Otsu: thresholds a region if its separability η = σ²_B/σ²_T ≥ `ETA_THRESHOLD`, otherwise splits it in half by width and recurses (stops below `MIN_SIZE`) |

## Build and run

Dependencies: Qt6 (widgets), and `libfftw3` for `HighFrequenciesFilter`. Build each project from its own directory:

```bash
cd AdaptiveOtsu/otsu && qmake6 && make && ./otsu
```

Regenerate the Makefile with `qmake6` (or `qmake`) after editing a `.pro` file; the committed Makefiles have Qt6 paths hardcoded for x86_64 Linux.

**Run from the project's directory** — input images are loaded via a hardcoded relative `file_name` constant at the top of each source file (`gray_lenna_small.png` for the FFT filter, `../face.jpg` for both Otsu programs). If the load fails, the program prints "Cannot load image" (or, for `adaptive_otsu`, silently exits).

Build outputs (`Makefile`, `.qmake.stash`, `*.o`, and the binaries) are committed to git; avoid committing rebuilt artifacts unless intended.

## Code conventions and gotchas

- Pixels are accessed directly via `image.bits() + row * bytesPerLine()` or `scanLine()`, so images must be `QImage::Format_Grayscale8`. The Otsu programs convert to that format; `HighFrequenciesFilter` silently does nothing for non-grayscale input.
- The FFT filter centers the spectrum by multiplying input pixels by (-1)^(r+c) before the forward transform and undoes it after the inverse; FFTW's backward transform is unnormalized, so output is divided by `m*n`.
- In `HighFrequenciesFilter/main.cpp` the result image is constructed as `QImage(image.height(), image.width(), ...)` (width/height swapped) — correct only for square inputs.
- Algorithm parameters (`keep_ratio`, `ETA_THRESHOLD`, `MIN_SIZE`) are hardcoded constants; there is no CLI argument parsing.
- Some comments in `adaptive_otsu.cpp` are in Bulgarian.
