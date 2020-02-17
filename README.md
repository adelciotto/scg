# Simple Computer Graphics (SCG)

My personal single file header library for quick and simple cross-platform graphics applications. It is currently in a WIP progress state, so not really usable for others. But feel free to take any code from it if it's useful.

The library does all rendering in software by manually setting pixels. At the end of each frame these pixels are sent to a texture on the GPU which is scaled and presented on screen. This method of drawing is slow and not intended for any serious production software. It is designed for learning purposes and for building interactive demos.

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
* `plasma`
* `image`
* `transform`
* `rotozoom`
* `tunnel`

By default all the examples are built with optimization flags. To build in debug mode, use the `DEBUG=1` flag.

```sh
make DEBUG=1
```

Or for building a specific example:

```sh
make {{example_name}} DEBUG=1
```

## Examples

| Basic                          | Plasma                         | Image                         | Transform                             | Rotozoom                             | Tunnel                          |
| ------------------------------ | ------------------------------ | ----------------------------- | ------------------------------------- | ------------------------------------ | ------------------------------- |
| ![basic](/previews/basic.png)  | ![plasma](/previews/plasma.png)| ![image](/previews/image.png) | ![transform](/previews/transform.png) | ![rotozoom](/previews/rotozoom.png)  | ![tunnel](/previews/tunnel.png) |
| [Source](/examples/basic.c)    | [Source](/examples/plasma.c)   | [Source](examples/image.c)    | [Source](examples/transform.c)        | [Source](examples/rotozoom.c)        | [Source](examples/tunnel.c)     |

## Converting audio

Ensure `ffmpeg` is installed. Then run:

```sh
ffmpeg -i assets/{example_sound}.wav -acodec pcm_s16le -ac 2 -ar 48000 assets/{example_sound_output}.wav
```

## Attributions / References

- Music in the basic example is by [joshuaempyre](https://freesound.org/people/joshuaempyre/).
- This library wraps [SDL2](https://www.libsdl.org/).
