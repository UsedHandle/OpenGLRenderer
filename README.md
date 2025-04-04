# OpenGLRenderer
![Image of output](https://github.com/UsedHandle/OpenGLRenderer/blob/main/output.PNG?raw=true)
## build (CMake fetches assimp, glm, and glfw if needed but not freetype)
For mac or linux:
```
cmake .
make
```
Windows (vcpkg):
```
vcpkg install freetype:x64-windows
cmake . --preset=vcpkg
```
On windows, ```make``` can be replaced with compilation through Visual Studio after setting OpenGLRender as the startup project
## usage
make sure ```resources/``` and ```shaders/``` are in the current working directory
