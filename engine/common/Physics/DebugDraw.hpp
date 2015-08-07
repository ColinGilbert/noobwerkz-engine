#include <btBulletDynamicsCommon.h>
#include "Mesh.hpp"

namespace noob
{
	class debug_draw : public btIDebugDraw
	{

		public:
			virtual void drawLine(const btVector3&, const btVector3&, const btVector3&);

			// args: point_on_b, normal_on_b, distance, lifetime, colour
			virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&);

			virtual void reportErrorWarning(const char*);

			// args: location, text
			virtual void draw3dText(const btVector3&, const char*);

			virtual void setDebugMode(int);

			virtual int getDebugMode() const;

			void frame();


			struct text_info
			{
				noob::vec2 pos;
				std::string text;
			};

			struct line_info
			{
				noob::vec3 from;
				noob::vec3 to;
				noob::vec3 colour;
			};
			// Tuple is from point + to point + colour, refreshed every frame. User needs to setup drawing code in app. I recommend using a very thin cylinder shape + instanced drawing
			std::vector<line_info> debug_lines;
			std::vector<text_info> debug_text;
		protected:
			int _debug_mode;

	};
}
