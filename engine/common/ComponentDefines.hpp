#pragma once

#include "Component.hpp"
#include "Shape.hpp"
#include "Sphere.hpp"
#include "Box.hpp"
#include "Cylinder.hpp"
#include "Capsule.hpp"
#include "Cone.hpp"
#include "Plane.hpp"
#include "Convex.hpp"
#include "TriMesh.hpp"
#include "Body.hpp"
#include "BasicMesh.hpp"
#include "AnimatedModel.hpp"
#include "SkeletalAnim.hpp"
#include "Light.hpp"
#include "ShaderVariant.hpp"

namespace noob
{
	class body;
	
	typedef noob::component<std::unique_ptr<noob::animated_model>> model_component;
	typedef noob::component<std::unique_ptr<noob::basic_mesh>> mesh_component;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeleton_component;
	typedef noob::component<noob::light> light_component;
	typedef noob::component<noob::reflection> reflection_component;
	typedef noob::component<noob::prepared_shaders::info> shader_component;
	typedef noob::component<noob::shape> shape_component;
	typedef noob::component<noob::body> body_component;
}
