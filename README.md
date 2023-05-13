# Ocean Simulation

## Introduction

Ocean simulation is based on the project by [Keith Lantz](https://keithlantz.net/). This project uses OpenGL 3.3 by default and it is possible to make configuration for OpenGL 2.0 for maximum compatibility.

The numerical model is based on the paper by J. Tessendorf and utilizes
Phillips spectrum and algorithm of inverse FFT.

![Ocean Simulation screenshot](images/ocean.png)

![Ocean Simulation with UI](images/ocean_ui_win.png)

![Ocean Simulation with Colors UI](images/ocean_colors_ui.png)


## Prerequisites

Generating build files for your OS and build system requires CMake.
Most of the dependencies are bundled with the repository as git submodules.
This includes:

* [glad](https://github.com/Dav1dde/glad) - OpenGL Function Loader.
* [glfw](https://github.com/glfw/glfw) - Windowing and Input.
* [glm](https://github.com/g-truc/glm) - OpenGL Mathematics.
* [plog](https://github.com/SergiusTheBest/plog) - Logging library.
* [imgui](https://github.com/ocornut/imgui) - UI library.
* [stb](https://github.com/nothings/stb) - Saving images for making screenshots.

## Building for Linux

### Dependencies

The following instructions apply to:

* Ubuntu 20.04, 18.04, 16.04
* Debian 9 and higher

```
sudo apt-get install -y \
    build-essential \
    cmake \
    xorg-dev \
    libgl1-mesa-dev \
    libfreetype6-dev
```

The following instructions apply to:

* Fedora 22 and higher

```
sudo dnf install -y \
    gcc gcc-c++ make \
    cmake \
    mesa-libGL-devel \
    mesa-libGLU-devel \
    libXrandr-devel \
    libXinerama-devel \
    libXcursor-devel \
    libXi-devel \
    freetype-devel
```

* CentOS 7 and higher

```
sudo yum install -y \
    gcc gcc-c++ make \
    cmake \
    mesa-libGL-devel \
    mesa-libGLU-devel \
    libXrandr-devel \
    libXinerama-devel \
    libXcursor-devel \
    libXi-devel \
    freetype-devel
```

### Cloning Repository
```
git clone --recursive https://github.com/Postrediori/OceanSimulation.git
cd OceanSimulation
```

Cloning the repository requires passing the `--recursive` flag to load Git submodules.

### Building Project

The program is built with the commands below. CMake requires the directory 
with the main project's `CMakeLists.txt` file as an argument. Then the CMake 
creates the build files for the GNU make which build an executable.

```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
make install
```

### Additional build options

The following options are available when running CMake:
* `USE_OPENGL2_0` (default value: OFF) - Use rendering with legacy OpenGL 2.0 (and GLSL 1.10) for maximum compatibility.

Example usage:

```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_OPENGL2_0=ON
```

## Running Project

After the successful build the binary `Ocean` will end up in `<source dir>/bundle`.

```
cd <PathToProject>
./bundle/Ocean
```

## Building for macOS

### Dependencies
You’ll need to download and install the following to build the project:
* Xcode and the Xcode command line tools
* [CMake](https://cmake.org/) (e.g. via homebrew `brew install cmake`)

### Cloning Repository
```
git clone --recursive https://github.com/Postrediori/OceanSimulation
cd OceanSimulation
```

### Using CMake
Generate makefiles for the build:

```
mkdir build && cd build
cmake .. -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE=Release
```

### Building
```
make
make install
```

An `Ocean` application bundle will now be present under `<Source directory>/bundle`.


## Building for Windows

### Pre-requisites

Install [MSYS2](http://msys2.github.io/) and update it to the latest version by `pacman -Syu`.

### Dependencies

Open the "MSYS2 MinGW 64-bit" (`mingw64.exe`) shell. Download and install all dependencies

```
pacman -S mingw-w64-x86_64-{gcc,cmake} make git
```

### Cloning Repository

```
git clone --recursive https://github.com/Postrediori/OceanSimulation
cd OceanSimulation
```

### Building

```
mkdir build && cd build
cmake -G "MSYS Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
make install
```

An `Ocean` application bundle will now be present under `<Source directory>/bundle`.


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
* `waveAmplitude` - amplitude of the surface disturbance.
* `windDirX`, `windDirZ` - components of the wind vector. The vertical (Y) component is ignored in the model.
* `oceanLen` - discretization parameter.
* `oceanRepeat` - this model does use only a small patch of memory to model the whole Ocean.
This parameter adjusts the size of the model in terms of one minimal patch. The value represents
the number of ocean patches along X and Z dimensions. E.g. value 5 mean the simulated area will be 5x5
the size of the initial patch.
* `oceanSize`, `oceanLen` - these are the parameters of the initial patch that will be used for
simulation of the ocean. The first parameter is the grid size and the second value
stands for the length of the patch.

## Controls

* `F1` - Toggle fullscreen mode.
* `F2` - Show/hide UI on the screen.
* `Arrow keys` - Navigate the model.
* `PgUp/PgDown` - Adjust vertical position of the viewer point.
* `1` - Toggle wireframe mode.
* `2` - Toggle solid surface ("normal") mode.
* `S` - Switch to the next post-processing shader.

## Links

*  Tessendorf, Jerry. [Simulating Ocean Water](http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.161.9102&rep=rep1&type=pdf). In SIGGRAPH 2002 Course Notes #9 (Simulating Nature: Realistic and Interactive Techniques), ACM Press.
* [Ocean simulation part one: using the discrete Fourier transform](https://www.keithlantz.net/2011/10/ocean-simulation-part-one-using-the-discrete-fourier-transform/)
* [Ocean simulation part two: using the fast Fourier transform](https://www.keithlantz.net/2011/11/ocean-simulation-part-two-using-the-fast-fourier-transform/)

## TODO
* [x] Adjust ocean parameters in the UI.
* [x] Color adjustment.
* [x] Add build instructions for Windows.
* [ ] Mouse navigation.
* [ ] Add Vulkan renderer.
