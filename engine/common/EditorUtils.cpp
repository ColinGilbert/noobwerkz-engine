#include <Magick++.h>
#include <vector>

#include "EditorUtils.hpp"
#include "NoobUtils.hpp"
#include "Graphics.hpp"

// Returns true upon success, image is saved to "blended_texture.tga"

bool noob::editor_utils::blend_channels()
{
	Magick::InitializeMagick(NULL);
	
	try 
	{

		Magick::Image r("r.tga");
		Magick::Image g("g.tga");
		Magick::Image b("b.tga");
		Magick::Image a("a.tga");

		{
			std::stringstream ss;
			ss << "Texture blending: red channel rows = " << r.rows() << " columns = " << r.columns() << ", green channel rows = " << g.rows() << " columns = " << g.columns() << ", blue channel rows = " << b.rows() << " columns = " << b.columns() << ", alpha channel rows = " << a.rows() << " columns = " << a.columns();
			logger::log(ss.str());
		}

		/* if ((r.columns() != g.columns() != b.columns() != a.columns()) || (r.rows() != g.rows() != b.rows() != a.rows()))
		{
			logger::log("Cannot create blended texture as source images not the same size.");
			return false;
		} */
			// Use the size of any image, as the prior test ensured that they are all the same size
			unsigned int output_cols = r.columns();
			unsigned int output_rows = r.rows();

			// Add to vector in order to loop quickly.
			std::vector<Magick::Image> images;
			images.push_back(r);
			images.push_back(g);
			images.push_back(b);
			images.push_back(a);

			for (Magick::Image i : images)
			{
				i.quantizeColorSpace(Magick::GRAYColorspace);
				i.quantizeColors(256);
				i.quantize();
			}

			
			Magick::Color c(static_cast<uint8_t>(256),static_cast<uint8_t>(256),static_cast<uint8_t>(256),static_cast<uint8_t>(256));
			Magick::Geometry geom(output_cols, output_rows);
			Magick::Image output_image(geom, c);
			for (unsigned int x = 0; x < output_cols; ++x)
			{
				for (unsigned int y =0; y < output_rows; ++y)
				{
					Magick::ColorGray red_channel = r.pixelColor(x, y);
					Magick::ColorGray green_channel = g.pixelColor(x, y);
					Magick::ColorGray blue_channel = b.pixelColor(x, y);
					Magick::ColorGray alpha_channel = a.pixelColor(x, y);

					double red_channel_value = red_channel.shade();
					double green_channel_value = green_channel.shade();
					double blue_channel_value = blue_channel.shade(); 
					double alpha_channel_value = alpha_channel.shade();

					Magick::ColorRGB output_colour;
					output_colour.red(red_channel_value);
					output_colour.green(green_channel_value);
					output_colour.blue(blue_channel_value);
					output_colour.alpha(alpha_channel_value);
					//output_colour(red_channel_value, green_channel_value, blue_channel_value, alpha_channel_value);
					output_image.pixelColor(x, y, output_colour);
				}
			}
			output_image.write("blended_textures.tga");

			return true;

	}
	catch (std::exception e)
	{
		std::stringstream ss;
		ss << "Texture blending: Caught exception \"" << e.what() << "\"";
		logger::log(ss.str());
		return false;
	}

}
