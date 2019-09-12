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

MatrixXd V;
MatrixXi F;
MatrixXd C;

const std::string DATADIR = "/home/bverdier/voxelizer/data";
const std::string BUNNY = "/home/bverdier/voxelizer/data/bunny.off";

int cli(int argc, char *argv[]);

int main(int argc, char *argv[])
{   
    //initializing things

    //parse cli args
    return cli(argc, argv);
}

int cli(int argc, char *argv[]) {
    std::string file = BUNNY;
    std::string saveFile = "";
    std::string cell = "";
    std::string outputFile = "";
    bool customCell = false;
    bool voxelize = false;
    bool saveVox = false;
    bool fileIsVoxels = false;
    bool display = false;
    bool write = false;
    int res = 32;

    int opt;
    while ((opt = getopt(argc, argv, "f:vs:e:r:dc:w:")) != -1) {
        switch (opt) {
            case 'f':
                file = optarg;
                break;
            case 'v':
                voxelize = true;
                break;
            case 's':
                saveVox = true;
                saveFile = optarg;
                break;
            case 'e':
                fileIsVoxels = true;
                file = optarg;
                break;
            case 'r':
                res = atoi(optarg);
                break;
            case 'd':
                display = true;
                break;
            case 'c':
                customCell = true;
                cell = optarg;
                break;
            case 'w':
                write = true;
                outputFile = optarg;
            default:
                break;
        }
    }
    
    Voxelizer vox;

    if (customCell) {
        vox.loadCell(cell);
    }
    
    if (!fileIsVoxels) {
        vox.loadMesh(file);
        vox.setRes(res);
        
        if (voxelize) {
            vox.computeVoxels();
            vox.computeVoxelizedMesh();
            V = vox.Vv;
            F = vox.Fv;

            if (saveVox) {
                vox.saveVoxels(saveFile);
            }

        } else {
            V = vox.V;
            F = vox.F;
        }
    } else {
        vox.loadVoxels(file);
        vox.computeVoxelizedMesh();
        V = vox.Vv;
        F = vox.Fv;
    }

    if (write) {
        vox.writeMesh(outputFile);
    }

    // Plot the mesh
    opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    viewer.data().set_face_based(true);
    viewer.core().toggle(viewer.data().show_lines);

    // Launch the viewer
    viewer.launch();

    return 0;
}