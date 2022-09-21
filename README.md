# geocfd

Process geometry for cfd simulation.

`Now`:

Read two adjacent buildings, process repeated vertices and build two `nef polyhedra`.

Union two nef polyhedra.

`To do`:

- process repeated vertices of the `big nef polyhedron`

- export to `.cityjson` and visualise in ninja

- test more complicated buildings 

  - how much will the shape change?
  
  - [3D Minkowski Sum](https://doc.cgal.org/latest/Minkowski_sum_3/index.html#Chapter_3D_Minkowski_Sum_of_Polyhedra) - can we do sum in some specific direction?
 
 `long term`
 
  - robust
  
  - `val3dity`  - [validate](http://geovalidation.bk.tudelft.nl/val3dity/) the geometry
  
  - `validator` - [validate](https://validator.cityjson.org/) the `cityjson` file

# How to use?

The files are built and executed via `WSL` on a `windows10` platform.

-------------------------------------------------------------------------------------------------------------------------------------------------------------

## dependencies

1. `WSL` - check: https://learn.microsoft.com/en-us/windows/wsl/install

2. `vscode` - installed on the local machine, not installed in `WSL`

3. `C++ extension pack` - installed in `WSL` NOT locally

4. `CGAL` - installed in `WSL`

------------------------------------------------------------------------------------------------------------------------------------------------------------

## set up the environment(C++ vscode WSL):

https://learn.microsoft.com/en-us/windows/wsl/install - install `WSL` on `windows 10/11`

https://code.visualstudio.com/docs/remote/wsl-tutorial - install remote development in WSL extension

https://code.visualstudio.com/docs/cpp/config-wsl - config C++ environment

https://code.visualstudio.com/docs/cpp/cmake-linux - cmake in vscode

https://3d.bk.tudelft.nl/courses/geo1004//hw/0/wsl/ - install `CMake`, `CGAL`, `GDAL`, `GDB` in `WSL`

------------------------------------------------------------------------------------------------------------------------------------------------------------

## issues!

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

------------------------------------------------------------------------------------------------------------------------------------------------------------

## build and run

If everything goes right, you can `build` and `run` like so:

1. you can `press` the `build` button in vscode:

![image](https://user-images.githubusercontent.com/72781910/191454706-5be07145-3276-4488-bd7f-f9d08c954427.png)

2. after the `build`, do the following command in your `ternimal`:

`cd build` - this command means you enter into geocfd/build folder

`./geocfd` - this command means you execute the geocfd project, `geocfd` is the name of the project

------------------------------------------------------------------------------------------------------------------------------------------------------------

for example:

-> build

![image](https://user-images.githubusercontent.com/72781910/191267077-34bac47c-954f-4e0e-9397-194cae06594c.png)

-> execute(in the terminal)

![image](https://user-images.githubusercontent.com/72781910/191267218-2a77ef4e-a575-4288-9ce4-69a2f412709d.png)

Then you could see some prompt information in the terminal:

![image](https://user-images.githubusercontent.com/72781910/191267583-f2908ce0-d295-4285-8e01-ae2ef9864346.png)

-------------------------------------------------------------------------------------------------------------------------------------------------------------
If you use other platforms (such as `windows` or `MacOS`), you can refer to `CMakeLists.txt` file and use it to build a `CMake` project using `src`, `include` and `data` folder.

