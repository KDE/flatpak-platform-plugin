# flatpak-platform-plugin

A Qt platform plugin for [xdg-desktop-portal](http://github.com/flatpak/xdg-desktop-portal)

This plugin implements only desktop portals for flatpak, while leaving other implementation
to plugins provided by Qt itself. To specify Qt platform plugin set QT_FLATPAK_REAL_PLATFORM
variable (e.g. QT_QPA_FLATPAK_PLATFORM="xcb"), if you don't specify this variable then "xcb"
platform plugin will be used by default. If you want to specify Qt platform theme, you need to
set QT_FLATPAK_REAL_PLATFORM_THEME variable (e.g. QT_QPA_FLATPAK_PLATFORM_THEME="kde"), if you
don't specify this variable then Qt platform theme will be chosen according to your current desktop.

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
