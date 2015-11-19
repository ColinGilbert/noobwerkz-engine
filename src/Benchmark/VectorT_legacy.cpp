/*
 * VectorT_legacy.cpp
 *
 *  Created on: Nov 19, 2015
 *      Author: ebke
 */

#include <benchmark/benchmark_api.h>

#define OPENMESH_VECTOR_LEGACY
#include <OpenMesh/Core/Geometry/VectorT.hh>

#define BMPREFIX Legacy_
#include "VectorT.cpp"
