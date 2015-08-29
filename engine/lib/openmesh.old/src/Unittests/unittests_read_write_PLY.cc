#include <gtest/gtest.h>
#include <Unittests/unittests_common.hh>


namespace {

class OpenMeshReadWritePLY : public OpenMeshBase {

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

/*
 * Just load a point file in ply format and count whether
 * the right number of entities has been loaded.
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePointPLYFileWithBadEncoding) {

    mesh_.clear();

    bool ok = OpenMesh::IO::read_mesh(mesh_, "pointCloudBadEncoding.ply");

    EXPECT_TRUE(ok) << "Unable to load pointCloudBadEncoding.ply";

    EXPECT_EQ(10u , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(0u  , mesh_.n_edges()) << "The number of loaded edges is not correct!";
    EXPECT_EQ(0u  , mesh_.n_faces()) << "The number of loaded faces is not correct!";
}

/*
 * Just load a point file in ply format and count whether
 * the right number of entities has been loaded.
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePointPLYFileWithGoodEncoding) {

    mesh_.clear();

    bool ok = OpenMesh::IO::read_mesh(mesh_, "pointCloudGoodEncoding.ply");

    EXPECT_TRUE(ok) << "Unable to load pointCloudGoodEncoding.ply";

    EXPECT_EQ(10u , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(0u  , mesh_.n_edges()) << "The number of loaded edges is not correct!";
    EXPECT_EQ(0u  , mesh_.n_faces()) << "The number of loaded faces is not correct!";
}

/*
 * Just load a ply
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePLY) {

    mesh_.clear();

    bool ok = OpenMesh::IO::read_mesh(mesh_, "cube-minimal.ply");

    EXPECT_TRUE(ok) << "Unable to load cube-minimal.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges()) << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces()) << "The number of loaded faces is not correct!";

}

/*
 * Just load a ply file and set vertex color option before loading
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePLYForceVertexColorsAlthoughNotAvailable) {

  mesh_.clear();

  mesh_.request_vertex_colors();

  std::string file_name = "cube-minimal.ply";

  OpenMesh::IO::Options options;
  options += OpenMesh::IO::Options::VertexColor;

  bool ok = OpenMesh::IO::read_mesh(mesh_, file_name,options);

  EXPECT_TRUE(ok) << file_name;

  EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
  EXPECT_EQ(18u , mesh_.n_edges())     << "The number of loaded edges is not correct!";
  EXPECT_EQ(12u , mesh_.n_faces())     << "The number of loaded faces is not correct!";
  EXPECT_EQ(36u , mesh_.n_halfedges())  << "The number of loaded halfedges is not correct!";

  EXPECT_FALSE(options.vertex_has_normal()) << "Wrong user options are returned!";
  EXPECT_FALSE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
  EXPECT_FALSE(options.vertex_has_color()) << "Wrong user options are returned!";
}

/*
 * Just load a ply file of a cube with vertex colors
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePLYWithVertexColors) {

    mesh_.clear();

    mesh_.request_vertex_colors();

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexColor;

    bool ok = OpenMesh::IO::read_mesh(mesh_, "cube-minimal-vertexColors.ply",options);

    EXPECT_TRUE(ok) << "Unable to load cube-minimal-vertexColors.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges())    << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces())    << "The number of loaded faces is not correct!";

    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(0))[0] ) << "Wrong vertex color at vertex 0 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[1] ) << "Wrong vertex color at vertex 0 component 1";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[2] ) << "Wrong vertex color at vertex 0 component 2";

    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(3))[0] ) << "Wrong vertex color at vertex 3 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(3))[1] ) << "Wrong vertex color at vertex 3 component 1";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(3))[2] ) << "Wrong vertex color at vertex 3 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(4))[0] ) << "Wrong vertex color at vertex 4 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(4))[1] ) << "Wrong vertex color at vertex 4 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(4))[2] ) << "Wrong vertex color at vertex 4 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[0] ) << "Wrong vertex color at vertex 7 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[1] ) << "Wrong vertex color at vertex 7 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(7))[2] ) << "Wrong vertex color at vertex 7 component 2";

    EXPECT_FALSE(options.vertex_has_normal()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.vertex_has_color()) << "Wrong user options are returned!";

    mesh_.release_vertex_colors();
}

/*
 * Just load a ply file of a cube with vertex colors
 */
TEST_F(OpenMeshReadWritePLY, LoadPLYFromMeshLabWithVertexColors) {

    mesh_.clear();

    mesh_.request_vertex_colors();

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexColor;

    bool ok = OpenMesh::IO::read_mesh(mesh_, "meshlab.ply",options);

    EXPECT_TRUE(ok) << "Unable to load meshlab.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges())    << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces())    << "The number of loaded faces is not correct!";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[0] ) << "Wrong vertex color at vertex 0 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[1] ) << "Wrong vertex color at vertex 0 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(0))[2] ) << "Wrong vertex color at vertex 0 component 2";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(3))[0] ) << "Wrong vertex color at vertex 3 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(3))[1] ) << "Wrong vertex color at vertex 3 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(3))[2] ) << "Wrong vertex color at vertex 3 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(4))[0] ) << "Wrong vertex color at vertex 4 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(4))[1] ) << "Wrong vertex color at vertex 4 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(4))[2] ) << "Wrong vertex color at vertex 4 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[0] ) << "Wrong vertex color at vertex 7 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[1] ) << "Wrong vertex color at vertex 7 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(7))[2] ) << "Wrong vertex color at vertex 7 component 2";

    EXPECT_FALSE(options.vertex_has_normal()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.vertex_has_color()) << "Wrong user options are returned!";

    mesh_.release_vertex_colors();
}

/*
 * Just read and write a binary ply file of a cube with vertex colors
 */
TEST_F(OpenMeshReadWritePLY, WriteAndReadBinaryPLYWithVertexColors) {

    mesh_.clear();

    mesh_.request_vertex_colors();

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexColor;

    bool ok = OpenMesh::IO::read_mesh(mesh_, "meshlab.ply",options);

    EXPECT_TRUE(ok) << "Unable to load meshlab.ply";

    options += OpenMesh::IO::Options::Binary;

    ok = OpenMesh::IO::write_mesh(mesh_, "meshlab_binary.ply",options);
    EXPECT_TRUE(ok) << "Unable to write meshlab_binary.ply";

    mesh_.clear();
    ok = OpenMesh::IO::read_mesh(mesh_, "meshlab_binary.ply",options);
    EXPECT_TRUE(ok) << "Unable to load meshlab_binary.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges())    << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces())    << "The number of loaded faces is not correct!";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[0] ) << "Wrong vertex color at vertex 0 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[1] ) << "Wrong vertex color at vertex 0 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(0))[2] ) << "Wrong vertex color at vertex 0 component 2";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(3))[0] ) << "Wrong vertex color at vertex 3 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(3))[1] ) << "Wrong vertex color at vertex 3 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(3))[2] ) << "Wrong vertex color at vertex 3 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(4))[0] ) << "Wrong vertex color at vertex 4 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(4))[1] ) << "Wrong vertex color at vertex 4 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(4))[2] ) << "Wrong vertex color at vertex 4 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[0] ) << "Wrong vertex color at vertex 7 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[1] ) << "Wrong vertex color at vertex 7 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(7))[2] ) << "Wrong vertex color at vertex 7 component 2";

    EXPECT_FALSE(options.vertex_has_normal()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.vertex_has_color()) << "Wrong user options are returned!";

    mesh_.release_vertex_colors();
}

/*
 * Just read and write a ply file of a cube with float vertex colors
 */
TEST_F(OpenMeshReadWritePLY, WriteAndReadPLYWithFloatVertexColors) {

    mesh_.clear();

    mesh_.request_vertex_colors();

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexColor;

    bool ok = OpenMesh::IO::read_mesh(mesh_, "meshlab.ply",options);

    EXPECT_TRUE(ok) << "Unable to load meshlab.ply";

    options += OpenMesh::IO::Options::ColorFloat;

    ok = OpenMesh::IO::write_mesh(mesh_, "meshlab_float.ply",options);
    EXPECT_TRUE(ok) << "Unable to write meshlab_float.ply";

    mesh_.clear();
    ok = OpenMesh::IO::read_mesh(mesh_, "meshlab_float.ply",options);
    EXPECT_TRUE(ok) << "Unable to load meshlab_float.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges())    << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces())    << "The number of loaded faces is not correct!";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[0] ) << "Wrong vertex color at vertex 0 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[1] ) << "Wrong vertex color at vertex 0 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(0))[2] ) << "Wrong vertex color at vertex 0 component 2";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(3))[0] ) << "Wrong vertex color at vertex 3 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(3))[1] ) << "Wrong vertex color at vertex 3 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(3))[2] ) << "Wrong vertex color at vertex 3 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(4))[0] ) << "Wrong vertex color at vertex 4 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(4))[1] ) << "Wrong vertex color at vertex 4 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(4))[2] ) << "Wrong vertex color at vertex 4 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[0] ) << "Wrong vertex color at vertex 7 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[1] ) << "Wrong vertex color at vertex 7 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(7))[2] ) << "Wrong vertex color at vertex 7 component 2";

    EXPECT_FALSE(options.vertex_has_normal()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.vertex_has_color()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.color_is_float()) << "Wrong user options are returned!";

    mesh_.release_vertex_colors();
}

/*
 * Just read and write a binary ply file of a cube with float vertex colors
 */
TEST_F(OpenMeshReadWritePLY, WriteAndReadBinaryPLYWithFloatVertexColors) {

    mesh_.clear();

    mesh_.request_vertex_colors();

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexColor;

    bool ok = OpenMesh::IO::read_mesh(mesh_, "meshlab.ply",options);

    EXPECT_TRUE(ok) << "Unable to load meshlab.ply";

    options += OpenMesh::IO::Options::ColorFloat;
    options += OpenMesh::IO::Options::Binary;

    ok = OpenMesh::IO::write_mesh(mesh_, "meshlab_binary_float.ply",options);
    EXPECT_TRUE(ok) << "Unable to write meshlab_binary_float.ply";

    mesh_.clear();
    ok = OpenMesh::IO::read_mesh(mesh_, "meshlab_binary_float.ply",options);
    EXPECT_TRUE(ok) << "Unable to load meshlab_binary_float.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges())    << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces())    << "The number of loaded faces is not correct!";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[0] ) << "Wrong vertex color at vertex 0 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(0))[1] ) << "Wrong vertex color at vertex 0 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(0))[2] ) << "Wrong vertex color at vertex 0 component 2";

    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(3))[0] ) << "Wrong vertex color at vertex 3 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(3))[1] ) << "Wrong vertex color at vertex 3 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(3))[2] ) << "Wrong vertex color at vertex 3 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(4))[0] ) << "Wrong vertex color at vertex 4 component 0";
    EXPECT_EQ(0,   mesh_.color(mesh_.vertex_handle(4))[1] ) << "Wrong vertex color at vertex 4 component 1";
    EXPECT_EQ(255,   mesh_.color(mesh_.vertex_handle(4))[2] ) << "Wrong vertex color at vertex 4 component 2";

    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[0] ) << "Wrong vertex color at vertex 7 component 0";
    EXPECT_EQ(0, mesh_.color(mesh_.vertex_handle(7))[1] ) << "Wrong vertex color at vertex 7 component 1";
    EXPECT_EQ(255, mesh_.color(mesh_.vertex_handle(7))[2] ) << "Wrong vertex color at vertex 7 component 2";

    EXPECT_FALSE(options.vertex_has_normal()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.vertex_has_color()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.color_is_float()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.is_binary()) << "Wrong user options are returned!";

    mesh_.release_vertex_colors();
}

/*
 * Just load a ply file of a cube with vertex texCoords
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePLYWithTexCoords) {

    mesh_.clear();

    mesh_.request_vertex_texcoords2D();

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexTexCoord;

    bool ok = OpenMesh::IO::read_mesh(mesh_, "cube-minimal-texCoords.ply",options);

    EXPECT_TRUE(ok) << "Unable to load cube-minimal-texCoords.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges())    << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces())    << "The number of loaded faces is not correct!";

    EXPECT_EQ(10,   mesh_.texcoord2D(mesh_.vertex_handle(0))[0] ) << "Wrong vertex color at vertex 0 component 0";
    EXPECT_EQ(10,   mesh_.texcoord2D(mesh_.vertex_handle(0))[1] ) << "Wrong vertex color at vertex 0 component 1";

    EXPECT_EQ(6,   mesh_.texcoord2D(mesh_.vertex_handle(2))[0] ) << "Wrong vertex color at vertex 2 component 0";
    EXPECT_EQ(6,   mesh_.texcoord2D(mesh_.vertex_handle(2))[1] ) << "Wrong vertex color at vertex 2 component 1";

    EXPECT_EQ(9,   mesh_.texcoord2D(mesh_.vertex_handle(4))[0] ) << "Wrong vertex color at vertex 4 component 0";
    EXPECT_EQ(9,   mesh_.texcoord2D(mesh_.vertex_handle(4))[1] ) << "Wrong vertex color at vertex 4 component 1";

    EXPECT_EQ(12,   mesh_.texcoord2D(mesh_.vertex_handle(7))[0] ) << "Wrong vertex color at vertex 7 component 0";
    EXPECT_EQ(12,   mesh_.texcoord2D(mesh_.vertex_handle(7))[1] ) << "Wrong vertex color at vertex 7 component 1";


    EXPECT_FALSE(options.vertex_has_normal()) << "Wrong user options are returned!";
    EXPECT_TRUE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_color()) << "Wrong user options are returned!";

    mesh_.release_vertex_texcoords2D();
}

/*
 * Just load a ply with normals, ascii mode
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePLYWithNormals) {

    mesh_.clear();

    mesh_.request_vertex_normals();

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::VertexNormal;

    bool ok = OpenMesh::IO::read_mesh(mesh_, "cube-minimal-normals.ply", options);

    EXPECT_TRUE(ok) << "Unable to load cube-minimal-normals.ply";

    EXPECT_EQ(8u  , mesh_.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(18u , mesh_.n_edges()) << "The number of loaded edges is not correct!";
    EXPECT_EQ(12u , mesh_.n_faces()) << "The number of loaded faces is not correct!";

    EXPECT_TRUE(options.vertex_has_normal()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_texcoord()) << "Wrong user options are returned!";
    EXPECT_FALSE(options.vertex_has_color()) << "Wrong user options are returned!";


    EXPECT_EQ(0, mesh_.normal(mesh_.vertex_handle(0))[0] ) << "Wrong normal at vertex 0 component 0";
    EXPECT_EQ(0, mesh_.normal(mesh_.vertex_handle(0))[1] ) << "Wrong normal at vertex 0 component 1";
    EXPECT_EQ(1, mesh_.normal(mesh_.vertex_handle(0))[2] ) << "Wrong normal at vertex 0 component 2";

    EXPECT_EQ(1, mesh_.normal(mesh_.vertex_handle(3))[0] ) << "Wrong normal at vertex 3 component 0";
    EXPECT_EQ(0, mesh_.normal(mesh_.vertex_handle(3))[1] ) << "Wrong normal at vertex 3 component 1";
    EXPECT_EQ(0, mesh_.normal(mesh_.vertex_handle(3))[2] ) << "Wrong normal at vertex 3 component 2";

    EXPECT_EQ(1, mesh_.normal(mesh_.vertex_handle(4))[0] ) << "Wrong normal at vertex 4 component 0";
    EXPECT_EQ(0, mesh_.normal(mesh_.vertex_handle(4))[1] ) << "Wrong normal at vertex 4 component 1";
    EXPECT_EQ(1, mesh_.normal(mesh_.vertex_handle(4))[2] ) << "Wrong normal at vertex 4 component 2";

    EXPECT_EQ(1, mesh_.normal(mesh_.vertex_handle(7))[0] ) << "Wrong normal at vertex 7 component 0";
    EXPECT_EQ(1, mesh_.normal(mesh_.vertex_handle(7))[1] ) << "Wrong normal at vertex 7 component 1";
    EXPECT_EQ(2, mesh_.normal(mesh_.vertex_handle(7))[2] ) << "Wrong normal at vertex 7 component 2";

    mesh_.release_vertex_normals();

}

/*
 * Just load a ply with custom properties, ascii mode
 */
TEST_F(OpenMeshReadWritePLY, LoadSimplePLYWithCustomProps) {

    PolyMesh mesh;

    OpenMesh::IO::Options options;
    options += OpenMesh::IO::Options::Custom;

    bool ok = OpenMesh::IO::read_mesh(mesh, "cube-minimal-custom_props.ply", options);

    EXPECT_TRUE(ok) << "Unable to load cube-minimal-custom_props.ply";

    EXPECT_EQ(8u  , mesh.n_vertices()) << "The number of loaded vertices is not correct!";
    EXPECT_EQ(12u , mesh.n_edges()) << "The number of loaded edges is not correct!";
    EXPECT_EQ(6u , mesh.n_faces()) << "The number of loaded faces is not correct!";

    OpenMesh::VPropHandleT<float> qualityProp;
    OpenMesh::VPropHandleT<unsigned int> indexProp;
    EXPECT_TRUE(mesh.get_property_handle(qualityProp,"quality")) << "Could not access quality property";
    EXPECT_TRUE(mesh.get_property_handle(indexProp,"index")) << "Could not access index property";

    if (!mesh.get_property_handle(qualityProp,"quality"))
      return;

    if (!mesh.get_property_handle(indexProp,"index"))
      return;

    //check index property
    for (unsigned i = 0; i < mesh.n_vertices(); ++i)
      EXPECT_EQ(i ,mesh.property(indexProp,OpenMesh::VertexHandle(i))) << "Vertex index at vertex " << i << " is wrong";

    //check quality property
    EXPECT_EQ(1.f,mesh.property(qualityProp,OpenMesh::VertexHandle(0))) << "Wrong quality value at Vertex 0";
    EXPECT_EQ(0.5f,mesh.property(qualityProp,OpenMesh::VertexHandle(1))) << "Wrong quality value at Vertex 1";
    EXPECT_EQ(0.7f,mesh.property(qualityProp,OpenMesh::VertexHandle(2))) << "Wrong quality value at Vertex 2";
    EXPECT_EQ(1.f,mesh.property(qualityProp,OpenMesh::VertexHandle(3))) << "Wrong quality value at Vertex 3";
    EXPECT_EQ(0.1f,mesh.property(qualityProp,OpenMesh::VertexHandle(4))) << "Wrong quality value at Vertex 4";
    EXPECT_EQ(0.f,mesh.property(qualityProp,OpenMesh::VertexHandle(5))) << "Wrong quality value at Vertex 5";
    EXPECT_EQ(2.f,mesh.property(qualityProp,OpenMesh::VertexHandle(6))) << "Wrong quality value at Vertex 6";
    EXPECT_EQ(5.f,mesh.property(qualityProp,OpenMesh::VertexHandle(7))) << "Wrong quality value at Vertex 7";

    //check for custom list properties

    OpenMesh::VPropHandleT< std::vector<int> > testValues;
    EXPECT_TRUE(mesh.get_property_handle(testValues,"test_values")) << "Could not access texcoords per face";
    if (!mesh.get_property_handle(testValues,"test_values"))
      return;

    EXPECT_EQ(2u,mesh.property(testValues,OpenMesh::VertexHandle(0)).size()) << "Wrong verctor size";

    EXPECT_EQ(1,mesh.property(testValues,OpenMesh::VertexHandle(0))[0]) << "Wrong list value at Vertex 0";
    EXPECT_EQ(4,mesh.property(testValues,OpenMesh::VertexHandle(1))[1]) << "Wrong list value at Vertex 1";
    EXPECT_EQ(5,mesh.property(testValues,OpenMesh::VertexHandle(2))[0]) << "Wrong list value at Vertex 2";
    EXPECT_EQ(8,mesh.property(testValues,OpenMesh::VertexHandle(3))[1]) << "Wrong list value at Vertex 3";
    EXPECT_EQ(9,mesh.property(testValues,OpenMesh::VertexHandle(4))[0]) << "Wrong list value at Vertex 4";
    EXPECT_EQ(12,mesh.property(testValues,OpenMesh::VertexHandle(5))[1]) << "Wrong list value at Vertex 5";
    EXPECT_EQ(13,mesh.property(testValues,OpenMesh::VertexHandle(6))[0]) << "Wrong list value at Vertex 6";
    EXPECT_EQ(16,mesh.property(testValues,OpenMesh::VertexHandle(7))[1]) << "Wrong list value at Vertex 7";

    OpenMesh::FPropHandleT< std::vector<float> > texCoordsPerFace;
    EXPECT_TRUE(mesh.get_property_handle(texCoordsPerFace,"texcoords")) << "Could not access texcoords per face";

    if (!mesh.get_property_handle(texCoordsPerFace,"texcoords"))
          return;

    for (Mesh::FaceIter f_iter = mesh.faces_begin(); f_iter != mesh.faces_end(); ++f_iter)
    {
      EXPECT_EQ(8u, mesh.property(texCoordsPerFace, *f_iter).size()) << "Texcoords per face container has wrong size on face: " << f_iter->idx();
      if (!mesh.property(texCoordsPerFace, *f_iter).empty())
      {
        EXPECT_EQ(1.0, mesh.property(texCoordsPerFace, *f_iter)[0]) << "Texcoords wrong on index 0 with face: " << f_iter->idx();
        EXPECT_EQ(1.0, mesh.property(texCoordsPerFace, *f_iter)[1]) << "Texcoords wrong on index 1 with face: " << f_iter->idx();
        EXPECT_EQ(-1.0f, mesh.property(texCoordsPerFace, *f_iter)[2]) << "Texcoords wrong on index 2 with face: " << f_iter->idx();
        EXPECT_EQ(-1.0f, mesh.property(texCoordsPerFace, *f_iter)[3]) << "Texcoords wrong on index 3 with face: " << f_iter->idx();
        EXPECT_EQ(0.0f, mesh.property(texCoordsPerFace, *f_iter)[4]) << "Texcoords wrong on index 4 with face: " << f_iter->idx();
        EXPECT_EQ(0.0f, mesh.property(texCoordsPerFace, *f_iter)[5]) << "Texcoords wrong on index 5 with face: " << f_iter->idx();
        EXPECT_EQ(-0.5f, mesh.property(texCoordsPerFace, *f_iter)[6]) << "Texcoords wrong on index 6 with face: " << f_iter->idx();
        EXPECT_EQ(-0.5f, mesh.property(texCoordsPerFace, *f_iter)[7]) << "Texcoords wrong on index 7 with face: " << f_iter->idx();
      }

    }



}
}
