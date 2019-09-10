# Voxelizer

## Description

This is a voxelizer. It takes a mesh and voxelizes it.

Current state: takes a mesh and voxelizes it. Can save/load voxelization to save resources.

Final Objective: take a mesh, voxelize it and replace voxels with metamaterial cells.

## TODO:

- Build graph of voxels for cell gradient variations.

- Integrate voxel to cell pipeline.

- Cleanup.

## Usage

Note: we assume the user has a `data` folder with a `bunny.off` file as it is the default file loaded.

Launch `voxelizer` to load the mesh of the bunny. Available options:
- `-f file` to load the mesh located at `file`. Presently it is assumed that the file is in the OFF format.
- `-v` to voxelize the mesh.
- `-r n` to specify the resolution (number of voxels in the largest dimension).
- `-s file` to save the voxelization at `file`.
- `-e file` to load the voxelization at `file`.

## Build

This projects is highly dependent of [libigl](https://libigl.github.io/). To build, first clone and build libigl, then add the include folder under the LIBIGL environment variable. Then build like any project:
```
mkdir build
cd build
cmake ..
make
```

To run, simply run the voxelizer executable.

# Notes

- Based off the example project from libigl found [here](https://github.com/libigl/libigl-example-project).
- Developed and solely tested on Linux (Ubuntu 16.04).