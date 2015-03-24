#include <gtest/gtest.h>
#include <Unittests/unittests_common.hh>
#include <iostream>

namespace {

class OpenMeshVectorTest : public testing::Test {

    protected:

        // This function is called before each test is run
        virtual void SetUp() {
            
            // Do some initial stuff with the member data here...
        }

        // This function is called after all tests are through
        virtual void TearDown() {

            // Do some final stuff with the member data here...
        }

};



/*
 * ====================================================================
 * Define tests below
 * ====================================================================
 */

/* Compute surface area via cross product
 */
TEST_F(OpenMeshVectorTest, ComputeTriangleSurfaceWithCrossProduct) {


  //
  // vec1
  //  x
  //  |
  //  |
  //  |
  //  x------>x vec2
  //

  OpenMesh::Vec3d vec1(0.0,1.0,0.0);
  OpenMesh::Vec3d vec2(1.0,0.0,0.0);

  double area = 0.5 * cross(vec1,vec2).norm();
  EXPECT_EQ(0.5f , area ) << "Wrong area in cross product function";

  area = 0.5 * ( vec1 % vec2 ).norm();
  EXPECT_EQ(0.5f , area ) << "Wrong area in cross product operator";

}

/* Check OpenMesh Vector type abs function
 */
TEST_F(OpenMeshVectorTest, AbsTest) {

  OpenMesh::Vec3d vec1(0.5,0.5,-0.5);

  EXPECT_EQ( vec1.l8_norm() , 0.5f ) << "Wrong l8norm computation";

}

/* Compute surface area via cross product
 */
TEST_F(OpenMeshVectorTest, VectorCasting) {

  OpenMesh::Vec3d vecd(1.0,2.0,3.0);
  OpenMesh::Vec3f vecf = OpenMesh::vector_cast<OpenMesh::Vec3f>(vecd);
  EXPECT_EQ(1.f, vecf[0]) << "vector type cast failed on component 0";
  EXPECT_EQ(2.f, vecf[1]) << "vector type cast failed on component 1";
  EXPECT_EQ(3.f, vecf[2]) << "vector type cast failed on component 2";

  OpenMesh::Vec4d vecd4(40.0,30.0,20.0,10.0);
  vecd = OpenMesh::vector_cast<OpenMesh::Vec3d>(vecd4);
  EXPECT_EQ(40.0, vecd[0]) << "vector dimension cast failed on component 0";
  EXPECT_EQ(30.0, vecd[1]) << "vector dimension cast failed on component 1";
  EXPECT_EQ(20.0, vecd[2]) << "vector dimension cast failed on component 2";

}



}
