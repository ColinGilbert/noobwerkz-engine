#include <bx/timer.h>
#include <bx/string.h>
#include <bx/fpumath.h>

#include "NoobUtils.hpp"
#include "UIFont.hpp"


void noob::ui_font::init(const std::string& filename, size_t font_size, float win_width, float win_height) noexcept(true)
{
	window_width = win_width;
	window_height = win_height;
	font_manager = new FontManager(1024);
	buffer = new text_buffer_manager(font_manager);

	ttf_handle = load_ttf(font_manager, filename);
	font_handle = font_manager->create_font_by_pixel_size(ttf_handle, 0, font_size, FONT_TYPE_ALPHA);

	font_manager->preload_glyph(font_handle, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890-=_+[]{}!@#$%^&*()`~,<>/?\\|'\";:. \n");

	font_manager->destroy_ttf(ttf_handle);

	text_to_draw = buffer->createTextBuffer(FONT_TYPE_ALPHA, BufferType::Transient);//FONT_TYPE_ALPHA, BufferType::Transient);
}


void noob::ui_font::draw_text(uint8_t view_id, const std::string& message, float x, float y) noexcept(true)
{
	float at[3] = { 0, 0, 0.0f };
	float eye[3] = { 0, 0, -1.0f };
	float view_matrix[16];
	bx::mtxLookAt(view_matrix, eye, at);
	const float centering = 0.5f;
	float ortho[16];
	bx::mtxOrtho(ortho, centering, window_width + centering, window_height + centering, centering, -1.0f, 1.0f);

	// bgfx::setViewClear(view_id, BGFX_CLEAR_COLOR);// | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL);
	bgfx::setViewTransform(view_id, view_matrix, ortho);
	bgfx::setViewRect(view_id, 0, 0, window_width, window_height);
	
	buffer->clearTextBuffer(text_to_draw);
	buffer->setPenPosition(text_to_draw, x, y);
	buffer->appendText(text_to_draw, font_handle, message.c_str());
	// TextRectangle rect = textBufferManager->getRectangle(text_to_draw);
	// bgfx::setState(BGFX_STATE_RGB_WRITE |BGFX_STATE_ALPHA_WRITE |BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
	buffer->submitTextBuffer(text_to_draw, view_id);
}


void noob::ui_font::set_colour(uint32_t colour) noexcept(true)
{
	buffer->setTextColor(text_to_draw, colour);
}


void noob::ui_font::set_window_dims(float width, float height) noexcept(true)
{
	window_width = width;
	window_height = height;
}


TrueTypeHandle noob::ui_font::load_ttf(FontManager* _fm, const std::string& file_path) noexcept(true)
{
	std::string mem = noob::utils::load_file_as_string(file_path);
	TrueTypeHandle handle = _fm->create_ttf(reinterpret_cast<const unsigned char*>(mem.c_str()), mem.size());
	return handle;
	TrueTypeHandle invalid = BGFX_INVALID_HANDLE;
	return invalid;
}





/*
void noob::ui_font::draw_text(uint8_t view_id, const std::string& message, float x, float y, uint32_t colour)
{

	float at[3] = { 0, 0, 0.0f };
	float eye[3] = { 0, 0, -1.0f };
	float view_matrix[16];
	bx::mtxLookAt(view_matrix, eye, at);
	const float centering = 0.5f;
	float ortho[16];
	bx::mtxOrtho(ortho, centering, window_width + centering, window_height + centering, centering, -1.0f, 1.0f);

	bgfx::setViewTransform(view_id, view_matrix, ortho);
	bgfx::setViewRect(view_id, 0, 0, window_width, window_height);

	textBufferManager->clearTextBuffer(text_to_draw);
	textBufferManager->setPenPosition(text_to_draw, x, y);
	textBufferManager->setTextColor
		textBufferManager->appendText(text_to_draw, font_handle, message.c_str());
	// bgfx::setState(BGFX_STATE_RGB_WRITE |BGFX_STATE_ALPHA_WRITE |BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
	textBufferManager->submitTextBuffer(text_to_draw, view_submit);
	bgfx::submit(view_id);
}
*/
