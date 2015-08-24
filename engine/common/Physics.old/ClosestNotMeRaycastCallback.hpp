#pragma once

#include <btBulletDynamicsCommon.h>

class btClosestNotMeRayResultCallback : public btCollisionWorld::ClosestRayResultCallback
{
	public:
		btClosestNotMeRayResultCallback(btRigidBody* me) : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0, 0.0, 0.0), btVector3(0.0, 0.0, 0.0))
	{
		m_me = me;
	}

		virtual btScalar AddSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			if (rayResult.m_collisionObject == m_me)
			{
				return 1.0;
			}

			return ClosestRayResultCallback::AddSingleResult(rayResult, normalInWorldSpace);
		}

	protected:
		btRigidBody* m_me;
};
