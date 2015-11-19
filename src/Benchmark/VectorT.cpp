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
#define MYBENCHMARK_TEMPLATE(NAME, TYPE) BENCHMARK_TEMPLATE(NAME, TYPE)

template<class Vec>
static void ASSEMBLE(BMPREFIX, Vec_add_compare)(benchmark::State& state) {
    Vec v1(0, 0, 0);
    Vec v2(1000, 1000, 1000);
    while (state.KeepRunning()) {
        v1 += Vec(1.1, 1.2, 1.3);
        v2 -= Vec(1.1, 1.2, 1.3);
        if (v1 == v2) {
            v1 -= v2;
            v2 += v1;
        }
    }
    // Just so nothing gets optimized away.
    static double dummy;
    dummy = v1.norm() + v2.norm();
}

MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_add_compare), OpenMesh::Vec3d);
MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_add_compare), OpenMesh::Vec3f);

template<class Vec>
static void ASSEMBLE(BMPREFIX, Vec_cross_product)(benchmark::State& state) {
    Vec v1(0, 0, 0);
    Vec v2(1000, 1000, 1000);
    while (state.KeepRunning()) {
        v1 += Vec(1.1, 1.2, 1.3);
        v2 -= Vec(1.1, 1.2, 1.3);
        v1 = (v1 % v2);
    }
    // Just so nothing gets optimized away.
    static double dummy;
    dummy = v1.norm() + v2.norm();
}

MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_cross_product), OpenMesh::Vec3d);
MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_cross_product), OpenMesh::Vec3f);

template<class Vec>
static void ASSEMBLE(BMPREFIX, Vec_scalar_product)(benchmark::State& state) {
    Vec v1(0, 0, 0);
    Vec v2(1000, 1000, 1000);
    double acc = 0;
    while (state.KeepRunning()) {
        v1 += Vec(1.1, 1.2, 1.3);
        v2 -= Vec(1.1, 1.2, 1.3);
        acc += (v1 | v2);
    }
    // Otherwise GCC will optimize everything away.
    static double dummy;
    dummy = acc;
}

MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_scalar_product), OpenMesh::Vec3d);
MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_scalar_product), OpenMesh::Vec3f);

template<class Vec>
static void ASSEMBLE(BMPREFIX, Vec_norm)(benchmark::State& state) {
    Vec v1(0, 0, 0);
    double acc = 0;
    while (state.KeepRunning()) {
        v1 += Vec(1.1, 1.2, 1.3);
        acc += v1.norm();
    }
    // Otherwise GCC will optimize everything away.
    static double dummy;
    dummy = acc;
}

MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_norm), OpenMesh::Vec3d);
MYBENCHMARK_TEMPLATE (ASSEMBLE(BMPREFIX, Vec_norm), OpenMesh::Vec3f);
