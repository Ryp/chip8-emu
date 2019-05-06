# Chip-8 Emulator

I coded this simple emulator for fun and as an exercise. It can run vanilla Chip-8 ROMs without any kind of fancy extensions.
I used the publicly available `pong.rom` for testing as well as a bunch of custom unit tests.

## Building

This should get you going after cloning the repo:
```sh
$ cmake -H. -B./build
$ cmake --build build
$ ./build/chip8emu <your_rom_here>
```

**Disclaimer:** I didn't spend too much effort making this portable/packaged at all.
This was developped under Linux, but the dependencies (SDL2) are available in Windows so if you really want to run that there that won't be too much effort.

If you want to change the SDL2 backend, my source is modular enough to make that happen easily. Contact me if you have questions!

## Input

For simplicity, this program assumes that you are using a QWERTY layout, and maps the original 4x4 keys on this area of the keyboard:
```
Original    ->  QWERTY layout
1  2  3  C      1  2  3  4
4  5  6  D      Q  W  E  R
7  8  9  E      A  S  D  F
A  0  B  F      Z  X  C  V
```

You can exit the emulator by pressing `ESC`.

## Output

For the video output, I'm scaling the original 64x32 framebuffer by 8 for more comfortable viewing by default. This can only be changed by recompiling `main.cpp` for now.

**Note:** Sound is NOT supported at the moment.

## Acknowledgments

This was implemented mostly with the help of [Cowgod's technical specs](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM). Thanks for your very complete specs!
