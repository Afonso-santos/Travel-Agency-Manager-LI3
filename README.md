# Projeto LI3

## Grade: XX/20 ⭐️

An [glib](https://docs.gtk.org/glib/) and [ncurses](https://invisible-island.net/ncurses/) based project, made for
our [LI3](https://www4.di.uminho.pt/~jno/sitedi/uc_J303N1.html) class. It is licensed under the
[Apache License](http://www.apache.org/licenses/LICENSE-2.0).

Project requirements (set by the professors) are in `Requirements.pdf` (🇵🇹).

## Building

### Dependencies

- GNU Make (build-time)
- GCC (build-time)
- Bash (build-time)
- Doxygen (optional, build-time)
- glib
- ncurses

Note that the only supported platform is **Linux** (others \*NIX systems _may_ work) and the only
supported compiler is GCC (clang _may_ work).

### Building

A release build is achieved with:

```bash
$ make
```

To perform a debug build (no optimizations, debug symbols enabled):

```bash
$ DEBUG=1 make
```

To generate documentation (Doxygen is required):

```bash
$ make docs
```

To remove files generated by a build, run:

```bash
$ make clean
```

## Running

```bash
$ ./programa-principal <path to *.csv files> <path to input file>
```

## Contributing

As a university group project, we cannot allow external contributors.

## Group Members

* [Afonso Dionísio Santos](https://github.com/afonso-santos/) (a104276)
* [Mário André Leite Rodrigues](https://github.com/MarioRodrigues10/) (a100109)
* [Pedro Figueiredo Pereira](https://github.com/pedrofp4444) (a104082)
