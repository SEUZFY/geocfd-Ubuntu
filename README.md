# geocfd

Process geometry for cfd simulation.

`Now`:

- Read two adjacent buildings, process repeated vertices and build two `nef polyhedra`.

- Union two `nef polyhedra` into one `big nef polyhedron`.

- Export the `big nef polyhedron` as `.cityjson` file(with no repeated vertices) and visualise it in [ninja](https://ninja.cityjson.org/), observe its `exterior` and `interior`

- Get the `convex hull` of the `big nef polyhedron` and visualise it in [ninja](https://ninja.cityjson.org/), observe its `exterior` and `interior`

`To do`:

- test more complicated buildings 

  - how much will the shape change?
  
  - [3D Minkowski Sum](https://doc.cgal.org/latest/Minkowski_sum_3/index.html#Chapter_3D_Minkowski_Sum_of_Polyhedra) - can we do sum in some specific direction?
 
 `long term`
 
  - `robust`
  
  - `val3dity`  - [validate](http://geovalidation.bk.tudelft.nl/val3dity/) the geometry
  
  - `validator` - [validate](https://validator.cityjson.org/) the `cityjson` file
  
  - `#include` - include multiple files, how to avoid possibly messy includings?

# Dependencies

[CGAL](https://www.cgal.org/) - The version should be above `5.0` since we can use the `header-only`, which means we don't have to manually compile `CGAL`.

if you are using `WSL-Ubuntu` or `Ubuntu`, you can install `CGAL` via:

```console
sudo apt-get update
sudo apt-get install libcgal-dev
```

# How to use?

The files are built and executed via `WSL-Ubuntu` on a `windows10` platform.

If you have `WSL` and `vscode`(with `C++ extension` installed on `WSL`), just clone this project and open it in `vscode`, you should be able to build and run.

## Compile & run in WSL-Ubuntu
you can choose to download the code, and:
```console
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release .. # or just cmake ..
$ make
```
After successful compilation, run:
`$ ./geocfd`
for example:

```console
username@LAPTOP-4OG0PACK:~/geocfd$ mkdir build
username@LAPTOP-4OG0PACK:~/geocfd$ cd build
username@LAPTOP-4OG0PACK:~/geocfd/build$ cmake ..
-- Using header-only CGAL
-- Targetting Unix Makefiles
-- Using /usr/bin/g++ compiler.
-- Boost include dirs: /usr/include
-- Boost libraries:    
-- Using gcc version 4 or later. Adding -frounding-math
-- Build type: Release
-- USING CXXFLAGS = ' -O3 -DNDEBUG'
-- USING EXEFLAGS = ' '
-- Requested component: MPFR
-- Requested component: GMP
-- CGAL found
-- Configuring done
-- Generating done
-- Build files have been written to: /home/fengyan/geocfd/build
username@LAPTOP-4OG0PACK:~/geocfd/build$ make
Consolidate compiler generated dependencies of target geocfd
[ 50%] Building CXX object CMakeFiles/geocfd.dir/src/main.cpp.o
[100%] Linking CXX executable geocfd
[100%] Built target geocfd
username@LAPTOP-4OG0PACK:~/geocfd/build$ ./geocfd
content from the project
```
## issues (if you are using vscode)!

### can I build a new cmake project?

Sure! check this -> https://code.visualstudio.com/docs/cpp/cmake-linux

### not recognizing header files?

**1.** **check** - in `c_cpp_properties.json` file (compiler path and IntelliSense settings), add `include path`

`include` - where the header(which needs to be included) file is

```json
"includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/include",
                "${workspaceFolder}/include"
            ],
```

multiple `"${workspaceFolder}/include"`? (if more than one file needs to include)

**2.** **check** - in `tasks.json` file, add 

```json
"-I${workspaceFolder}/include"
```

```json
"args": [
                "-fdiagnostics-color=always",
                "-g",
                "-I${workspaceFolder}/include",
                "${workspaceFolder}/src/*.cpp",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
```

**3.** **check** - in `CMakeLists.txt` file

`include_directories( ${CMAKE_SOURCE_DIR}/include ) # the file which needs to be included is in the include folder`

## Other platforms

If you use other platforms (such as `windows`, `Linux` or `MacOS`), you can refer to `CMakeLists.txt` file and use it to build a `CMake` project using `src`, `include` and `data` folder.

## useful links when using vscode and C++ in WSL
https://learn.microsoft.com/en-us/windows/wsl/install - install `WSL` on `windows 10/11`

https://code.visualstudio.com/docs/remote/wsl-tutorial - install remote development in WSL extension

https://code.visualstudio.com/docs/cpp/config-wsl - config C++ environment

https://code.visualstudio.com/docs/cpp/cmake-linux - cmake in vscode

https://3d.bk.tudelft.nl/courses/geo1004//hw/0/wsl/ - install `CMake`, `CGAL`, `GDAL`, `GDB` in `WSL`


