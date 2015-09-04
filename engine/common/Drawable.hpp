#pragma once
#include <memory>
#include "Light.hpp"
#include "Model.hpp"
#include "ShaderVariant.hpp"

namespace noob
{
	class drawable
	{
		public:
			drawable(noob::model* _model, noob::light* _light, noob::reflectance* _reflectance, noob::prepared_shaders::info* _shading, const noob::vec3& _scale) :  model(_model), light(_light), reflectance(_reflectance), shading(_shading), scale(_scale) {}
		
			noob::model* get_model() const { return model; }
			noob::light* get_light() const { return light; }
			noob::reflectance* get_reflectance() const { return reflectance; }
			noob::prepared_shaders::info* get_shading() const { return shading; }
			noob::vec3 get_scale() const { return scale; }

			//void set_model(const std::shared_ptr<noob::model>& _model) { model = _model; }
			void set_light(noob::light* _light) { light = _light; }
			void set_reflectance(noob::reflectance* _reflectance) { reflectance = _reflectance; }
			void set_shading(noob::prepared_shaders::info* _shading) { shading = _shading; }
			void set_scale(const noob::vec3& _scale) { scale = _scale; }

		protected:
			noob::model* model;
			noob::light* light;
			noob::reflectance* reflectance;
			noob::prepared_shaders::info* shading;
			noob::vec3 scale;
	};
}
