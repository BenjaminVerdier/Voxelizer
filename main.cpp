#include <igl/readOFF.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/jet.h>

#include "voxelizer.h"

using namespace Eigen;
using namespace igl;

MatrixXd V;
MatrixXi F;
MatrixXd C;

const std::string DATADIR = "/home/bverdier/voxelizer/data";
const std::string BUNNY = "/home/bverdier/voxelizer/data/bunny.off";

int main(int argc, char *argv[])
{
  // Load a mesh in OFF format
  Voxelizer vox;
  vox.setRes(32);
  vox.loadMesh(BUNNY);
  vox.computeVoxels();
  vox.computeMesh(V,F);

  // Plot the mesh
  opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(V, F);

  // Launch the viewer
  viewer.launch();
}
