#include "NDOF.hpp"
#include "Logger.hpp"
#include <thread>
#include <stdlib.h>
//#include <signal.h>
#include <csignal>
#include <spnav.h>

void sig()
{
	spnav_close();
	noob::logger::log("[NDOF] libsnpav received SIGINT.");
}

void noob::ndof::run()
{	
	if(spnav_open()==-1)
	{
		logger::log("failed to connect to the space navigator daemon");
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
			//logger::log(fmt::format("[NDOF] Got motion event: t({0}, {1}, {2}) r({3}, {4}, {5})", sev.motion.x, sev.motion.y, sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz));
			}
			else
			{
			/* SPNAV_EVENT_BUTTON */
			//logger::log(fmt::format("[NDOF] Got button {0} event {1}", sev.button.press ? "press" : "release", sev.button.bnum));
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
		temp.movement = false;
	}

	else
	{
		temp.movement = true;
	}
	
	temp.translation = noob::vec3(x/ticks, y/ticks, z/ticks);
	temp.rotation = noob::vec3(rx/ticks, ry/ticks, rz/ticks);

	x = y = z = rx = ry = rz = 0;
	ticks = 0;
	
	return temp;
}
