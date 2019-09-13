//std includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <string>

//igl includes
#include <igl/opengl/glfw/Viewer.h>
#include <igl/jet.h>

//local includes
#include "voxelizer.h"

using namespace Eigen;
using namespace igl;

const std::string DATADIR = "/home/bverdier/voxelizer/data";
const std::string BUNNY = "/home/bverdier/voxelizer/data/bunny.off";

Voxelizer vox;

int cli(int argc, char *argv[]);

bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier);

int main(int argc, char *argv[])
{   
    //initializing things

    //parse cli args
    return cli(argc, argv);
}

int cli(int argc, char *argv[]) {
    std::string meshFile = "";
    std::string voxelFile = "";
    std::string saveFile = "";
    std::string cellFile = "";
    std::string outputFile = "";
    bool loadMesh = false;
    bool customCell = false;
    bool voxelize = false;
    bool saveVox = false;
    bool loadVoxels = false;
    bool display = false;
    bool write = false;
    int res = 32;

    int opt;
    while ((opt = getopt(argc, argv, "f:vs:e:r:dc:w:")) != -1) {
        switch (opt) {
            case 'f':
                loadMesh = true;
                meshFile = optarg;
                if (!meshFile.compare("default")) meshFile = BUNNY;
                break;
            case 'v':
                voxelize = true;
                break;
            case 's':
                saveVox = true;
                saveFile = optarg;
                break;
            case 'e':
                loadVoxels = true;
                voxelFile = optarg;
                break;
            case 'r':
                res = atoi(optarg);
                break;
            case 'd':
                display = true;
                break;
            case 'c':
                customCell = true;
                cellFile = optarg;
                break;
            case 'w':
                write = true;
                outputFile = optarg;
            default:
                break;
        }
    }

    if (customCell) {
        vox.loadCell(cellFile);
    }

    if (loadMesh) {
        vox.setRes(res);
        vox.loadMesh(meshFile);
        if (voxelize) {
            vox.computeVoxels();
            vox.computeVoxelizedMesh();

            if (saveVox) {
                vox.saveVoxels(saveFile);
            }

        } else if (loadVoxels) {
            vox.loadVoxels(voxelFile);
            vox.computeVoxelizedMesh();
            vox.processMesh();
        }
    }

    

    if (write) {
        vox.writeVoxelizedMesh(outputFile);
    }

    if (!display) return 0;

    // Plot the mesh
    opengl::glfw::Viewer viewer;

    viewer.callback_key_down = &key_down;

    viewer.append_mesh();
    viewer.append_mesh();
    viewer.core().toggle(viewer.data(0).show_lines);
    viewer.core().toggle(viewer.data(1).show_lines);
    viewer.core().toggle(viewer.data(2).show_lines);
    viewer.data(0).set_mesh(vox.Vm, vox.Fm);
    viewer.data(0).set_face_based(true);
    viewer.data(1).set_mesh(vox.Vc, vox.Fc);
    viewer.data(1).set_face_based(true);
    viewer.data(2).set_mesh(vox.Vv, vox.Fv);
    viewer.data(2).set_face_based(true);

    viewer.data(1).set_visible(false);
    if (loadMesh) {
        viewer.data(0).set_visible(true);
        viewer.data(2).set_visible(false);
        viewer.core().align_camera_center(vox.Vm,vox.Fm);
    } else if (voxelize || loadVoxels) {
        viewer.data(0).set_visible(false);
        viewer.data(2).set_visible(true);
        viewer.core().align_camera_center(vox.Vv,vox.Fv);
    }

    // Launch the viewer
    viewer.launch();

    return 0;
}

bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier)
{
  if (key == '1')
  {
    viewer.data(0).set_visible(true);
    viewer.data(1).set_visible(false);
    viewer.data(2).set_visible(false);
    viewer.core().align_camera_center(vox.Vm,vox.Fm);
  }
  else if (key == '2')
  {
    viewer.data(0).set_visible(false);
    viewer.data(1).set_visible(true);
    viewer.data(2).set_visible(false);
    viewer.core().align_camera_center(vox.Vc,vox.Fc);
  }
  else if (key == '3')
  {
    viewer.data(0).set_visible(false);
    viewer.data(1).set_visible(false);
    viewer.data(2).set_visible(true);
    viewer.core().align_camera_center(vox.Vv,vox.Fv);
  }
  return false;
}