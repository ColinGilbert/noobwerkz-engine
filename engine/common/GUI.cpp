#include "GUI.hpp"

// #define GUI_FILL_LAYER_1 1
// #define GUI_FILL_LAYER_2 2
// #define GUI_FILL_LAYER_3 3
// #define GUI_TEXT_LAYER 4


bool noob::gui::init(const std::string& Prefix, const noob::vec2ui Dims, const noob::vec2d Dpi) noexcept(true)
{
	set_dims(Dims);

	noob::graphics& gfx = noob::get_graphics();

	billbuf = gfx.add_billboards(max_text_verts);

	if (!gui_font.init_library(noob::load_file_as_string(noob::concat(Prefix, /*"font/DroidSansMono.ttf" "font/Asimov.otf" */ "font/opendyslexic-3-regular.ttf")), Dpi, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890!@#$%^&*()-=_+`~[]{}|;:<>,./?'\"", 24))
	{
		return false;
	}

	// gui_font.add_feature(noob::font::feature::KERNING, true);
	// gui_font.add_feature(noob::font::feature::LIGATURE, false);
	// gui_font.add_feature(noob::font::feature::CONTEXTUAL_LIGATURE, false);

	return true;
}


void noob::gui::set_dims(const noob::vec2ui Dims) noexcept(true)
{
	dims = Dims;
}


void noob::gui::text(const std::string& Text, const noob::vec2f Pos, noob::gui::font_size Size, const noob::vec4f Colour) noexcept(true)
{
	auto shaped = gui_font.shape_text(Text);

	if (shaped.valid)
	{
		for (std::array<noob::billboard_vertex, 6> qq : shaped.value.quads)
		{
			uint32_t dbg_counter = 0;
			for (noob::billboard_vertex vv : qq)
			{
				noob::billboard_vertex temp(vv);
				temp.pos = screenspace_pos(Pos + vv.pos);
				temp.colour = Colour;
				drawlist.push_back(temp);
			}

			++num_quads;
		}
	}
}



void noob::gui::draw() noexcept(true)
{
	noob::graphics& gfx = noob::get_graphics();

	if (drawlist.size() < max_text_verts)
	{
		max_text_verts = drawlist.size();

		gfx.resize_buffers(billbuf, max_text_verts);
	}

	auto mapped_buf = gfx.map_billboards(billbuf, 0, drawlist.size());

	for (noob::billboard_vertex vv : drawlist)
	{
		noob::vec4f pos_and_tex;
		pos_and_tex[0] = vv.pos[0];
		pos_and_tex[1] = vv.pos[1];
		pos_and_tex[2] = vv.uv[0];
		pos_and_tex[3] = vv.uv[1];

		if (!mapped_buf.push_back(pos_and_tex))
		{
			noob::logger::log(noob::importance::ERROR, "[GUI] Could not push back position + uv vertex attribute.");
			break;
		}

		if (!mapped_buf.push_back(vv.colour))
		{
			noob::logger::log(noob::importance::ERROR, "[GUI] Could not push back colour vertex attribute.");
			break;
		}

	}

	gfx.unmap_buffer();

	gfx.draw_text(billbuf, gui_font.get_texture(), num_quads);

	drawlist.clear();
	num_quads = 0;
}

void noob::gui::debug_draw_atlas() const noexcept(true)
{


}


noob::vec2f noob::gui::screenspace_pos(const noob::vec2f Pos) const noexcept(true)
{
	noob::vec2f position = Pos - noob::vec2f(dims[0] / 2.0, dims[1] / 2.0);
	position[0] = position[0] / (static_cast<float>(dims[0]) / 2.0);
	position[1] = position[1] / (static_cast<float>(dims[1]) / 2.0);
	return position;
}

