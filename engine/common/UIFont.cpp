#include <bx/timer.h>
#include <bx/string.h>
#include <bx/fpumath.h>

#include "NoobUtils.hpp"
#include "UIFont.hpp"
// #include "font_manager.h"
// #include "text_buffer_manager.h"



TrueTypeHandle loadTtf(FontManager* _fm, const char* _filePath)
{
	FILE* file = fopen(_filePath, "rb");
	if (NULL != file)
	{
		uint32_t size = (uint32_t)noob::utils::fsize(file);
		uint8_t* mem = (uint8_t*)malloc(size+1);
		size_t ignore = fread(mem, 1, size, file);
		// BX_UNUSED(ignore);
		fclose(file);
		mem[size-1] = '\0';
		TrueTypeHandle handle = _fm->createTtf(mem, size);
		free(mem);
		return handle;
	}
	TrueTypeHandle invalid = BGFX_INVALID_HANDLE;
	return invalid;
}


void noob::ui_font::init(const std::string& filename, size_t font_size, float win_width, float win_height)
{
	window_width = win_width;
	window_height = win_height;
	fontManager = new FontManager(1024);
	textBufferManager = new TextBufferManager(fontManager);

	ttf_handle = loadTtf(fontManager, filename.c_str());
	font_handle = fontManager->createFontByPixelSize(ttf_handle, 0, font_size);

	fontManager->preloadGlyph(font_handle, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!1234567890-=_+[]{}!@#$%^&*()`~,<>/?\\'\";:. \n");

	fontManager->destroyTtf(ttf_handle);

	transientText = textBufferManager->createTextBuffer(FONT_TYPE_ALPHA, BufferType::Transient);

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
	// bgfx::setState(BGFX_STATE_RGB_WRITE |BGFX_STATE_ALPHA_WRITE |BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA));
	textBufferManager->submitTextBuffer(transientText, view_id);
	bgfx::submit(view_id);
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
