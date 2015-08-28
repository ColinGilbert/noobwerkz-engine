#include "CharacterController.hpp"
#include "Logger.hpp"

void noob::character_controller::init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::prepared_shaders::info>& _info, const noob::mat4& _transform, float _mass, float _width, float _height, float _max_speed)
{
	world = _world;
	mass = _mass;
	width = _width;
	height = _height;
	max_speed = _max_speed;
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&_transform.m[0]));
	prop.init(world, _model, _info, _transform);
	//rp3d::CapsuleShape capsule(width, height);
	//prop.body->addCollisionShape(capsule, rp3d::Transform::identity(), mass);
	rp3d::SphereShape sphere(width);
	prop.body->addCollisionShape(sphere, rp3d::Transform::identity(), mass);
	//prop.body->setType(rp3d::KINEMATIC);
	//prop.add_sphere(mass, (width + height)/2.0);
}
/*
//Capsule height is 1.5 and position is in center so distance to ground should be 0.5 + 0.75 = 1.25
//For some reason it keeps on bobbing
 
//glm::vec3 pos = glm::vec3(capsule->getWorldTransform().getOrigin().x(), capsule->getWorldTransform().getOrigin().z(), capsule->getWorldTransform().getOrigin().y());
 
//glm::vec3 springforce(0.0f);
 
bool Airborne = true;
btVector3 from(pos.x, pos.z, pos.y);
btVector3 to(pos.x, pos.z - 10, pos.y);
btCollisionWorld::AllHitsRayResultCallback res(from, to);
physics.dynamicsWorld->rayTest(from, to, res);
for (int i = 0; i < res.m_hitPointWorld.size(); i++) {
        if (res.m_collisionObjects[i] != capsule) {    
                btVector3 a = res.m_hitPointWorld[i];
                glm::vec3 hp(a.x(), a.z(), a.y());
                       
                float d = glm::distance(pos, hp);
                std::cout << d << "\n";
                if (d < 1.25) {
                        float stiffness = 8000.0f;
                        springforce = stiffness * glm::vec3(0, 0, 1) * glm::max(1.9f - d, -0.05f);
                        Airborne = false;
                        break;
                }
        }
}
 
capsule->applyCentralForce(btVector3(springforce.x, springforce.z, springforce.y));
*/
void noob::character_controller::update()
{
	noob::prop::info inf = prop.get_info();
	float ideal_distance = height / 2 + 0.5;
	bool airborne = true;
	noob::vec3 spring_force(0.0, 0.0, 0.0);
	noob::vec3 from(inf.position.v[0], inf.position.v[2], inf.position.v[1]);
	noob::vec3 to = from - noob::vec3(0.0, -10.0, 0.0);
	rp3d::Ray ray(rp3d::Vector3(from.v[0], from.v[1], from.v[2]), rp3d::Vector3(to.v[0], to.v[1], to.v[2]));
	noob::character_controller::raycast_callback ray_cb(&prop);
	world->raycast(ray, &ray_cb);
}


void noob::character_controller::step(float dt, bool forward, bool back, bool left, bool right, bool jump)
{
	
}
