#include "Scenery.hpp"

#include "TransformHelper.hpp"

noob::scenery::scenery(btDiscreteDynamicsWorld* _world, const std::shared_ptr<noob::drawable>& _drawable, const noob::vec3& _position, const noob::versor& _orientation)
{
	logger::log("scenery - contructing");
	world = _world;
	logger::log("scenery - world pointer copied");
	drawable = _drawable;
	logger::log("scenery - drawable pointer copied");
	noob::model* model  = drawable->get_model();
	logger::log("scenery - got model");
	btTriangleMesh* phyz_mesh = new btTriangleMesh();
	logger::log("scenery - got trimesh");

	for (size_t i = 0; i < model->meshes[0].indices.size(); i += 3)
	{
		uint16_t index_1 = model->meshes[0].indices[i];
		uint16_t index_2 = model->meshes[0].indices[i+1];
		uint16_t index_3 = model->meshes[0].indices[i+2];

		std::array<float, 3> v1 = model->meshes[0].vertices[index_1].position;
		std::array<float, 3> v2 = model->meshes[0].vertices[index_2].position;
		std::array<float, 3> v3 = model->meshes[0].vertices[index_3].position;

		btVector3 bv1 = btVector3(v1[0], v1[1], v1[2]);
		btVector3 bv2 = btVector3(v2[0], v2[1], v2[2]);
		btVector3 bv3 = btVector3(v3[0], v3[1], v3[2]);
		phyz_mesh->addTriangle(bv1, bv2, bv3);
	}        

	shape = new btBvhTriangleMeshShape(phyz_mesh, true);
	btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(btQuaternion(_orientation.q[0], _orientation.q[1], _orientation.q[2], _orientation.q[3]), btVector3(_position.v[0], _position.v[1], _position.v[2])));
	btRigidBody::btRigidBodyConstructionInfo ci(0.0, motion_state, shape);
	body = new btRigidBody(ci);
	world->addRigidBody(body);
	
	noob::transform_helper t;
	t.translate(_position);
	t.rotate(_orientation);
	transform = t.get_matrix();
}
