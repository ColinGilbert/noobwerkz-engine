/*
 * Copyright 2013 Jeremie Roy. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <bgfx/bgfx.h>

#include <limits.h> // INT_MAX
#include <limits>
#include <memory.h> // memset
#include <vector>

#include "cube_atlas.h"

class RectanglePacker
{
	public:
		RectanglePacker();
		RectanglePacker(uint32_t _width, uint32_t _height);

		/// non constructor initialization
		void init(uint32_t _width, uint32_t _height);

		/// find a suitable position for the given rectangle
		/// @return true if the rectangle can be added, false otherwise
		bool addRectangle(uint16_t _width, uint16_t _height, uint16_t& _outX, uint16_t& _outY);

		/// return the used surface in squared unit
		uint32_t getUsedSurface()
		{
			return m_usedSpace;
		}

		/// return the total available surface in squared unit
		uint32_t getTotalSurface()
		{
			return m_width * m_height;
		}

		/// return the usage ratio of the available surface [0:1]
		float getUsageRatio();

		/// reset to initial state
		void clear();

	private:
		int32_t fit(uint32_t _skylineNodeIndex, uint16_t _width, uint16_t _height);

		/// Merges all skyline nodes that are at the same level.
		void merge();

		struct Node
		{
			Node(int16_t _x, int16_t _y, int16_t _width) : x(_x), y(_y), width(_width)
			{
			}

			int16_t x;     //< The starting x-coordinate (leftmost).
			int16_t y;     //< The y-coordinate of the skyline level line.
			int32_t width; //< The line _width. The ending coordinate (inclusive) will be x+width-1.
		};


		uint32_t m_width;            //< width (in pixels) of the underlying texture
		uint32_t m_height;           //< height (in pixels) of the underlying texture
		uint32_t m_usedSpace;        //< Surface used in squared pixel
		std::vector<Node> m_skyline; //< node of the skyline algorithm
};

RectanglePacker::RectanglePacker() : m_width(0), m_height(0), m_usedSpace(0) {}

RectanglePacker::RectanglePacker(uint32_t _width, uint32_t _height) : m_width(_width), m_height(_height), m_usedSpace(0)
{
	// We want a one pixel border around the whole atlas to avoid any artefact when sampling texture
	m_skyline.push_back(Node(1, 1, uint16_t(_width - 2) ) );
}

void RectanglePacker::init(uint32_t _width, uint32_t _height)
{
	BX_CHECK(_width > 2, "_width must be > 2");
	BX_CHECK(_height > 2, "_height must be > 2");
	m_width = _width;
	m_height = _height;
	m_usedSpace = 0;

	m_skyline.clear();
	// We want a one pixel border around the whole atlas to avoid any artifact when sampling texture
	m_skyline.push_back(Node(1, 1, uint16_t(_width - 2) ) );
}

bool RectanglePacker::addRectangle(uint16_t _width, uint16_t _height, uint16_t& _outX, uint16_t& _outY)
{
	int best_height, best_index;
	int32_t best_width;
	Node* node;
	Node* prev;
	_outX = 0;
	_outY = 0;

	best_height = INT_MAX;
	best_index = -1;
	best_width = INT_MAX;
	for (uint16_t ii = 0, num = static_cast<uint16_t>(m_skyline.size()); ii < num; ++ii)
	{
		int32_t yy = fit(ii, _width, _height);
		if (yy >= 0)
		{
			node = &m_skyline[ii];
			if ( ( (yy + _height) < best_height)
					|| ( ( (yy + _height) == best_height) && (node->width < best_width) ) )
			{
				best_height = uint16_t(yy) + _height;
				best_index = ii;
				best_width = node->width;
				_outX = node->x;
				_outY = uint16_t(yy);
			}
		}
	}

	if (best_index == -1)
	{
		return false;
	}

	Node newNode(_outX, _outY + _height, _width);
	m_skyline.insert(m_skyline.begin() + best_index, newNode);

	for (uint16_t ii = uint16_t(best_index + 1), num = uint16_t(m_skyline.size() ); ii < num; ++ii)
	{
		node = &m_skyline[ii];
		prev = &m_skyline[ii - 1];
		if (node->x < (prev->x + prev->width) )
		{
			uint16_t shrink = uint16_t(prev->x + prev->width - node->x);
			node->x += shrink;
			node->width -= shrink;
			if (node->width <= 0)
			{
				m_skyline.erase(m_skyline.begin() + ii);
				--ii;
				--num;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}

	merge();
	m_usedSpace += _width * _height;

	return true;
}

float RectanglePacker::getUsageRatio()
{
	uint32_t total = m_width * m_height;
	if (total > 0)
	{
		return static_cast<float>(m_usedSpace) / static_cast<float>(total);
	}

	return 0.0f;
}

void RectanglePacker::clear()
{
	m_skyline.clear();
	m_usedSpace = 0;

	// We want a one pixel border around the whole atlas to avoid any artefact when sampling texture
	m_skyline.push_back(Node(1, 1, uint16_t(m_width - 2) ) );
}

int32_t RectanglePacker::fit(uint32_t _skylineNodeIndex, uint16_t _width, uint16_t _height)
{
	int32_t width = _width;
	int32_t height = _height;

	const Node& baseNode = m_skyline[_skylineNodeIndex];

	int32_t xx = baseNode.x, yy;
	int32_t widthLeft = width;
	int32_t ii = _skylineNodeIndex;

	if ( (xx + width) > (int32_t)(m_width - 1) )
	{
		return -1;
	}

	yy = baseNode.y;
	while (widthLeft > 0)
	{
		const Node& node = m_skyline[ii];
		if (node.y > yy)
		{
			yy = node.y;
		}

		if ( (yy + height) > (int32_t)(m_height - 1) )
		{
			return -1;
		}

		widthLeft -= node.width;
		++ii;
	}

	return yy;
}

void RectanglePacker::merge()
{
	Node* node;
	Node* next;
	uint32_t ii;

	for (ii = 0; ii < m_skyline.size() - 1; ++ii)
	{
		node = (Node*) &m_skyline[ii];
		next = (Node*) &m_skyline[ii + 1];
		if (node->y == next->y)
		{
			node->width += next->width;
			m_skyline.erase(m_skyline.begin() + ii + 1);
			--ii;
		}
	}
}

struct atlas::packed_layer
{
	RectanglePacker packer;
	atlas_region faceRegion;
};

atlas::atlas(uint16_t _texture_size, uint16_t _max_regions_count) : m_used_layers(0), m_used_faces(0), m_texture_size(_texture_size), m_region_count(0), m_max_region_count(_max_regions_count)
{
	BX_CHECK(_texture_size >= 64 && _texture_size <= 4096, "Invalid _texture_size %d.", _texture_size);
	BX_CHECK(_max_regions_count >= 64 && _max_regions_count <= 32000, "Invalid _max_regions_count %d.", _max_regions_count);

	init();

	m_layers = new packed_layer[24];
	for (int ii = 0; ii < 24; ++ii)
	{
		m_layers[ii].packer.init(_texture_size, _texture_size);
	}

	m_regions = new atlas_region[_max_regions_count];
	m_texture_buffer = new uint8_t[ _texture_size * _texture_size * 6 * 4 ];
	memset(m_texture_buffer, 0, _texture_size * _texture_size * 6 * 4);

	m_texture_handle = bgfx::createTextureCube(_texture_size
			, false
			, 1
			, bgfx::TextureFormat::BGRA8
			);
}

atlas::atlas(uint16_t _texture_size, const uint8_t* _texture_buffer, uint16_t _region_count, const uint8_t* _region_buffer, uint16_t _max_regions_count) : m_used_layers(24), m_used_faces(6), m_texture_size(_texture_size), m_region_count(_region_count), m_max_region_count(_region_count < _max_regions_count ? _region_count : _max_regions_count)
{
	BX_CHECK(_region_count <= 64 && _max_regions_count <= 4096, "_region_count %d, _max_regions_count %d", _region_count, _max_regions_count);

	init();

	m_regions = new atlas_region[_region_count];
	m_texture_buffer = new uint8_t[get_texture_buffer_size()];

	memcpy(m_regions, _region_buffer, _region_count * sizeof(atlas_region) );
	memcpy(m_texture_buffer, _texture_buffer, get_texture_buffer_size() );

	m_texture_handle = bgfx::createTextureCube(_texture_size, false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_NONE, bgfx::makeRef(m_texture_buffer, get_texture_buffer_size()));
}

atlas::~atlas()
{
	bgfx::destroyTexture(m_texture_handle);

	delete [] m_layers;
	delete [] m_regions;
	delete [] m_texture_buffer;
}

void atlas::init()
{
	m_texel_size = static_cast<float>(std::numeric_limits<uint16_t>::max()) / static_cast<float>(m_texture_size);
	float texelHalf = m_texel_size/2.0f;
	switch (bgfx::getRendererType() )
	{
		case bgfx::RendererType::Direct3D9:
			{
				m_texel_offset[0] = 0.0f;
				m_texel_offset[1] = 0.0f;
				break;
			}
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			{
				m_texel_offset[0] = texelHalf;
				m_texel_offset[1] = texelHalf;
				break;
			}
		default:
			{
				m_texel_offset[0] = texelHalf;
				m_texel_offset[1] = -texelHalf;
				break;
			}
	}
}


uint16_t atlas::add_region(uint16_t _width, uint16_t _height, const uint8_t* _bitmap_buffer, atlas_region::type _type, uint16_t outline)
{
	if (m_region_count >= m_max_region_count)
	{
		return UINT16_MAX;
	}

	uint16_t xx = 0;
	uint16_t yy = 0;
	uint32_t idx = 0;
	while (idx < m_used_layers)
	{
		if (m_layers[idx].faceRegion.get_type() == _type
				&&  m_layers[idx].packer.addRectangle(_width + 1, _height + 1, xx, yy) )
		{
			break;
		}

		idx++;
	}

	if (idx >= m_used_layers)
	{
		if ( (idx + _type) > 24
				|| m_used_faces >= 6)
		{
			return UINT16_MAX;
		}

		for (int ii = 0; ii < _type; ++ii)
		{
			atlas_region& region = m_layers[idx + ii].faceRegion;
			region.x = 0;
			region.y = 0;
			region.width = m_texture_size;
			region.height = m_texture_size;
			region.set_mask(_type, m_used_faces, ii);
		}

		m_used_layers += _type;
		m_used_faces++;

		if (!m_layers[idx].packer.addRectangle(_width + 1, _height + 1, xx, yy) )
		{
			return UINT16_MAX;
		}
	}

	atlas_region& region = m_regions[m_region_count];
	region.x = xx;
	region.y = yy;
	region.width = _width;
	region.height = _height;
	region.mask = m_layers[idx].faceRegion.mask;

	update_region(region, _bitmap_buffer);

	region.x += outline;
	region.y += outline;
	region.width -= (outline * 2);
	region.height -= (outline * 2);

	return m_region_count++;
}

void atlas::update_region(const atlas_region& _region, const uint8_t* _bitmap_buffer)
{
	uint32_t size = _region.width * _region.height * 4;
	if (0 < size)
	{
		const bgfx::Memory* mem = bgfx::alloc(size);
		memset(mem->data, 0, mem->size);
		if (_region.get_type() == atlas_region::type::BGRA8)
		{
			const uint8_t* inLineBuffer = _bitmap_buffer;
			uint8_t* outLineBuffer = m_texture_buffer + _region.get_face_index() * (m_texture_size * m_texture_size * 4) + ( ( (_region.y * m_texture_size) + _region.x) * 4);

			for (int yy = 0; yy < _region.height; ++yy)
			{
				memcpy(outLineBuffer, inLineBuffer, _region.width * 4);
				inLineBuffer += _region.width * 4;
				outLineBuffer += m_texture_size * 4;
			}

			memcpy(mem->data, _bitmap_buffer, mem->size);
		}
		else
		{
			uint32_t layer = _region.get_component_index();
			const uint8_t* inLineBuffer = _bitmap_buffer;
			uint8_t* outLineBuffer = (m_texture_buffer + _region.get_face_index() * (m_texture_size * m_texture_size * 4) + ( ( (_region.y * m_texture_size) + _region.x) * 4) );

			for (int yy = 0; yy < _region.height; ++yy)
			{
				for (int xx = 0; xx < _region.width; ++xx)
				{
					outLineBuffer[(xx * 4) + layer] = inLineBuffer[xx];
				}

				memcpy(mem->data + yy * _region.width * 4, outLineBuffer, _region.width * 4);
				inLineBuffer += _region.width;
				outLineBuffer += m_texture_size * 4;
			}
		}

		bgfx::updateTextureCube(m_texture_handle, 0, (uint8_t)_region.get_face_index(), 0, _region.x, _region.y, _region.width, _region.height, mem);
	}
}

void atlas::pack_face_layer_uv(uint32_t _idx, uint8_t* _vertex_buffer, uint32_t _offset, uint32_t _stride) const
{
	pack_uv(m_layers[_idx].faceRegion, _vertex_buffer, _offset, _stride);
}

void atlas::pack_uv(uint16_t _regionHandle, uint8_t* _vertex_buffer, uint32_t _offset, uint32_t _stride) const
{
	const atlas_region& region = m_regions[_regionHandle];
	pack_uv(region, _vertex_buffer, _offset, _stride);
}

static void write_uv(uint8_t* _vertex_buffer, int16_t _x, int16_t _y, int16_t _z, int16_t _w)
{
	uint16_t* xyzw = (uint16_t*)_vertex_buffer;
	xyzw[0] = _x;
	xyzw[1] = _y;
	xyzw[2] = _z;
	xyzw[3] = _w;
}

void atlas::pack_uv(const atlas_region& _region, uint8_t* _vertex_buffer, uint32_t _offset, uint32_t _stride) const
{
	int16_t x0 = (int16_t)( ( (float)_region.x * m_texel_size + m_texel_offset[0]) - float(INT16_MAX) );
	int16_t y0 = (int16_t)( ( (float)_region.y * m_texel_size + m_texel_offset[1]) - float(INT16_MAX) );
	int16_t x1 = (int16_t)( ( ( (float)_region.x + _region.width) * m_texel_size + m_texel_offset[0]) - float(INT16_MAX) );
	int16_t y1 = (int16_t)( ( ( (float)_region.y + _region.height) * m_texel_size + m_texel_offset[1]) - float(INT16_MAX) );
	int16_t ww = (int16_t)( (float(INT16_MAX) / 4.0f) * (float)_region.get_component_index() );

	_vertex_buffer += _offset;
	switch (_region.get_face_index() )
	{
		case 0: // +X
			x0 = -x0;
			x1 = -x1;
			y0 = -y0;
			y1 = -y1;
			write_uv(_vertex_buffer, INT16_MAX, y0, x0, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, INT16_MAX, y1, x0, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, INT16_MAX, y1, x1, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, INT16_MAX, y0, x1, ww); _vertex_buffer += _stride;
			break;

		case 1: // -X
			y0 = -y0;
			y1 = -y1;
			write_uv(_vertex_buffer, INT16_MIN, y0, x0, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, INT16_MIN, y1, x0, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, INT16_MIN, y1, x1, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, INT16_MIN, y0, x1, ww); _vertex_buffer += _stride;
			break;

		case 2: // +Y
			write_uv(_vertex_buffer, x0, INT16_MAX, y0, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x0, INT16_MAX, y1, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, INT16_MAX, y1, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, INT16_MAX, y0, ww); _vertex_buffer += _stride;
			break;

		case 3: // -Y
			y0 = -y0;
			y1 = -y1;
			write_uv(_vertex_buffer, x0, INT16_MIN, y0, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x0, INT16_MIN, y1, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, INT16_MIN, y1, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, INT16_MIN, y0, ww); _vertex_buffer += _stride;
			break;

		case 4: // +Z
			y0 = -y0;
			y1 = -y1;
			write_uv(_vertex_buffer, x0, y0, INT16_MAX, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x0, y1, INT16_MAX, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, y1, INT16_MAX, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, y0, INT16_MAX, ww); _vertex_buffer += _stride;
			break;

		case 5: // -Z
			x0 = -x0;
			x1 = -x1;
			y0 = -y0;
			y1 = -y1;
			write_uv(_vertex_buffer, x0, y0, INT16_MIN, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x0, y1, INT16_MIN, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, y1, INT16_MIN, ww); _vertex_buffer += _stride;
			write_uv(_vertex_buffer, x1, y0, INT16_MIN, ww); _vertex_buffer += _stride;
			break;
	}
}

