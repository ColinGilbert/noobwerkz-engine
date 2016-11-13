#include "Physics.hpp"

void noob::physics::init(const noob::vec3& gravity, noob::duration timestep) noexcept(true)
{
}

void noob::physics::clear() noexcept(true)
{
}

void noob::physics::set_gravity(const noob::vec3&) noexcept(true)
{

}

void noob::physics::set_timestep(const noob::duration) noexcept(true)
{

}

noob::vec3 noob::physics::get_gravity() const noexcept(true)
{

}

noob::duration noob::physics::get_timestep() const noexcept(true)
{

}

noob::physics::body_handle noob::physics::add_body(noob::body_type, const noob::physics::shape_handle, float mass, const noob::vec3& position, const noob::versor& orientation, bool ccd) noexcept(true)
{
}

noob::physics::body_handle noob::physics::add_body(noob::body_type, const noob::physics::shape_handle, const noob::body::info&) noexcept(true)
{
}

noob::physics::ghost_handle noob::physics::add_ghost(const noob::physics::shape_handle, const noob::vec3&, const noob::versor&) noexcept(true)
{
}

void noob::physics::set_position(noob::physics::body_handle, const noob::vec3&) noexcept(true)
{
}

void noob::physics::set_orientation(noob::physics::body_handle, const noob::versor&) noexcept(true)
{
}

noob::vec3 noob::physics::get_position(noob::physics::body_handle) const noexcept(true)
{
}

noob::versor noob::physics::get_orientation(noob::physics::body_handle) const noexcept(true)
{
}

noob::mat4 noob::physics::get_transform(noob::physics::body_handle) const noexcept(true)
{
}

noob::physics::shape_handle noob::physics::get_shape(noob::physics::body_handle) const noexcept(true)
{
}

noob::vec3 noob::physics::get_linear_velocity(noob::physics::body_handle) const noexcept(true)
{
}

noob::vec3 noob::physics::get_angular_velocity(noob::physics::body_handle) const noexcept(true)
{
}

void noob::physics::set_position(noob::physics::ghost_handle, const noob::vec3&) noexcept(true)
{
}

void noob::physics::set_orientation(noob::physics::ghost_handle, const noob::versor&) noexcept(true)
{
}

noob::vec3 noob::physics::get_position(noob::physics::ghost_handle) const noexcept(true)
{
}

noob::versor noob::physics::get_orientation(noob::physics::ghost_handle) const noexcept(true)
{
}

noob::mat4 noob::physics::get_transform(noob::physics::ghost_handle) const noexcept(true)
{
}

noob::physics::shape_handle noob::physics::get_shape(noob::physics::ghost_handle) const noexcept(true)
{
}
