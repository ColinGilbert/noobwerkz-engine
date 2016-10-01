/*
 * Copyright 2013 Jeremie Roy. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef CUBE_ATLAS_H_HEADER_GUARD
#define CUBE_ATLAS_H_HEADER_GUARD

/// Inspired from texture-atlas from freetype-gl (http://code.google.com/p/freetype-gl/)
/// by Nicolas Rougier (Nicolas.Rougier@inria.fr)
/// The actual implementation is based on the article by Jukka Jylänki : "A
/// Thousand Ways to Pack the Bin - A Practical Approach to Two-Dimensional
/// Rectangle Bin Packing", February 27, 2010.
/// More precisely, this is an implementation of the Skyline Bottom-Left
/// algorithm based on C++ sources provided by Jukka Jylänki at:
/// http://clb.demon.fi/files/RectangleBinPack/
#include <bx/bx.h>
#include <bgfx/bgfx.h>

struct atlas_region
{
	enum type
	{
		GRAY = 1, // 1 component
		BGRA8 = 4  // 4 components
	};

	uint16_t x, y;
	uint16_t width, height;
	uint32_t mask; //encode the region type, the face index and the component index in case of a gray region

	atlas_region::type get_type() const
	{
		return (atlas_region::type) ( (mask >> 0) & 0x0000000F);
	}

	uint32_t get_face_index() const
	{
		return (mask >> 4) & 0x0000000F;
	}

	uint32_t get_component_index() const
	{
		return (mask >> 8) & 0x0000000F;
	}

	void set_mask(atlas_region::type _type, uint32_t _faceIndex, uint32_t _componentIndex)
	{
		mask = (_componentIndex << 8) + (_faceIndex << 4) + (uint32_t)_type;
	}
};

class atlas
{
	public:
		/// create an empty dynamic atlas (region can be updated and added)
		/// @param textureSize an atlas creates a texture cube of 6 faces with size equal to (textureSize*textureSize * sizeof(RGBA))
		/// @param max_region_count maximum number of region allowed in the atlas
		atlas(uint16_t _texture_size, uint16_t _max_regions_count = 4096);

		/// initialize a static atlas with serialized data	(region can be updated but not added)
		/// @param textureSize an atlas creates a texture cube of 6 faces with size equal to (textureSize*textureSize * sizeof(RGBA))
		/// @param texture_buffer buffer of size 6*textureSize*textureSize*sizeof(uint32_t) (will be copied)
		/// @param region_count number of region in the atlas
		/// @param regionBuffer buffer containing the region (will be copied)
		/// @param max_region_count maximum number of region allowed in the atlas
		atlas(uint16_t _texture_size, const uint8_t* _texture_buffer, uint16_t _region_count, const uint8_t* _region_buffer, uint16_t _max_regions_count = 4096);
		~atlas();

		/// add a region to the atlas, and copy the content of mem to the underlying texture
		uint16_t add_region(uint16_t _width, uint16_t _height, const uint8_t* _bitmap_buffer, atlas_region::type _type = atlas_region::type::BGRA8, uint16_t outline = 0);

		/// update a preallocated region
		void update_region(const atlas_region& _region, const uint8_t* _bitmap_buffer);

		/// Pack the UV coordinates of the four corners of a region to a vertex buffer using the supplied vertex format.
		/// v0 -- v3
		/// |     |     encoded in that order:  v0,v1,v2,v3
		/// v1 -- v2
		/// @remark the UV are four signed short normalized components.
		/// @remark the x,y,z components encode cube uv coordinates. The w component encode the color channel if any.
		/// @param handle handle to the region we are interested in
		/// @param vertexBuffer address of the first vertex we want to update. Must be valid up to vertexBuffer + offset + 3*stride + 4*sizeof(int16_t), which means the buffer must contains at least 4 vertex includind the first.
		/// @param offset byte offset to the first uv coordinate of the vertex in the buffer
		/// @param stride stride between tho UV coordinates, usually size of a Vertex.
		void pack_uv(uint16_t _regionHandle, uint8_t* _vertex_buffer, uint32_t _offset, uint32_t _stride) const;
		void pack_uv(const atlas_region& _region, uint8_t* _vertex_buffer, uint32_t _offset, uint32_t _stride) const;

		/// Same as pack_uv but pack a whole face of the atlas cube, mostly used for debugging and visualizing atlas
		void pack_face_layer_uv(uint32_t _idx, uint8_t* _vertex_buffer, uint32_t _offset, uint32_t _stride) const;

		/// return the TextureHandle (cube) of the atlas
		bgfx::TextureHandle get_texture_handle() const
		{
			return m_texture_handle;
		}

		//retrieve a region info
		const atlas_region& get_region(uint16_t _handle) const
		{
			return m_regions[_handle];
		}

		/// retrieve the size of side of a texture in pixels
		uint16_t get_texture_size() const
		{
			return m_texture_size;
		}

		/// retrieve the usage ratio of the atlas
		//float getUsageRatio() const { return 0.0f; }

		/// retrieve the numbers of region in the atlas
		uint16_t get_region_count() const
		{
			return m_region_count;
		}

		/// retrieve a pointer to the region buffer (in order to serialize it)
		const atlas_region* get_region_buffer() const
		{
			return m_regions;
		}

		/// retrieve the byte size of the texture
		uint32_t get_texture_buffer_size() const
		{
			return 6 * m_texture_size * m_texture_size * 4;
		}

		/// retrieve the mirrored texture buffer (to serialize it)
		const uint8_t* get_texture_buffer() const
		{
			return m_texture_buffer;
		}

	private:
		void init();

		struct packed_layer;
		packed_layer* m_layers;
		atlas_region* m_regions;
		uint8_t* m_texture_buffer;

		uint32_t m_used_layers;
		uint32_t m_used_faces;

		bgfx::TextureHandle m_texture_handle;
		uint16_t m_texture_size;
		float m_texel_size;
		float m_texel_offset[2];

		uint16_t m_region_count;
		uint16_t m_max_region_count;
};

#endif // CUBE_ATLAS_H_HEADER_GUARD
