#include "CSG.hpp"

#define V3ARGSIN(a)       (a)[X]/25.4, (a)[Y]/25.4, (a)[Z]/25.4
#define VSETIN(a, b)	{\
	(a)[X] = (b)[X]/25.4; \
	(a)[Y] = (b)[Y]/25.4; \
	(a)[Z] = (b)[Z]/25.4; \
    }

void noob::csg::init()
{
    tree_state = rt_initial_tree_state;
    tree_state.ts_tol = &calculation_tolerance;
    tree_state.ts_ttol = &tesselation_tolerance;
    tree_state.ts_m = &csg_model;

    // Set up tessellation tolerance defaults
    tesselation_tolerance.magic = RT_TESS_TOL_MAGIC;
    // Defaults, updated by command line options.
    tesselation_tolerance.abs = 0.0;
    tesselation_tolerance.rel = 0.01;
    tesselation_tolerance.norm = 0.0;

    // Set up calculation tolerance defaults
    // FIXME: These need to be improved
    calculation_tolerance.magic = BN_TOL_MAGIC;
    calculation_tolerance.dist = 0.0005;
    calculation_tolerance.dist_sq = calculation_tolerance.dist * calculation_tolerance.dist;
    calculation_tolerance.perp = 1e-6;
    calculation_tolerance.para = 1 - calculation_tolerance.perp;

    // init resources we might need
    rt_init_resource(&rt_uniresource, 0, NULL);
}


noob::basic_mesh noob::csg::to_basic_mesh() const
{

}
