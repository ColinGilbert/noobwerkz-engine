#pragma once

#include <functional>
#include <memory>
#include <map>

//#include <PolyVox/PagedVolume.h>
//#include <PolyVox/FilePager.h>
#include <PolyVox/RawVolume.h>
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <PolyVox/Mesh.h>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "Logger.hpp"

namespace noob
{
	class voxel_world
	{
		public:

			// const size_t world_width = 512;
			// const size_t world_height = 512;
			// const size_t world_depth = 512;
			const size_t world_width, world_height, world_depth;

			voxel_world() : world_width(512), world_height(512), world_depth(512) {}
			void init()
			{
				world = std::make_unique<PolyVox::RawVolume<uint8_t>>(PolyVox::Region(0, 0, 0, world_width, world_height, world_depth));
			}


			void clear_world()
			{
				init();
			}


			void set(size_t x, size_t y, size_t z, uint8_t value)
			{
				world->setVoxel(x, y, z, value);
			}


			// TODO: is functor the correct name for this?
			void apply_to_region(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, std::function<uint8_t(size_t, size_t, size_t)> functor)
			{
				PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
				PolyVox::Region world_region = world->getEnclosingRegion();
				bounding_box.cropTo(world_region);

				size_t bb_lower_x = bounding_box.getLowerX();
				size_t bb_lower_y = bounding_box.getLowerY();
				size_t bb_lower_z = bounding_box.getLowerZ();
				size_t bb_upper_x = bounding_box.getUpperX();
				size_t bb_upper_y = bounding_box.getUpperY();
				size_t bb_upper_z = bounding_box.getUpperZ();


				for (size_t x = bb_lower_x; x < bb_upper_x; ++x)
				{
					for (size_t y = bb_lower_y; y < bb_upper_y; ++y)
					{
						for (size_t z = bb_lower_z; z < bb_upper_z; ++z)
						{
							set(x, y, z, functor(x, y, z));
						}
					}
				}
			}




			void sphere(size_t radius, size_t origin_x, size_t origin_y, size_t origin_z, bool fill)
			{
				logger::log(fmt::format("[VoxelWorld] sphere({0}, {1}, {2}, {3}, {4})", radius, origin_x, origin_y, origin_z, fill));
				PolyVox::Region bounding_box(origin_x - radius, origin_y - radius, origin_z - radius, origin_x + radius, origin_y + radius, origin_z + radius);
				PolyVox::Region world_region = world->getEnclosingRegion();
				bounding_box.cropTo(world_region);


				size_t lower_x = bounding_box.getLowerX();
				size_t lower_y = bounding_box.getLowerY();
				size_t lower_z = bounding_box.getLowerZ();
				size_t upper_x = bounding_box.getUpperX();
				size_t upper_y = bounding_box.getUpperY();
				size_t upper_z = bounding_box.getUpperZ();

				for (size_t x = lower_x; x < upper_x; ++x)
				{
					for (size_t y = lower_y; y < upper_y; ++y)
					{
						for (size_t z = lower_z; z < upper_z; ++z)
						{
							float distance_from_origin = std::sqrt(std::pow(static_cast<float>(x) - static_cast<float>(origin_x), 2) + std::pow(static_cast<float>(y) - static_cast<float>(origin_y), 2) + std::pow(static_cast<float>(z) - static_cast<float>(origin_z), 2));
							if (distance_from_origin < static_cast<float>(radius))
							{
								if (fill == true)
								{
									set(x, y, z, 255);
								}
								else
								{
									set(x, y, z, 0);
								}
							}
						}
					}
				}
			}


			void box(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z, bool fill)
			{
				logger::log(fmt::format("[VoxelWorld] box({0}, {1}, {2}, {3}, {4}, {5}, {6})", lower_x, lower_y, lower_z, upper_x, upper_y, upper_z, fill));
				PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
				PolyVox::Region world_region = world->getEnclosingRegion();
				bounding_box.cropTo(world_region);

				size_t bb_lower_x = bounding_box.getLowerX();
				size_t bb_lower_y = bounding_box.getLowerY();
				size_t bb_lower_z = bounding_box.getLowerZ();
				size_t bb_upper_x = bounding_box.getUpperX();
				size_t bb_upper_y = bounding_box.getUpperY();
				size_t bb_upper_z = bounding_box.getUpperZ();

				for (size_t x = bb_lower_x; x < bb_upper_x; ++x)
				{
					for (size_t y = bb_lower_y; y < bb_upper_y; ++y)
					{
						for (size_t z = bb_lower_z; z < bb_upper_z; ++z)
						{
							if (fill == true)
							{
								set(x, y, z, 255);
							}
							else
							{
								set(x, y, z, 0);
							}
						}
					}
				}
			}


			void cylinder(size_t radius, size_t height, size_t origin_x, size_t origin_y, size_t origin_z, noob::cardinal_axis axis, bool fill)
			{


				/*
				   PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);

				   for (size_t x = lower_x; x < upper_x; ++x)
				   {
				   for (size_t y = lower_y; y < upper_y; ++y)
				   {
				   for (size_t z = lower_z; z < upper_z; ++z)
				   {
				   float distance_from_origin = std::sqrt(std::pow(x - origin_x, 2) + std::pow(z - origin_z, 2));
				   if (distance_from_origin <= radius)
				   {
				   if (fill == true)
				   {
				   set(x, y, z, 255);
				   }
				   else
				   {
				   set(x, y, z, 0);
				   }
				   }
				   }
				   }
				   }
				   */
			}


			uint8_t get(size_t x, size_t y, size_t z) const
			{
				return world->getVoxel(x, y, z);
			}


			noob::basic_mesh extract() const
			{
				return extract_region(0, 0, 0, world_width, world_height, world_depth);
			}



			noob::basic_mesh extract_region(size_t lower_x, size_t lower_y, size_t lower_z, size_t upper_x, size_t upper_y, size_t upper_z) const
			{
				logger::log("[VoxelWorld] extracting region");
				noob::basic_mesh world_mesh;
				PolyVox::Region bounding_box(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
				PolyVox::Region world_region = world->getEnclosingRegion();
				bounding_box.cropTo(world_region);

				auto mesh = extractMarchingCubesMesh(&*world, bounding_box);
				auto decoded_mesh = PolyVox::decodeMesh(mesh);
				size_t num_indices = decoded_mesh.getNoOfIndices();
				size_t num_vertices = decoded_mesh.getNoOfVertices();

				for (size_t i = 0; i < num_vertices; i++)
				{
					auto vertex = decoded_mesh.getVertex(i);
					world_mesh.vertices.push_back(noob::vec3(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
					//auto vert = half_edges.add_vertex(TriMesh::Point(vertex.position.getX(), vertex.position.getY(), vertex.position.getZ()));
					//verts.push_back(vert);
				}

				for (size_t i = 0; i < num_indices; i++)
				{
					world_mesh.indices.push_back(decoded_mesh.getIndex(i));
				}

				//noob::basic_mesh normalized;
				//world_mesh.save("temp/extracted-temp.off");
				//normalized.load("temp/extracted-temp.off", "extracted-temp");
				//logger::log("[Voxels] Region extracted");
				//return normalized;
				world_mesh.normalize();
				return world_mesh;
			}

			// bool save(const std::string& filename) const;
			// std::unique_ptr<noob::voxel_world> clone() const;
		protected:
			//std::unique_ptr<PolyVox::PagedVolume<uint8_t>> world;
			//std::unique_ptr<PolyVox::FilePager<uint8_t>> pager;
			std::unique_ptr<PolyVox::RawVolume<uint8_t>> world;
			float x_min, y_min, z_min, x_max, y_max, z_max;
	};
}
