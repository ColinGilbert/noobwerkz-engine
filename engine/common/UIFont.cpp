#include <bx/timer.h>
#include <bx/string.h>
#include <bx/fpumath.h>

#include "NoobUtils.hpp"
#include "UIFont.hpp"
// #include "font_manager.h"
// #include "text_buffer_manager.h"



TrueTypeHandle noob::ui_font::load_ttf(FontManager* _fm, const std::string& file_path)
{
	std::string mem = noob::utils::load_file_as_string(file_path);
	TrueTypeHandle handle = _fm->createTtf(reinterpret_cast<const unsigned char*>(mem.c_str()), mem.size());
	return handle;
	TrueTypeHandle invalid = BGFX_INVALID_HANDLE;
	return invalid;
}


void noob::ui_font::init(const std::string& filename, size_t font_size, float win_width, float win_height)
{
	window_width = win_width;
	window_height = win_height;
	fontManager = new FontManager(1024);
	textBufferManager = new TextBufferManager(fontManager);

	ttf_handle = load_ttf(fontManager, filename);
	font_handle = fontManager->createFontByPixelSize(ttf_handle, 0, font_size, FONT_TYPE_DISTANCE);

	fontManager->preloadGlyph(font_handle, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!1234567890-=_+[]{}!@#$%^&*()`~,<>/?\\'\";:. \n");

	fontManager->destroyTtf(ttf_handle);

	transientText = textBufferManager->createTextBuffer(FONT_TYPE_DISTANCE, BufferType::Transient);//FONT_TYPE_ALPHA, BufferType::Transient);

}


void noob::ui_font::draw_text(uint8_t view_id, const std::string& message, float x, float y)
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

	textBufferManager->clearTextBuffer(transientText);
	textBufferManager->setPenPosition(transientText, x, y);
	textBufferManager->appendText(transientText, font_handle, message.c_str());
	TextRectangle rect = textBufferManager->getRectangle(transientText);
	// bgfx::setState(BGFX_STATE_RGB_WRITE |BGFX_STATE_ALPHA_WRITE |BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
	textBufferManager->submitTextBuffer(transientText, view_id);
}


void noob::ui_font::set_colour(uint32_t colour)
{
	textBufferManager->setTextColor(transientText, colour);
}


void noob::ui_font::set_window_dims(float width, float height)
{
	window_width = width;
	window_height = height;
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

	textBufferManager->clearTextBuffer(transientText);
	textBufferManager->setPenPosition(transientText, x, y);
	textBufferManager->setTextColor
		textBufferManager->appendText(transientText, font_handle, message.c_str());
	// bgfx::setState(BGFX_STATE_RGB_WRITE |BGFX_STATE_ALPHA_WRITE |BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
	textBufferManager->submitTextBuffer(transientText, view_submit);
	bgfx::submit(view_id);
}
*/
