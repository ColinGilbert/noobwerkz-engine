
#include <gtest/gtest.h>
#include <Unittests/unittests_common.hh>
#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>

namespace {

class OpenMesh_Poly : public OpenMeshBasePoly {

    protected:

        // This function is called before each test is run
        virtual void SetUp() {

            // Do some initial stuff with the member data here...
        }

        // This function is called after all tests are through
        virtual void TearDown() {

            // Do some final stuff with the member data here...
        }

    // Member already defined in OpenMeshBase
    //Mesh mesh_;
};

class OpenMesh_Triangle : public OpenMeshBase {

    protected:

        // This function is called before each test is run
        virtual void SetUp() {

            // Do some initial stuff with the member data here...
        }

        // This function is called after all tests are through
        virtual void TearDown() {

            // Do some final stuff with the member data here...
        }

    // Member already defined in OpenMeshBase
    //Mesh mesh_;
};



/*
 * ====================================================================
 * Define tests below
 * ====================================================================
 */

#if __cplusplus > 199711L or __GXX_EXPERIMENTAL_CXX0X__

/*
 */
TEST_F(OpenMesh_Triangle, cpp11_initializer_test) {

  OpenMesh::Vec3d vec1 = { 1, 0, 0};
  OpenMesh::Vec3d vec2 = {0, 0.777971, 0.996969};


  EXPECT_EQ( dot(vec1,vec2) ,0.0 );

}

#endif

}
