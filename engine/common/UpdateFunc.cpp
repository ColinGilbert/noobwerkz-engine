#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>

#include "UserData.hpp"

#include "UpdateFunc.hpp"

class UserUpdateFunc : public TInterface<IID_UPDATE, UpdateFunc>
{
	public:
		virtual void update(double delta)
		{
			noob::data->droid_font->set_window_dims(noob::data->window_width, noob::data->window_height);
			if (noob::data->started == false)
			{

			}
			else noob::data->started = true;
		}
};

REGISTERCLASS(UserUpdateFunc);
