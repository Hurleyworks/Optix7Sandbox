# Optix7Sandbox

A framework for experimenting with the new Optix7 API

### Prerequisites

Graphics Hardware:

● All NVIDIA GPUs of Compute Capability 5.0 (Maxwell) or higher are supported.
Graphics Driver:

● OptiX 7.0.0 requires that you install the 435.80 driver on Windows or the 435.12 Driver for linux.. Note
OptiX dll from the SDK are no longer needed since the symbols are loaded from the driver.

● Windows 7/8.1/10 64-bit; 

● CUDA Toolkit 7, 8, 9, 10

OptiX 7.0.0 has been built with CUDA 10.1, but any specified toolkit should work when compiling PTX for OptiX.

OptiX uses the CUDA device API, but the CUDA runtime API objects can be cast to device API obj

### Installing

1. Go to the thirdparty folder and run the *.bat file for the compiler of your choice

2. Go to the newly created thirdparty/builds folder and  open the visual studio solution and compile

3. Go back to the root folder and run the *.bat file for the compiler of your choice

4. Go to the newly created /builds folder and open the visual studio solution and compile the sandbox programs

## Optix7Sandbox/Samples

### Dreamer

Based on the Optix7 SDK optixMeshView sample but allows dynamic adding of new meshes. Supports picking, geometry instancing and basic dynamic transforms. A few simple meshes can be added from the gui or you can drag and drop files or folders of objects into the window. OBJ, PLY, OFF and glTF 2.0 files are supported. Currently does not support glTF2.0 files with multiple meshes 

![Viewer screen grab](https://github.com/Hurleyworks/Optix7Sandbox/blob/master/resources/Common/sample_screen_grabs/Dreamer.png)


