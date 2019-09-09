#ifndef VOXELIZER_H
#define VOXELIZER_H

#include <Eigen/Dense>
#include <vector>

using namespace Eigen;

class Voxelizer {

public:
//constructors and destructors
Voxelizer();                            //bare constructors
Voxelizer(std::string path);            //cst with path to mesh
Voxelizer(MatrixXd &Verts, MatrixXi &Faces);    //cst with existing vertices and faces matrices
~Voxelizer(){};

//utilities
void loadMesh(std::string path);
void computeVoxels();
void computeMesh(MatrixXd &Verts, MatrixXi &Faces);

//getters/setters
double getRes() {return res;};
void setRes(double r) {res = r;};

MatrixXd V; //vertices of the current mesh
MatrixXi F; //Faces of the current mesh


protected:
std::vector<bool> voxels;               //voxels vector, size is res^3
int res;                             //number of voxels in the largest dimension
double cubeDim;                         //dimension of one cube
double minX,maxX,minY,maxY,minZ,maxZ;   //bounding box
Vector3d corner;                        //corner of the voxel grid with lowest x,y,z values


};

#endif // VOXELIZER_H   
