//std includes
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <string>

//igl includes
#include <igl/readOFF.h>
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
    bool voxelize = false;
    bool saveVox = false;
    bool fileIsVoxels = false;
    int res = 32;

    int opt;
    while ((opt = getopt(argc, argv, "f:vs:e:r:")) != -1) {
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
                if (!optarg) break;
                res = atoi(optarg);
                break;
            default:
                break;
        }
    }
    
    Voxelizer vox;
    
    if (!fileIsVoxels) {
        vox.loadMesh(file);
        vox.setRes(res);
        
        if (voxelize) {
            vox.computeVoxels();
            vox.computeMesh(V,F);

            if (saveVox) {
                vox.saveVoxels(saveFile);
            }

        } else {
            V = vox.V;
            F = vox.F;
        }
    } else {
        vox.loadVoxels(file);
        vox.computeMesh(V,F);
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