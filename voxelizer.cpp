#include "voxelizer.h"

#include "igl/read_triangle_mesh.h"
#include "igl/write_triangle_mesh.h"
#include "igl/ray_mesh_intersect.h"

#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <fstream>

using namespace Eigen;

Voxelizer::Voxelizer() {
    Vc = (Eigen::MatrixXd(8,3)<<
    0.0,0.0,0.0,
    0.0,0.0,1.0,
    0.0,1.0,0.0,
    0.0,1.0,1.0,
    1.0,0.0,0.0,
    1.0,0.0,1.0,
    1.0,1.0,0.0,
    1.0,1.0,1.0).finished();
    Fc = (Eigen::MatrixXi(12,3)<<
    1,7,5,
    1,3,7,
    1,4,3,
    1,2,4,
    3,8,7,
    3,4,8,
    5,7,8,
    5,8,6,
    1,5,6,
    1,6,2,
    2,6,8,
    2,8,4).finished().array()-1;
}

Voxelizer::Voxelizer(std::string path) {
    loadMesh(path);
}

Voxelizer::Voxelizer(MatrixXd &Verts, MatrixXi &Faces) {
    V = Verts;
    F = Faces;
}

void Voxelizer::loadMesh(std::string path) {
    igl::read_triangle_mesh(path, V, F);
}

void Voxelizer::writeMesh(std::string path) {
    igl::write_triangle_mesh(path, Vv, Fv);
}

void Voxelizer::loadCell(std::string path) {
    igl::read_triangle_mesh(path, Vc, Fc);
    Vector3d m = Vc.colwise().minCoeff();
    for (size_t i = 0; i < Vc.rows(); ++i) {
        Vc(i,0) -= m(0);
        Vc(i,1) -= m(1);
        Vc(i,2) -= m(2);
    }
    Vector3d M = Vc.colwise().maxCoeff();
    auto scaler = Scaling(1./M(0), 1./M(1), 1./M(2));
    Vc *= scaler;
}

void Voxelizer::computeVoxels() {
    //init voxels array
    voxels.clear();
    voxels.reserve(res*res*res);
    for (size_t i = 0; i < res*res*res; ++i) {
        voxels.push_back(false);
    }
    
    //get size of a voxel
    Vector3d m = V.colwise().minCoeff();
    Vector3d M = V.colwise().maxCoeff();
    Vector3d dif = M-m;
    double min = dif.minCoeff();
    if (min < 10) {
        V *= 10/min;
    }
    m = V.colwise().minCoeff();
    M = V.colwise().maxCoeff();
    dif = M-m;
    double longestSide = dif.maxCoeff();
    
    maxX = M(0);
    maxY = M(1);
    maxZ = M(2);
    minX = m(0);
    minY = m(1);
    minZ = m(2);

    double lengthX = dif(0);
    double lengthY = dif(1);
    double lengthZ = dif(2);

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
                if (igl::ray_mesh_intersect(src, dir, V, F, hits) && hits.size() % 2 == 1) {
                    voxels[i*res*res + j*res + k] = true;
                }
            }
        }
    }
}


void Voxelizer::computeVoxelizedMesh() {
    int voxNb = std::count (voxels.begin(), voxels.end(), true);

    Vv = MatrixXd::Zero(Vc.rows()*voxNb,3);
    Fv = MatrixXi::Zero(Fc.rows()*voxNb,3);
    int faceCount = 0, vertexCount = 0;
    for (size_t i = 0; i < res; ++i) {
        for (size_t j = 0; j < res; ++j) {
            for (size_t k = 0; k < res; ++k) {
                if (!voxels[i*res*res + j*res + k]) continue;
                for (size_t m = 0; m < Vc.rows(); ++m) {
                    Vv(vertexCount + m,0) = Vc(m,0) + i;
                    Vv(vertexCount + m,1) = Vc(m,1) + j;
                    Vv(vertexCount + m,2) = Vc(m,2) + k;
                }
                for (size_t m = 0; m < Fc.rows(); ++m) {
                    Fv(faceCount + m,0) = Fc(m,0) + vertexCount;
                    Fv(faceCount + m,1) = Fc(m,1) + vertexCount;
                    Fv(faceCount + m,2) = Fc(m,2) + vertexCount;
                }
                vertexCount +=  Vc.rows();
                faceCount += Fc.rows();
            }
        }
    }
}

void Voxelizer::saveVoxels(std::string path) {
    std::ofstream outfile (path, std::ofstream::binary);

    char c = res;
    outfile.write (&c,1);
    int pos;
    c = 0;
    for (size_t i = 0; i < voxels.size(); ++i) {
        pos = i % 8;
        if (voxels[i])
            c |= 1 << pos;
        else
            c |= 0 << pos;
        if (pos == 7) {
            outfile.write (&c,1);
            c = 0;
        }
    }
    if (pos < 7) {
        outfile.write (&c,1);
    }

    outfile.close();
}


void Voxelizer::loadVoxels(std::string path) {
    std::ifstream infile (path,std::ifstream::binary);
    char c;
    infile.read(&c,1);
    res = int(c);
    voxels.clear();
    int voxnum = res*res*res;
    voxels.reserve(voxnum);
    for (size_t i = 0; i < voxnum; ++i) {
        voxels.push_back(false);
    }
    int curVox = 0;
    for (size_t i = 0; i < voxnum; ++i) {
        int pos = i % 8;
        if (pos == 0) {
            infile.read(&c,1);
        }
        voxels[i] = ((c >> pos) & 0x1);
    }
    infile.close();
}
