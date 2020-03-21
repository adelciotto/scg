# Simple Computer Graphics (SCG)

My personal single file header library for quick and simple cross-platform graphics applications. It is currently in a WIP progress state, so not really usable for others. But feel free to take any code from it if it's useful.

The library does all rendering in software by manually setting pixels. At the end of each frame these pixels are sent to a texture on the GPU which is scaled and presented on screen. This method of drawing is slow and not intended for any serious production software. It is designed for learning purposes and for building interactive demos.

## Setup

The library requires SDL2.

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
* `starfield`
* `starfield_3d`
* `voxel_space`
* `audio`
* `matrix`
* `metablobs`

By default all the examples are built with optimization flags. To build in debug mode, use the `DEBUG=1` flag.

```sh
make DEBUG=1
```

Or for building a specific example:

```sh
make {{example_name}} DEBUG=1
```

## Examples

Basic | Plasma | Image
--- | --- | ---
![basic](/screenshots/basic.bmp) | ![plasma](/screenshots/plasma.bmp) | ![image](/screenshots/image.bmp)
[Source](/examples/basic.c) | [Source](/examples/plasma.c) | [Source](examples/image.c)

Transform | Rotozoom | Tunnel
--- | --- | ---
![transform](/screenshots/transform.bmp) | ![rotozoom](/screenshots/rotozoom.bmp) | ![tunnel](/screenshots/tunnel.bmp)
[Source](examples/transform.c) | [Source](examples/rotozoom.c) | [Source](examples/tunnel.c)

Starfield | Starfield 3D | Voxel Space
--- | --- | ---
![starfield](/screenshots/starfield.bmp) | ![starfield-3d](/screenshots/starfield_3d.bmp) | ![voxel-space](/screenshots/voxel_space.bmp)
[Source](examples/starfield.c) | [Source](examples/starfield_3d.c) | [Source](examples/voxel_space.c)

| Audio | Matrix | Metablobs
| --- | --- | ---
| ![audio](/screenshots/audio.bmp) | ![matrix](/screenshots/matrix.bmp) | ![metablobs](/screenshots/metablobs.bmp)
| [Source](examples/audio.c) | [Source](examples/matrix.c) | [Source](examples/metablobs.c)

## Converting audio

Ensure `ffmpeg` is installed. Then run:

```sh
ffmpeg -i assets/{example_sound}.wav -acodec pcm_s16le -ac 2 -ar 48000 assets/{example_sound_output}.wav
```

## Attributions / References

- This library wraps [SDL2](https://www.libsdl.org/).
- The work here is inspired by [Javidx9's](https://github.com/OneLoneCoder) work on the [olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine).
- Music in the basic example is by [joshuaempyre](https://freesound.org/people/joshuaempyre/).
	- Original link: https://freesound.org/people/joshuaempyre/sounds/251461/.
- Space texture used in the tunnel example is by [webtreats](http://www.everystockphoto.com/photographer.php?photographer_id=71737)
	- Original link: http://www.everystockphoto.com/photo.php?imageId=9268254.
- Tunnel, Rotozoom and Metablobs effects were implemented following the helpful descriptions given by [mrkite](https://github.com/mrkite) at https://seancode.com/demofx/.
- Voxel Space effect implemented with help from https://github.com/s-macke/VoxelSpace.
