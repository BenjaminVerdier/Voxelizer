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
#include <igl/file_dialog_open.h>

//local includes
#include "voxelizer.h"

using namespace Eigen;
using namespace igl;

const std::string DATADIR = "/home/bverdier/voxelizer/data";
const std::string BUNNY = "/home/bverdier/voxelizer/data/bunny.off";

Voxelizer vox;

int cli(int argc, char *argv[]);

bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier);

void displayHelp();

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
    int res = 16;

    int opt;
    while ((opt = getopt(argc, argv, "f:vs:e:r:dc:w:h")) != -1) {
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
                break;
            case 'h':
                std::cout << "Voxelizer help:" << "\n" <<\
                                "-f filename \t opens a mesh located at filename. 'default' loads the stanford bunny." << "\n" <<\
                                "-v \t\t voxelizes the mesh." << "\n" <<\
                                "-s filename \t saves the voxelization to filename." << "\n" <<\
                                "-e filename \t loads voxel file (created with -s) located at filename." << "\n" <<\
                                "-r n \t\t sets the resolution (number of voxels along the largest direction of the mesh) to n." << "\n" <<\
                                "-d \t\t opens the display." << "\n" <<\
                                "-c filename \t loads a custom cell at filename to use instead of a cube." << "\n" <<\
                                "-w filename \t saves the voxelized mesh to filename." << "\n" <<\
                                "-h \t\t displays this menu." << "\n";
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
    
    displayHelp();
    // Plot the mesh
    opengl::glfw::Viewer viewer;

    viewer.callback_key_down = &key_down;

    viewer.append_mesh();\
    viewer.append_mesh();
    viewer.core().toggle(viewer.data(0).show_lines);
    viewer.core().toggle(viewer.data(1).show_lines);
    viewer.core().toggle(viewer.data(2).show_lines);
    if (vox.Vm.size()>0)
        viewer.data(0).set_mesh(vox.Vm, vox.Fm);
    viewer.data(0).set_face_based(true);
    viewer.data(1).set_mesh(vox.Vc, vox.Fc);
    viewer.data(1).set_face_based(true);
    if (vox.Vv.size()>0)
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
    else if (key == 'M') {
        std::string fname = igl::file_dialog_open();
        if(fname.length() == 0) return false;
        std::cout << "Loading mesh " << fname << "...\n";
        vox.loadMesh(fname);
        viewer.data(0).clear();
        viewer.data(0).set_mesh(vox.Vm, vox.Fm);
        viewer.data(0).set_face_based(true);
        viewer.data(0).set_visible(true);
        viewer.data(1).set_visible(false);
        viewer.data(2).set_visible(false);
        viewer.core().align_camera_center(vox.Vm,vox.Fm);
        std::cout << "Mesh loaded.\n";
    }
    else if (key == 'V') {
        std::cout << "Voxelization started...\n";
        vox.computeVoxels();
        vox.computeVoxelizedMesh();
        viewer.data(2).clear();
        viewer.data(2).set_mesh(vox.Vv, vox.Fv);
        viewer.data(2).set_face_based(true);
        viewer.data(0).set_visible(false);
        viewer.data(1).set_visible(false);
        viewer.data(2).set_visible(true);
        viewer.core().align_camera_center(vox.Vv,vox.Fv);
        std::cout << "Voxelization Done.\n";
    }
    else if (key == 'C') {
        std::string fname = igl::file_dialog_open();
        if(fname.length() == 0) return false;
        vox.loadCell(fname);
        viewer.data(1).clear();
        viewer.data(1).set_mesh(vox.Vc, vox.Fc);
        viewer.data(1).set_face_based(true);
        viewer.data(0).set_visible(false);
        viewer.data(1).set_visible(true);
        viewer.data(2).set_visible(false);
        viewer.core().align_camera_center(vox.Vc,vox.Fc);
    }
    else if (key == 'B') {
        std::string fname = igl::file_dialog_open();
        if(fname.length() == 0) return false;
        vox.loadVoxels(fname);
        vox.computeVoxelizedMesh();
        viewer.data(2).clear();
        viewer.data(2).set_mesh(vox.Vv, vox.Fv);
        viewer.data(2).set_face_based(true);
        viewer.data(0).set_visible(false);
        viewer.data(1).set_visible(false);
        viewer.data(2).set_visible(true);
        viewer.core().align_camera_center(vox.Vv,vox.Fv);
    }
    else if (key == 'S') {
        std::string fname = igl::file_dialog_save();
        if(fname.length() == 0) return false;
        vox.writeVoxelizedMesh(fname);
    }
    else if (key == 'W') {
        std::string fname = igl::file_dialog_save();
        if(fname.length() == 0) return false;
        vox.saveVoxels(fname);
    }
    return false;
}


void displayHelp() {
    std::cout << \
        "Additional commands available in the display window:" << "\n"\
        "1 \t Displays original mesh." << "\n"\
        "2 \t Displays cell mesh." << "\n"\
        "3 \t Displays voxelized mesh." << "\n"\
        "M \t Opens dialog window to load mesh." << "\n"\
        "C \t Opens dialog window to load cell." << "\n"\
        "B \t Opens dialog window to load voxelization." << "\n"\
        "V \t Voxelizes original mesh with cell." << "\n"\
        "S \t Opens dialog window to save voxelized mesh." << "\n"\
        "W \t Opens dialog windiw to save voxelization." << "\n";

}