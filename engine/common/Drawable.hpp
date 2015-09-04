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
			drawable(const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::light>& _light, const std::shared_ptr<noob::reflectance>& _reflectance, const std::shared_ptr<noob::prepared_shaders::info>& _shading, const noob::vec3& _scale) :  model(_model), light(_light), reflectance(_reflectance), shading(_shading), scale(_scale) {}
		
			noob::model* get_model() const { return model.get(); }
			noob::light* get_light() const { return light.get(); }
			noob::reflectance* get_reflectance() const { return reflectance.get(); }
			noob::prepared_shaders::info* get_shading() const { return shading.get(); }
			noob::vec3 get_scale() const { return scale; }

			//void set_model(const std::shared_ptr<noob::model>& _model) { model = _model; }
			void set_light(const std::shared_ptr<noob::light>& _light) { light = _light; }
			void set_reflectance(const std::shared_ptr<noob::reflectance>& _reflectance) { reflectance = _reflectance; }
			void set_shading(const std::shared_ptr<noob::prepared_shaders::info>& _shading) { shading = _shading; }
			void set_scale(const noob::vec3& _scale) { scale = _scale; }

		protected:
			std::shared_ptr<noob::model> model;
			std::shared_ptr<noob::light> light;
			std::shared_ptr<noob::reflectance> reflectance;
			std::shared_ptr<noob::prepared_shaders::info> shading;
			noob::vec3 scale;
	};
}
