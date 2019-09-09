#include "voxelizer.h"

#include "igl/readOFF.h"
#include "igl/mat_max.h"
#include "igl/mat_min.h"

#include "igl/ray_mesh_intersect.h"

#include <iostream>
#include <algorithm>

using namespace Eigen;

Voxelizer::Voxelizer() {
}

Voxelizer::Voxelizer(std::string path) {
    loadMesh(path);
}

Voxelizer::Voxelizer(MatrixXd &Verts, MatrixXi &Faces) {
    V = Verts;
    F = Faces;
}

void Voxelizer::loadMesh(std::string path) {
    igl::readOFF(path, V, F);
}

void Voxelizer::computeVoxels() {
    //init voxels array
    voxels.clear();
    voxels.reserve(res*res*res);
    for (size_t i = 0; i < res*res*res; ++i) {
        voxels.push_back(false);
    }
    
    //get size of a voxel
    VectorXi idxs;
    VectorXf vals;
    igl::mat_max(V,1,vals,idxs);
    maxX = vals[0];
    maxY = vals[1];
    maxZ = vals[2];
    
    igl::mat_min(V,1,vals,idxs);
    minX = vals[0];
    minY = vals[1];
    minZ = vals[2];

    double lengthX = maxX - minX;
    double lengthY = maxY - minY;
    double lengthZ = maxZ - minZ;

    double longestSide = lengthX > lengthY ? (lengthX > lengthZ ? lengthX : lengthZ) : (lengthY > lengthZ ? lengthY : lengthZ);
    cubeDim = longestSide / res;

    corner = Vector3d(minX,minY,minZ);
    corner(0) -= (longestSide - lengthX) / 2;
    corner(1) -= (longestSide - lengthY) / 2;
    corner(2) -= (longestSide - lengthZ) / 2;

    //iterate over each voxel. use ray_mesh_interesect to get hits
    Vector3d cornerToCenter(cubeDim/2,cubeDim/2,cubeDim/2);
    Vector3d dir(1,0,0);
    #pragma omp parallel for
    for (size_t i = 0; i < res; ++i) {
        for (size_t j = 0; j < res; ++j) {
            for (size_t k = 0; k < res; ++k) {
                std::vector<igl::Hit> hits;
                Vector3d src = corner + Vector3d(i*cubeDim, j*cubeDim, k*cubeDim) + cornerToCenter;
                if (igl::ray_mesh_intersect(src, dir, V, F, hits) && hits.size() % 2 == 1 && 
                igl::ray_mesh_intersect(src, -dir, V, F, hits) && hits.size() % 2 == 1) {
                    voxels[i*res*res + j*res + k] = true;
                }
            }
        }
    }

}


void Voxelizer::computeMesh(MatrixXd &Verts, MatrixXi &Faces) {
    int voxNb = std::count (voxels.begin(), voxels.end(), true);

    Verts = MatrixXd::Zero(8*voxNb,3);
    Faces = MatrixXi::Zero(12*voxNb,3);
    int faceCount = 0, vertexCount = 0;
    for (size_t i = 0; i < res; ++i) {
        for (size_t j = 0; j < res; ++j) {
            for (size_t k = 0; k < res; ++k) {
                if (!voxels[i*res*res + j*res + k]) continue;
                Verts(vertexCount,0) = corner(0) + i * cubeDim;
                Verts(vertexCount+1,0) = corner(0) + i * cubeDim;
                Verts(vertexCount+2,0) = corner(0) + i * cubeDim;
                Verts(vertexCount+3,0) = corner(0) + i * cubeDim;
                Verts(vertexCount+4,0) = corner(0) + (i+1) * cubeDim;
                Verts(vertexCount+5,0) = corner(0) + (i+1) * cubeDim;
                Verts(vertexCount+6,0) = corner(0) + (i+1) * cubeDim;
                Verts(vertexCount+7,0) = corner(0) + (i+1) * cubeDim;
                Verts(vertexCount,1) = corner(1) + j * cubeDim;
                Verts(vertexCount+1,1) = corner(1) + j * cubeDim;
                Verts(vertexCount+2,1) = corner(1) + (j+1) * cubeDim;
                Verts(vertexCount+3,1) = corner(1) + (j+1) * cubeDim;
                Verts(vertexCount+4,1) = corner(1) + j * cubeDim;
                Verts(vertexCount+5,1) = corner(1) + j * cubeDim;
                Verts(vertexCount+6,1) = corner(1) + (j+1) * cubeDim;
                Verts(vertexCount+7,1) = corner(1) + (j+1) * cubeDim;
                Verts(vertexCount,2) = corner(2) + k * cubeDim;
                Verts(vertexCount+1,2) = corner(2) + (k+1) * cubeDim;
                Verts(vertexCount+2,2) = corner(2) + k * cubeDim;
                Verts(vertexCount+3,2) = corner(2) + (k+1) * cubeDim;
                Verts(vertexCount+4,2) = corner(2) + k * cubeDim;
                Verts(vertexCount+5,2) = corner(2) + (k+1) * cubeDim;
                Verts(vertexCount+6,2) = corner(2) + k * cubeDim;
                Verts(vertexCount+7,2) = corner(2) + (k+1) * cubeDim;
                Faces(faceCount,0) = vertexCount + 1;
                Faces(faceCount+1,0) = vertexCount + 1;
                Faces(faceCount+2,0) = vertexCount + 1;
                Faces(faceCount+3,0) = vertexCount + 1;
                Faces(faceCount+4,0) = vertexCount + 3;
                Faces(faceCount+5,0) = vertexCount + 3;
                Faces(faceCount+6,0) = vertexCount + 5;
                Faces(faceCount+7,0) = vertexCount + 5;
                Faces(faceCount+8,0) = vertexCount + 1;
                Faces(faceCount+9,0) = vertexCount + 1;
                Faces(faceCount+10,0) = vertexCount + 2;
                Faces(faceCount+11,0) = vertexCount + 2;
                Faces(faceCount,1) = vertexCount + 7;
                Faces(faceCount+1,1) = vertexCount + 3;
                Faces(faceCount+2,1) = vertexCount + 4;
                Faces(faceCount+3,1) = vertexCount + 2;
                Faces(faceCount+4,1) = vertexCount + 8;
                Faces(faceCount+5,1) = vertexCount + 4;
                Faces(faceCount+6,1) = vertexCount + 7;
                Faces(faceCount+7,1) = vertexCount + 8;
                Faces(faceCount+8,1) = vertexCount + 5;
                Faces(faceCount+9,1) = vertexCount + 6;
                Faces(faceCount+10,1) = vertexCount + 6;
                Faces(faceCount+11,1) = vertexCount + 8;
                Faces(faceCount,2) = vertexCount + 5;
                Faces(faceCount+1,2) = vertexCount + 7;
                Faces(faceCount+2,2) = vertexCount + 3;
                Faces(faceCount+3,2) = vertexCount + 4;
                Faces(faceCount+4,2) = vertexCount + 7;
                Faces(faceCount+5,2) = vertexCount + 8;
                Faces(faceCount+6,2) = vertexCount + 8;
                Faces(faceCount+7,2) = vertexCount + 6;
                Faces(faceCount+8,2) = vertexCount + 6;
                Faces(faceCount+9,2) = vertexCount + 2;
                Faces(faceCount+10,2) = vertexCount + 8;
                Faces(faceCount+11,2) = vertexCount + 4;
                vertexCount += 8;
                faceCount += 12;
            }
        }
    }
    Faces = Faces.array()-1;
}