#include "Body.hpp"

#include "Globals.hpp"
#include "StringFuncs.hpp"


void noob::body::init(btDynamicsWorld* const dynamics_world, noob::body_type type_arg, const noob::shape& shape, float mass, const noob::vec3f& pos, const noob::versorf& orient, bool ccd) noexcept(true) 
{
	btTransform start_transform;

	start_transform.setRotation(btQuaternion(orient.q[0], orient.q[1], orient.q[2], orient.q[3]));
	start_transform.setOrigin(btVector3(pos.v[0], pos.v[1], pos.v[2]));

	float _mass = mass;

	type = type_arg;

	switch(type)
	{
		case(noob::body_type::DYNAMIC):
			{
				break;
			}
		case(noob::body_type::KINEMATIC):
			{
				_mass = 0.0;
				break;
			}
		case(noob::body_type::STATIC):
			{
				_mass = 0.0;
				break;
			}
		default:
			{
				break;
			}
	};


	btVector3 inertia(0.0, 0.0, 0.0);
	btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);

	if (type == noob::body_type::DYNAMIC)
	{
		shape.inner->calculateLocalInertia(_mass, inertia);
	}

	btRigidBody::btRigidBodyConstructionInfo ci(_mass, motion_state, shape.inner, inertia);
	inner = new btRigidBody(ci);

	set_ccd(ccd);

	dynamics_world->addRigidBody(inner);
	noob::globals& g = noob::get_globals();
	physics_valid = true;	
}


void noob::body::init(btDynamicsWorld* const dynamics_world, noob::body_type type_arg, const noob::shape& shape, const noob::body_info& _info) noexcept(true) 
{
	btTransform start_transform;
	// start_transform.setIdentity();
	start_transform.setRotation(btQuaternion(_info.orientation.q[0], _info.orientation.q[1], _info.orientation.q[2], _info.orientation.q[3]));
	start_transform.setOrigin(btVector3(_info.position.v[0], _info.position.v[1], _info.position.v[2]));
	btVector3 inertia(0, 0, 0);
	btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
	shape.inner->calculateLocalInertia(_info.mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(_info.mass, motion_state, shape.inner, inertia);
	inner = new btRigidBody(ci);
	inner->setFriction(_info.friction);
	inner->setRestitution(_info.restitution);
	inner->setAngularFactor(btVector3(_info.angular_factor.v[0], _info.angular_factor.v[1], _info.angular_factor.v[2]));
	inner->setLinearFactor(btVector3(_info.linear_factor.v[0], _info.linear_factor.v[1], _info.linear_factor.v[2]));
	inner->setLinearVelocity(btVector3(_info.linear_velocity.v[0], _info.linear_velocity.v[1], _info.linear_velocity.v[2]));
	inner->setAngularVelocity(btVector3(_info.angular_velocity.v[0], _info.angular_velocity.v[1], _info.angular_velocity.v[2]));
	set_ccd(_info.ccd);
	dynamics_world->addRigidBody(inner);	
	physics_valid = true;
}


noob::vec3f noob::body::get_position() const noexcept(true) 
{
	btTransform xform;
	inner->getMotionState()->getWorldTransform(xform);
	return vec3f_from_bullet(xform.getOrigin());
}


noob::versorf noob::body::get_orientation() const noexcept(true) 
{
	btTransform xform;
	inner->getMotionState()->getWorldTransform(xform);
	return versorf_from_bullet(xform.getRotation());
}


noob::vec3f noob::body::get_linear_velocity() const noexcept(true) 
{
	return vec3f_from_bullet(inner->getLinearVelocity());
}


noob::vec3f noob::body::get_angular_velocity() const noexcept(true) 
{
	return vec3f_from_bullet(inner->getAngularVelocity());
}


noob::mat4f noob::body::get_transform() const noexcept(true) 
{
	btTransform xform;
	inner->getMotionState()->getWorldTransform(xform);
	return mat4f_from_bullet(xform);
}


noob::body_info noob::body::get_info() const noexcept(true)
{
	noob::body_info results;

	float inv_mass = inner->getInvMass();
	if (inv_mass > 0.0)
	{
		results.mass = 1.0 / inv_mass;
	}
	else
	{
		results.mass = 0.0;
	}
	results.type = type;
	results.friction = inner->getFriction();
	results.restitution = inner->getRestitution();
	results.position = vec3f_from_bullet(inner->getCenterOfMassPosition());
	results.linear_velocity = vec3f_from_bullet(inner->getLinearVelocity());
	results.angular_velocity = vec3f_from_bullet(inner->getAngularVelocity());
	results.linear_factor = vec3f_from_bullet(inner->getLinearFactor());
	results.angular_factor = vec3f_from_bullet(inner->getAngularFactor());
	results.orientation = versorf_from_bullet(inner->getOrientation());
	results.ccd = ccd;

	return results;
}


std::string noob::body::get_debug_string() const noexcept(true) 
{
	return noob::concat("[Body] position ", noob::to_string(get_position()), ", orientation ", noob::to_string(get_orientation()), ", linear velocity ", noob::to_string(get_linear_velocity()), ", angular velocity ", noob::to_string(get_angular_velocity()));// << ", on ground? " << on_ground() << ", ray lambda  = " << ray_lambda; //<< " ray lambda # 2 = " << ray_lambda[1];
}


void noob::body::set_ccd(bool b) noexcept(true) 
{
	if (b == true)
	{
		btVector3 center;
		btScalar radius;
		inner->getCollisionShape()->getBoundingSphere(center, radius);
		inner->setCcdMotionThreshold(radius);
		ccd = true;
	}
}


bool noob::body::get_ccd_enabled() const noexcept(true)
{
	return ccd;
}


float noob::body::get_ccd_threshold() const noexcept(true)
{
	return inner->getCcdMotionThreshold();
}

void noob::body::set_user_index_1(uint32_t i) noexcept(true)
{
	inner->setUserIndex(static_cast<int>(i));
}


void noob::body::set_user_index_2(uint32_t i) noexcept(true)
{
	inner->setUserIndex2(static_cast<int>(i));
}


uint32_t noob::body::get_user_index_1() const noexcept(true)
{
	return static_cast<uint32_t>(inner->getUserIndex());
}


uint32_t noob::body::get_user_index_2() const noexcept(true)
{
	return static_cast<uint32_t>(inner->getUserIndex2());
}


uint32_t noob::body::get_shape_index() const noexcept(true)
{
	return inner->getCollisionShape()->getUserIndex();
}
