# Image Processing

Small image-processing programs written in C++ with Qt6, implemented from scratch on raw grayscale pixel data. Each program loads an image, processes it and shows the result in a window.

## Projects

### High Frequencies Filter

[`HighFrequenciesFilter/`](HighFrequenciesFilter/)

A frequency-domain low-pass filter:

1. Computes the 2D discrete Fourier transform (DFT) of a grayscale image using [FFTW](https://www.fftw.org/). The spectrum is centered by multiplying each pixel by (-1)<sup>x+y</sup> first.
2. Sets every frequency outside a central box to zero. The box keeps 10% of the spectrum by default (`keep_ratio`).
3. Applies the inverse DFT and normalizes the result, which gives a smoothed, blurred image.

Input: `gray_lenna_small.png` · Report: [`hw1-nevena-angelova.pdf`](HighFrequenciesFilter/hw1-nevena-angelova.pdf)

### Otsu Thresholding

[`AdaptiveOtsu/otsu/`](AdaptiveOtsu/otsu/)

Global binarization with [Otsu's method](https://en.wikipedia.org/wiki/Otsu%27s_method). The program builds a normalized histogram, then picks the threshold *k* that maximizes the between-class variance:

σ²<sub>B</sub>(k) = (m<sub>G</sub>·P<sub>1</sub>(k) − m(k))² / (P<sub>1</sub>(k)·(1 − P<sub>1</sub>(k)))

Pixels below the threshold become black and all others become white.

### Adaptive Otsu Thresholding

[`AdaptiveOtsu/adaptive_otsu/`](AdaptiveOtsu/adaptive_otsu/)

A recursive, region-based variant for images with uneven lighting. For each region:

- The program computes the Otsu threshold and the separability measure η = σ²<sub>B</sub> / σ²<sub>T</sub>.
- If η ≥ `ETA_THRESHOLD` (0.85), the region is thresholded as is.
- Otherwise the region is split into left and right halves, and each half is processed the same way. The splitting stops once a region is smaller than `MIN_SIZE` (32 px).

Input: `AdaptiveOtsu/face.jpg` · Report: [`hw2-nevena-angelova.pdf`](AdaptiveOtsu/hw2-nevena-angelova.pdf)

## Requirements

- C++ compiler (g++)
- Qt6 (Core, Gui, Widgets) and `qmake6`
- FFTW3 (only for the High Frequencies Filter)

On Debian/Ubuntu:

```bash
sudo apt install build-essential qt6-base-dev qt6-base-dev-tools libfftw3-dev
```

## Building and running

Each project builds on its own. Build and run it from its own directory, because the input image path is relative to that directory:

```bash
cd HighFrequenciesFilter
qmake6 && make
./fftw
```

```bash
cd AdaptiveOtsu/otsu
qmake6 && make
./otsu
```

```bash
cd AdaptiveOtsu/adaptive_otsu
qmake6 && make
./adaptive_otsu
```

To use a different image, change the `file_name` constant at the top of the source file. Parameters such as `keep_ratio`, `ETA_THRESHOLD` and `MIN_SIZE` are also constants in the source.

## Notes

- Images are processed as 8-bit grayscale (`QImage::Format_Grayscale8`). The Otsu programs convert their input to this format automatically. The FFT filter requires its input to be grayscale already.
- The FFT filter currently assumes a square input image.
