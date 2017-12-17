# Ocean Simulation With OpenGL

## Description

Ocean simulation is based on the project by [Keith Lantz](keithlantz.net).
This program is a downport to older OpenGL 2.0 as well as GLSL 1.10 and GLSL 1.30,
since the original is using modern GLSL >4.00. The model can be run on very old GPUs.
For example, it is possible to run this model even on Intel m965 ;)

Original project is described at:
* [Ocean simulation part one: using the discrete Fourier transform](https://www.keithlantz.net/2011/10/ocean-simulation-part-one-using-the-discrete-fourier-transform/)
* [Ocean simulation part two: using the fast Fourier transform](https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/)

![Ocean Simulation screenshot](https://github.com/Postrediori/OceanSimulation/blob/master/images/ocean.png)

## Environment setup

### Fedora Linux

```
sudo dnf install -y \
    freeglut-devel \
    glew-devel \
    glm-devel \
    freetype-devel \
    SOIL-devel
```

### Ubuntu 16.04

```
sudo apt install -y \
    freeglut3-dev \
    libglew-dev \
    libglm-dev \
    libfreetype6-dev \
    libsoil-dev
```

## Building

```
cd klantz
mkdir build && cd build
cmake ..
make -j4
```
