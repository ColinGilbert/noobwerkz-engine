#include <gtest/gtest.h>
#include <Unittests/unittests_common.hh>

#include <iostream>

namespace {

class OpenMeshTrimeshCirculatorFaceVertex : public OpenMeshBase {

    protected:

        // This function is called before each test is run
        virtual void SetUp() {
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



/*
 * Small FaceVertexIterator Test
 */
TEST_F(OpenMeshTrimeshCirculatorFaceVertex, FaceVertexIterWithoutIncrement) {

  mesh_.clear();

  // Add some vertices
  Mesh::VertexHandle vhandle[5];

  vhandle[0] = mesh_.add_vertex(Mesh::Point(0, 1, 0));
  vhandle[1] = mesh_.add_vertex(Mesh::Point(1, 0, 0));
  vhandle[2] = mesh_.add_vertex(Mesh::Point(2, 1, 0));
  vhandle[3] = mesh_.add_vertex(Mesh::Point(0,-1, 0));
  vhandle[4] = mesh_.add_vertex(Mesh::Point(2,-1, 0));

  // Add two faces
  std::vector<Mesh::VertexHandle> face_vhandles;

  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[2]);
  Mesh::FaceHandle fh0 = mesh_.add_face(face_vhandles);

  face_vhandles.clear();

  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[4]);
  mesh_.add_face(face_vhandles);

  face_vhandles.clear();

  face_vhandles.push_back(vhandle[0]);
  face_vhandles.push_back(vhandle[3]);
  face_vhandles.push_back(vhandle[1]);
  mesh_.add_face(face_vhandles);

  face_vhandles.clear();

  face_vhandles.push_back(vhandle[2]);
  face_vhandles.push_back(vhandle[1]);
  face_vhandles.push_back(vhandle[4]);
  mesh_.add_face(face_vhandles);

  /* Test setup:
      0 ==== 2
      |\  0 /|
      | \  / |
      |2  1 3|
      | /  \ |
      |/  1 \|
      3 ==== 4 */


  // Check face handle index
  EXPECT_EQ(0, fh0.idx() ) << "Index wrong in FaceVertexIter at initialization";

  // Iterate around vertex 1 at the middle (with holes in between)
  Mesh::FaceVertexIter fv_it  = mesh_.fv_begin(fh0);
  Mesh::FaceVertexIter fv_end = mesh_.fv_end(fh0);
  EXPECT_EQ(0, fv_it->idx() ) << "Index wrong in FaceVertexIter at initialization";
  EXPECT_TRUE(fv_it.is_valid()) << "Iterator invalid in FaceVertexIter at initialization";
  ++fv_it ;
  EXPECT_EQ(1, fv_it->idx() ) << "Index wrong in FaceVertexIter at step 1";
  EXPECT_TRUE(fv_it.is_valid()) << "Iterator invalid in FaceVertexIter at step 1";
  ++fv_it ;
  EXPECT_EQ(2, fv_it->idx() ) << "Index wrong in FaceVertexIter at step 2";
  EXPECT_TRUE(fv_it.is_valid()) << "Iterator invalid in FaceVertexIter at step 2";
  ++fv_it ;
  EXPECT_EQ(0, fv_it->idx() ) << "Index wrong in FaceVertexIter at step 3";
  EXPECT_FALSE(fv_it.is_valid()) << "Iterator invalid in FaceVertexIter at step 3";
  EXPECT_TRUE( fv_it == fv_end )  << "End iterator for FaceVertexIter not matching";

  // Iterate around vertex 1 at the middle (with holes in between) with const iterator
  Mesh::ConstFaceVertexIter cfv_it   = mesh_.cfv_begin(fh0);
  Mesh::ConstFaceVertexIter cfv_end  = mesh_.cfv_end(fh0);
  EXPECT_EQ(0, cfv_it->idx() ) << "Index wrong in ConstFaceVertexIter at initialization";
  EXPECT_TRUE(cfv_it.is_valid()) << "Iterator invalid in ConstFaceVertexIter at initialization";
  ++cfv_it ;
  EXPECT_EQ(1, cfv_it->idx() ) << "Index wrong in ConstFaceVertexIter at step 1";
  EXPECT_TRUE(cfv_it.is_valid()) << "Iterator invalid in ConstFaceVertexIter at step 1";
  ++cfv_it ;
  EXPECT_EQ(2, cfv_it->idx() ) << "Index wrong in ConstFaceVertexIter at step 2";
  EXPECT_TRUE(cfv_it.is_valid()) << "Iterator invalid in ConstFaceVertexIter at step 2";
  ++cfv_it ;
  EXPECT_EQ(0, cfv_it->idx() ) << "Index wrong in ConstFaceVertexIter at step 3";
  EXPECT_FALSE(cfv_it.is_valid()) << "Iterator invalid in ConstFaceVertexIter at step 3";
  EXPECT_TRUE( cfv_it == cfv_end )  << "End iterator for ConstFaceVertexIter not matching";
}


/*
 * Test if the end iterator stays invalid after one lap
 */
TEST_F(OpenMeshTrimeshCirculatorFaceVertex, FaceVertexIterCheckInvalidationAtEnds) {

  mesh_.clear();

   // Add some vertices
   Mesh::VertexHandle vhandle[5];

   vhandle[0] = mesh_.add_vertex(Mesh::Point(0, 1, 0));
   vhandle[1] = mesh_.add_vertex(Mesh::Point(1, 0, 0));
   vhandle[2] = mesh_.add_vertex(Mesh::Point(2, 1, 0));
   vhandle[3] = mesh_.add_vertex(Mesh::Point(0,-1, 0));
   vhandle[4] = mesh_.add_vertex(Mesh::Point(2,-1, 0));

   // Add two faces
   std::vector<Mesh::VertexHandle> face_vhandles;

   face_vhandles.push_back(vhandle[0]);
   face_vhandles.push_back(vhandle[1]);
   face_vhandles.push_back(vhandle[2]);
   Mesh::FaceHandle fh0 = mesh_.add_face(face_vhandles);

   face_vhandles.clear();

   face_vhandles.push_back(vhandle[1]);
   face_vhandles.push_back(vhandle[3]);
   face_vhandles.push_back(vhandle[4]);
   mesh_.add_face(face_vhandles);

   face_vhandles.clear();

   face_vhandles.push_back(vhandle[0]);
   face_vhandles.push_back(vhandle[3]);
   face_vhandles.push_back(vhandle[1]);
   mesh_.add_face(face_vhandles);

   face_vhandles.clear();

   face_vhandles.push_back(vhandle[2]);
   face_vhandles.push_back(vhandle[1]);
   face_vhandles.push_back(vhandle[4]);
   mesh_.add_face(face_vhandles);

   /* Test setup:
       0 ==== 2
       |\  0 /|
       | \  / |
       |2  1 3|
       | /  \ |
       |/  1 \|
       3 ==== 4 */


  // Check if the end iterator stays invalid after end
  Mesh::FaceVertexIter endIter = mesh_.fv_end(fh0);
  EXPECT_FALSE(endIter.is_valid()) << "EndIter is not invalid";
  ++endIter ;
  EXPECT_FALSE(endIter.is_valid()) << "EndIter is not invalid after increment";

  // Check if the end iterators becomes valid after decrement
  endIter = mesh_.fv_end(fh0);
  EXPECT_FALSE(endIter.is_valid()) << "EndIter is not invalid";
  --endIter;
  EXPECT_TRUE(endIter.is_valid()) << "EndIter is invalid after decrement";
  EXPECT_EQ(2,endIter->idx()) << "EndIter points on the wrong element";


  // Check if the start iterator decrement is invalid
  Mesh::FaceVertexIter startIter = mesh_.fv_begin(fh0);
  EXPECT_TRUE(startIter.is_valid()) << "StartIter is not valid";
  --startIter;
  EXPECT_FALSE(startIter.is_valid()) << "StartIter decrement is not invalid";

  // Check if the start iterator becomes valid
  ++startIter;
  EXPECT_TRUE(startIter.is_valid()) << "StartIter is invalid after re-incrementing";
  EXPECT_EQ(startIter->idx(), mesh_.fv_begin(fh0)->idx()) << "StartIter points on the wrong element";

}


}
