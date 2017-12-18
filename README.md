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
    libfreetype6-dev \
    libsoil-dev
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
    freetype-devel \
    SOIL-devel
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

After the successful build the binary `Ocean` will end up in `build/Ocean/`.

```
./Ocean/Ocean
```

## Links

* [Ocean simulation part one: using the discrete Fourier transform](https://www.keithlantz.net/2011/10/ocean-simulation-part-one-using-the-discrete-fourier-transform/)
* [Ocean simulation part two: using the fast Fourier transform](https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/)
