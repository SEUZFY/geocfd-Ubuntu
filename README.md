# geocfd

Process geometry for cfd simulation.

`Now`:

Read two adjacent buildings, process repeated vertices and build two `nef polyhedra`.

`To do`:

Union two nef polyhedra and output as one building via `.cityjson` file.

# How to use?

The files are built and executed via `WSL` on a `windows10` platform.

If you have `WSL` and `vscode`(with C++ extension installed on WSL), just clone this project and open it in `vscode`, you should be able to build and run.

## compile & run in WSL-Ubuntu
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

If you use other platforms (such as `windows`, `Linux` or `MacOS`), you can refer to `CMakeLists.txt` file and use it to build a `CMake` project using `src`, `include` and `data` folder.

