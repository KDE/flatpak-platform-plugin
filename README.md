# flatpak-platform-plugin

A Qt platform plugin for [xdg-desktop-portal](http://github.com/flatpak/xdg-desktop-portal)

## Building flatpak-platform-plugin

### Dependencies:
 - xdg-desktop-portal (runtime)
 - flatpak (runtime)
 - Qt 5 (build)

### Build instructions:
```
$ mkdir build && cd build
$ cmake .. [your_options]
$ make -j5
$ make install
```
