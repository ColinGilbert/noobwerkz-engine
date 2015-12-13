#pragma once

#include "ShaderVariant.hpp"
#include "Model.hpp"
#include "Body.hpp"
#include "Logger.hpp"

namespace noob
{
	class prop
	{
		public:
			void init(noob::body* body_ptr, noob::model* drawable_ptr, noob::prepared_shaders::info* shading_info)
			{
				// logger::log("Prop init()");
				_body_ptr = body_ptr;
				_drawable_ptr = drawable_ptr;
				_shading_info = shading_info;
			}

			void set_shading(noob::prepared_shaders::info* shading_info)
			{
				_shading_info = shading_info;
			}

			noob::body* get_body() const
			{
				return _body_ptr;
			}

			noob::model* get_model() const
			{
				return _drawable_ptr;
			}

			noob::prepared_shaders::info* get_shading() const
			{
				return _shading_info;
			}

		protected:
			noob::body* _body_ptr;
			noob::model* _drawable_ptr;
			noob::prepared_shaders::info* _shading_info;
	};
}
