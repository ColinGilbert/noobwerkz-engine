#pragma once
/*
#include "Logger.hpp"
#include "reactphysics3d.h"

namespace noob
{
	class physics_event_listener : public rp3d::EventListener
	{
		public:
			virtual void beginContact(const rp3d::ContactPointInfo& contact)
			{
			
				// fmt::MemoryWriter w;
				// w << "[PhysicsEventListener] beginContact() - " << log_info(contact);
				//logger::log(w.str());
			
			}

			virtual void newContact(const rp3d::ContactPointInfo& contact)
			{
			
				//fmt::MemoryWriter w;
				//w << "[PhysicsEventListener] newContact() - " << log_info(contact);
				//logger::log(w.str());
			
			}

			virtual void beginInternalTick()
			{
				// logger::log("[PhysicsEventListener] beginInternalTick");

			}

			virtual void endInternalTick()
			{
				// logger::log("[PhysicsEventListener] endInternalTick");

			}

		protected:
			std::string log_info(const rp3d::ContactPointInfo& contact)
			{
				fmt::MemoryWriter w;
				w << "[Contact] Normal = (" << contact.normal.x << ", " << contact.normal.y << ", " << contact.normal.z << "). Penetration depth = " << contact.penetrationDepth << ". Point 1 (local) = (" << contact.localPoint1.x << ", " << contact.localPoint1.y << ", " << contact.localPoint1.z << "). Point 2 (local) = (" << contact.localPoint2.x << ", " << contact.localPoint2.y << ", " << contact.localPoint2.z << ")";
				return w.str();
			}
	};
}
*/
