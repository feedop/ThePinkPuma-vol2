# The Pink Puma vol. 2
Puma robot simulator including inverse kinematics

## Build
Warning: due to using newest C++ 23 features, the prerequisites for building this project are quite restrictive:
 - CMake 3.31.0+
 - clang 18.1.3+
 - Ninja 1.11+
 - libc++ and libc++-abi installed
 - libglfw3 installed
 - mesa-utils installed

Building the project is also possible on Windows with VS 2022 17.12 as MSVC supports all the required features.
The provided CMake doesn't include that option (yet), so you'd have to change it manually.

In the main directory execute the following commands:
```
mkdir build

cd build

cmake -G Ninja ..

ninja
```

## Usage
In the build directory:
```
./ThePinkPuma-vol2
```

The app consists of two windows - the scene and config.
Have fun messing around with the different config options to find a visualization method that best suits your taste.

Mouse controls - right mouse button:
 - RBM - rotate scene
 - SHIFT + RBM - camera movement
 - CTRL + RBM - 3d cursor rotation
 - CTRL + SHIFT + RBM - 3d cursor movement
 - scroll wheel - zoom in/out

Keyboard controls - move the nth joint
 - keys A,S,D,F,G,H - increase the first/second/.../nth parameter respectively
 - keys Z,X,C,V,B,N - decrease the first/second/.../nth parameter respectively