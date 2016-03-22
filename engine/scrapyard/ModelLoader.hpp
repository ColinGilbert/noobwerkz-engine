#include <array>
#include <vector>
#include <set>
#include <unordered_map>
#include <iostream>
#include <fstream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <lemon/list_graph.h>

#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/skeleton_builder.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/offline/animation_builder.h>
#include <ozz/animation/runtime/animation.h>
#include <ozz/base/io/archive.h>
#include <ozz/base/io/stream.h>

#include <boost/filesystem.hpp>

#include <cereal/types/string.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/archives/binary.hpp>


#include "format.h"
//#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "TransformHelper.hpp"

namespace noob
{
	class model_loader
	{
		public:
			struct vertex
			{
				template <class Archive>
					void serialize( Archive & ar )
					{
						ar(position, normal, uv, tangent, bitangent, bone_indices, bone_weights);
					}
				vertex() : position({0.0f, 0.0f, 0.0f}), normal({0.0f, 0.0f, 0.0f}), uv({0.0f, 0.0f}), tangent({0.0f, 0.0f, 0.0f}), bitangent({0.0f, 0.0f, 0.0f}), bone_names({"", "", "", ""}), bone_indices({0, 0, 0, 0}), bone_weights({0.0f, 0.0f, 0.0f, 0.0f}) {}
				std::array<float, 3> position, normal;
				std::array<float, 2> uv;
				std::array<float, 3> tangent, bitangent;
				std::array<std::string, 4> bone_names;
				std::array<size_t, 4> bone_indices;
				std::array<float, 4> bone_weights;
			};

			struct mesh
			{
				template <class Archive>
					void serialize(Archive & ar)
					{
						ar(vertices, indices, dimensions, name);
					}

				std::vector<noob::model_loader::vertex> vertices;
				std::vector<uint16_t> indices;
				std::vector<std::string> bone_names;
				std::array<float,3> dimensions;
				std::string name;
			};

			template <class Archive>
				void serialize(Archive& ar)
				{
					ar(meshes);
				}

			class hierarchy
			{
				public:


					hierarchy(): _translation(_graph), _scale(_graph), _rotation(_graph), _name(_graph) {}

					void init(const aiScene* scene, const std::set<std::string>& bone_names);
					ozz::animation::offline::RawSkeleton make_raw_skeleton();
					void print_info();

				protected:

					std::vector<lemon::ListDigraph::Node> find_roots();
					void recursive_build(aiNode* current, const std::set<std::string>& bone_names);
					void link(aiNode* parent, aiNode* child);
					lemon::ListDigraph::Node add(aiNode* assimp_node);
					void recursive_ozz_helper(const lemon::ListDigraph::Node& n, ozz::animation::offline::RawSkeleton::Joint& caller_joint, size_t index);
					void recursive_print(lemon::ListDigraph::Node n);

					lemon::ListDigraph _graph;
					lemon::ListDigraph::NodeMap<std::array<float, 3>> _translation, _scale;
					lemon::ListDigraph::NodeMap<std::array<float, 4>> _rotation;
					lemon::ListDigraph::NodeMap<std::string> _name; 

					// For easy access
					std::map<aiNode*, lemon::ListDigraph::Node> nodes;
			};


			bool load(const aiScene* scene, const std::string& name);
			// static noob::graphics::mesh_vertex to_graphics_vert(const noob::model_loader::vertex&);
			std::string get_output_path() const;

		protected:
			std::vector<model_loader::mesh> meshes;
			std::string output_pathname;
	};
}

