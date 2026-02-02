# Kylin Distrobox

A native C++/Qt5 GUI for Distrobox, designed for OpenKylin.

## Overview
This project is a port of the original Python-based Distrobox GUI to C++ and Qt5, utilizing the OpenKylin SDK (`kysdk-qtwidgets`) for native integration.

## Dependencies
*   Qt5 (Core, Gui, Widgets)
*   OpenKylin SDK (`libkysdk-qtwidgets`, `libkysdk-qtwidgets-dev`)
*   `distrobox` (Runtime dependency)
*   `cmake`
*   `pkg-config`

## Build Instructions

```bash
mkdir build
cd build
cmake ..
make
```

## Running
```bash
./kylin-distrobox
```

## Features
*   List containers
*   Start/Stop/Delete containers
*   Enter containers (generates command)
*   Native OpenKylin look and feel
