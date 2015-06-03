#include "UserData.hpp"
#include "UpdateFunc.hpp"

void UpdateFunc::update(double delta)
{
	noob::data->droid_font.set_window_dims(window_width, window_height);
	if (noob::data->started == false)
	{

	}
	else noob::data->started = true;
}

REGISTERCLASS(UpdateFunc);
