# Simple Computer Graphics (SCG)

My personal single file header library for quick and simple cross-platform graphics applications. It is currently in a WIP progress state, so not really usable for others. But feel free to take any code from it if it's useful.

## Setup

The library requires the SDL2.

### OSX

```sh
brew install sdl2
```

### Linux

```sh
sudo apt-get install libsdl2-dev
```

Or, if on another distribution, use the package manager available.

## Building the examples

All the examples can be built with [Make](https://www.gnu.org/software/make/).

Build all examples:

```sh
make
```

Build a single example:

```sh
make {{example_name}}
```

Replace `{{example_name}}` with any of the following:

* `basic`

## Examples

| Basic         |
| ------------- |
| ![basic](/previews/basic.png) |
| [Source](/examples/basic.c) |
| Opens a screen, plots some pixels and plays some music. |

## Converting audio

Ensure `ffmpeg` is installed. Then run:

```sh
ffmpeg -i assets/{example_sound}.wav -acodec pcm_s16le -ac 2 -ar 48000 assets/{example_sound_output}.wav
```

## Attributions / References

- Music in the basic example is by [joshuaempyre](https://freesound.org/people/joshuaempyre/).
- This library wraps [SDL2](https://www.libsdl.org/)
