/*
 * VectorT.cpp
 *
 *  Created on: Nov 19, 2015
 *      Author: ebke
 */

#ifndef BMPREFIX
#error "Do not compile directly."
#endif

#define ASSEMBLE_(PREFIX, SUFFIX) PREFIX ## SUFFIX
#define ASSEMBLE(PREFIX, SUFFIX) ASSEMBLE_(PREFIX, SUFFIX)
#define MYBENCHMARK(NAME) BENCHMARK(NAME)

static void ASSEMBLE(BMPREFIX, Vec3f_add_compare)(benchmark::State& state) {
    OpenMesh::Vec3f v1(0, 0, 0);
    OpenMesh::Vec3f v2(1000, 1000, 1000);
    while (state.KeepRunning()) {
        v1 += OpenMesh::Vec3f(1.1, 1.2, 1.3);
        v2 -= OpenMesh::Vec3f(1.1, 1.2, 1.3);
        if (v1 == v2) {
            v1 -= v2;
            v2 += v1;
        }
    }
}

MYBENCHMARK (ASSEMBLE(BMPREFIX, Vec3f_add_compare));

static void ASSEMBLE(BMPREFIX, Vec3d_add_compare)(benchmark::State& state) {
    OpenMesh::Vec3d v1(0, 0, 0);
    OpenMesh::Vec3d v2(1000, 1000, 1000);
    while (state.KeepRunning()) {
        v1 += OpenMesh::Vec3d(1.1, 1.2, 1.3);
        v2 -= OpenMesh::Vec3d(1.1, 1.2, 1.3);
        if (v1 == v2) {
            v1 -= v2;
            v2 += v1;
        }
    }
}

MYBENCHMARK (ASSEMBLE(BMPREFIX, Vec3d_add_compare));

static void ASSEMBLE(BMPREFIX, Vec3d_cross_product)(benchmark::State& state) {
    OpenMesh::Vec3d v1(0, 0, 0);
    OpenMesh::Vec3d v2(1000, 1000, 1000);
    while (state.KeepRunning()) {
        v1 += OpenMesh::Vec3d(1.1, 1.2, 1.3);
        v2 -= OpenMesh::Vec3d(1.1, 1.2, 1.3);
        if ((v1 % v2) == (v2 % v1)) {
            v1 -= v2;
            v2 += v1;
        }
    }
}

MYBENCHMARK (ASSEMBLE(BMPREFIX, Vec3d_cross_product));

static void ASSEMBLE(BMPREFIX, Vec3d_scalar_product)(benchmark::State& state) {
    OpenMesh::Vec3d v1(0, 0, 0);
    OpenMesh::Vec3d v2(1000, 1000, 1000);
    while (state.KeepRunning()) {
        v1 += OpenMesh::Vec3d(1.1, 1.2, 1.3);
        v2 -= OpenMesh::Vec3d(1.1, 1.2, 1.3);
        if ((v1 | v2) > 0) {
            v1 -= v2;
            v2 += v1;
        }
    }
}

MYBENCHMARK (ASSEMBLE(BMPREFIX, Vec3d_scalar_product));
