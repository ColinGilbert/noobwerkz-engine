#include "NDOF.hpp"

#include <thread>
//#include <stdlib.h>
//#include <signal.h>
#include <csignal>
#include <spnav.h>

#include "Logger.hpp"
/*
void sig()
{
	spnav_close();
	noob::logger::log(noob::importance::WARNING, "[NDOF] libsnpav received SIGINT.");
}
*/

void noob::ndof::run()
{	
	if(spnav_open()==-1)
	{
		logger::log(noob::importance::ERROR, "[NDOF] Failed to connect to the space navigator daemon.");
		return;
	}

	std::thread t([this]()//, x, y, z, rx, ry, rz, sev]()
			{
			// std::signal(SIGINT, SIG_DFL);
			spnav_event sev;
			while(spnav_wait_event(&sev))
			{
			if(sev.type == SPNAV_EVENT_MOTION)
			{

			ticks++;

			x += sev.motion.x;
			y += sev.motion.y;
			z += sev.motion.z;
			rx += sev.motion.rx;
			ry += sev.motion.ry;
			rz += sev.motion.rz;

			 // logger::log(noob::importance::INFO, noob::concat("[NDOF] Got motion event: t", noob::to_string(sev.motion.x), ",", noob::to_string(sev.motion.y), ",", noob::to_string(sev.motion.z), " - r ", noob::to_string(sev.motion.rx), ",", noob::to_string(sev.motion.ry), ",", noob::to_string(sev.motion.rz)));
			}
			else
			{
				// SPNAV_EVENT_BUTTON 
				// logger::log(fmt::format("[NDOF] Got button {0} event {1}", sev.button.press ? "press" : "release", sev.button.bnum));
			}
			}

			spnav_close();

			});
	t.detach();

}


noob::ndof::data noob::ndof::get_data()
{
	if (ticks == 0)
	{
		ticks = 1;
	}

	noob::ndof::data temp;

	if (x == 0 && y == 0 && z == 0 && rx == 0 && ry == 0 && rz == 0)
	{
		temp.valid = false;
	}

	else
	{
		temp.valid = true;
	}

	temp.x = static_cast<float>(x)/static_cast<float>(ticks);
	temp.y = static_cast<float>(y)/static_cast<float>(ticks);
	temp.z = static_cast<float>(z)/static_cast<float>(ticks);
	temp.rx = static_cast<float>(rx)/static_cast<float>(ticks);
	temp,ry = static_cast<float>(ry)/static_cast<float>(ticks);
	temp.rz = static_cast<float>(rz)/static_cast<float>(ticks);

	x = y = z = rx = ry = rz = 0;
	ticks = 0;

	return temp;
}
