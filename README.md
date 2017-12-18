## Introduction

Ocean simulation is based on the project by [Keith Lantz](https://keithlantz.net/).
This program is a downport to older OpenGL 2.0 as well as GLSL 1.10 and GLSL 1.30,
since the original is using modern GLSL >4.00. The model can be run on very old GPUs.
For example, it is possible to run this model even on Mobile Intel 965 ;)

![Ocean Simulation screenshot](images/ocean.png)

## Prerequisites

Compiling and running the sample project requires GCC, CMake, GNU Make
as well as some graphics libraries installed.

## Environment Setup

### Debian-based Systems

The following instructions are applied to these operating systems:

* Ubuntu 16.04
* Debian 9

```
sudo apt-get install -y \
    build-essential \
    cmake \
    freeglut3-dev \
    libglew-dev \
    libglm-dev \
    libfreetype6-dev
```

### RedHat-based Systems

The following instructions are applied to these operating systems:

* Fedora >=22

```
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y \
    cmake \
    freeglut-devel \
    glew-devel \
    glm-devel \
    freetype-devel
```

## Building Project

The program is built with the commands below. CMake requires the directory 
with the main project's `CMakeLists.txt` file as an argument. Then the CMake 
creates the build files for the GNU make which build an executable.

```
cd <PathToProject>
mkdir build && cd build
cmake ..
make
```

After the successful build the binary `Ocean` will end up in `build/src/Ocean/` directory.

```
cd <PathToProject>
cd build/src/Ocean
./Ocean
```

## Installation

In order to make the stand-alone bundle for the simulation executable and all the data files `make install`
is used.

```
cd <PathToProject>/build
make install
```

This will install all of the files required for an executable to `<PathToProject>/bundle/Ocean` directory.

```
cd <PathToProject>/bundle
tree
.
└── Ocean
    ├── data
    │   ├── font.ttf
    │   ├── ocean.cfg
    │   ├── ocean.frag
    │   └── ocean.vert
    └── Ocean

2 directories, 5 files
cd Ocean
./Ocean
```

## Configuration

The parameters of the model can be adjusted in the `data/ocean.cfg` file:

```
# configuration file for Ocean Simulation

# wave parameters
waveAmplitude = 2e-5

# wind parameters
windDirX = 0.0
windDirZ = 12.8

oceanSize = 64
oceanLen = 320.0
oceanRepeat = 5
```

The following parameters are to be adjusted:
* **waveAmplitude** -- Amplitude of the 
* **windDirX**, **windDirZ** -- components of the wind vector. The vertical (Y) component is ignored in the model.
* **oceanLen** -- discretization parameter.
* **oceanRepeat** -- this model does use only a small patch of memory to model the whole Ocean.
This parameter adjusts the size of the model in terms of one minimal patch. The value represents
the number of ocean patches along X and Z dimensions. E.g. value 5 mean the simulated area will be 5x5
the size of the initial patch.
* **oceanSize**, **oceanLen** -- these are the parameters of the initial patch that will be used for
simulation of the ocean. The first parameter is the grid size and the second value
stands for the length of the patch.

## Controls

* **F1** -- Toggle fullscreen mode.
* **F2** -- Show/hide help on the screen.
* **Mouse** -- Control the view.
* **Arrow keys** -- Navigate the model.
* **PgUp/PgDown** -- Adjust vertical position of the viewer point.
* **1** -- Toggle wireframe mode.
* **2** -- Toggle solid surface ("normal") mode.

## Links

* [Ocean simulation part one: using the discrete Fourier transform](https://www.keithlantz.net/2011/10/ocean-simulation-part-one-using-the-discrete-fourier-transform/)
* [Ocean simulation part two: using the fast Fourier transform](https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/)

## TODO
* Add the detection of the latest supported GLSL version as well as shaders adjusted for newer versions.
* Add build instructions for Windows.
