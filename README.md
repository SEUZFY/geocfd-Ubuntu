# geocfd

Process geometry for cfd simulation.

`Now`:

Read two adjacent buildings, process repeated vertices and build two `nef polyhedra`.

`To do`:

Union two nef polyhedra and output as one building via `.cityjson` file.

# How to use?

The files are built and executed via `WSL` on a `windows10` platform.

-------------------------------------------------------------------------------------------------------------------------------------------------------------

## dependencies

1. `WSL` - check: https://learn.microsoft.com/en-us/windows/wsl/install

2. `vscode` - installed on the local machine, not installed in `WSL`

3. `C++ extension pack` - installed in `WSL` NOT locally, you can check: 

4. `CGAL` - installed in `WSL`

------------------------------------------------------------------------------------------------------------------------------------------------------------

## set up the environment(C++ vscode WSL):

https://code.visualstudio.com/docs/remote/wsl-tutorial - install remote development in WSL extension

https://code.visualstudio.com/docs/cpp/config-wsl - config C++ environment

https://code.visualstudio.com/docs/cpp/cmake-linux - cmake in vscode

https://3d.bk.tudelft.nl/courses/geo1004//hw/0/wsl/ - install `CMake`, `CGAL`, `GDAL`, `GDB` in `WSL`

If you have `WSL` and `vscode`(with C++ extension installed on WSL), just clone this project and open it in `vscode`, you should be able to build and run:

you can `press` the `build` button in vscode:

![image](https://user-images.githubusercontent.com/72781910/191454706-5be07145-3276-4488-bd7f-f9d08c954427.png)

after the `build`, do:

`cd build` - this command means you enter into geocfd/build folder

`./geocfd` - this command means you execute the geocfd project, `geocfd` is the name of the project

for example:

-> build

![image](https://user-images.githubusercontent.com/72781910/191267077-34bac47c-954f-4e0e-9397-194cae06594c.png)

-> execute(in the terminal)

![image](https://user-images.githubusercontent.com/72781910/191267218-2a77ef4e-a575-4288-9ce4-69a2f412709d.png)

Then you could see some prompt information in the terminal:

![image](https://user-images.githubusercontent.com/72781910/191267583-f2908ce0-d295-4285-8e01-ae2ef9864346.png)

-------------------------------------------------------------------------------------------------------------------------------------------------------------
If you use other platforms (such as 'windows' or `MacOS`), you can refer to `CMakeLists.txt` file and use it to build a `CMake` project using `src`, `include` and `data` folder.

