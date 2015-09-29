
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

#if __cplusplus > 199711L || defined(__GXX_EXPERIMENTAL_CXX0X__)

/*
 */
TEST_F(OpenMesh_Triangle, cpp11_initializer_test) {

  OpenMesh::Vec3d vec1 = { 1, 0, 0};
  OpenMesh::Vec3d vec2 = {0, 0.777971, 0.996969};


  EXPECT_EQ( dot(vec1,vec2) ,0.0 );

}

TEST_F(OpenMesh_Triangle, cpp11_vvrange_test) {
  //check empty vvrange
  mesh_.clear();
  Mesh::VertexHandle vh = mesh_.add_vertex(Mesh::Point(0, 1, 0));
  for(auto t : mesh_.vv_range(vh))
  {
    FAIL() << "The vvrange for a single vertex is not empty";
  }
}

TEST_F(OpenMesh_Poly, cpp11_vvrange_test) {
  mesh_.clear();
  PolyMesh::VertexHandle vh = mesh_.add_vertex(PolyMesh::Point(0, 1, 0));
  for(auto t : mesh_.vv_range(vh))
  {
    FAIL() << "The vvrange for a single vertex is not empty";
  }
}

#endif

}
