#include "Application.hpp"
#include <RuntimeCompiler/AUArray.h>
#include <RuntimeCompiler/BuildTool.h>
#include <RuntimeCompiler/ICompilerLogger.h>
#include <RuntimeCompiler/FileChangeNotifier.h>
#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeObjectSystem/ObjectFactorySystem/ObjectFactorySystem.h>
#include <RuntimeObjectSystem/RuntimeObjectSystem.h>
#include <RuntimeObjectSystem/IObject.h>

noob::app* noob::app::app_pointer = nullptr;

noob::app::app() : m_pCompilerLogger(0), m_pRuntimeObjectSystem(0), app_state(0)
{
	// logger::log("application()");
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	//droid_font = std::unique_ptr<noob::ui_font>(new noob::ui_font());
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
	prefix = std::unique_ptr<std::string>(new std::string("."));
}

void noob::app::OnConstructorsAdded()
{
	// This could have resulted in a change of object pointer, so release old and get new one.
	if(app_state)
	{
		IObject* pObj = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetObject(m_ObjectId);
		pObj->GetInterface(&app_state);
		if( 0 == app_state)
		{
			delete pObj;
			m_pCompilerLogger->LogError( "Error - no updateable interface found\n");
		}
	}
}

noob::app::~app()
{
	logger::log("~application()");
	app_pointer = nullptr;

	if( m_pRuntimeObjectSystem )
	{
		// clean temp object files
		m_pRuntimeObjectSystem->CleanObjectFiles();
	}
	if( m_pRuntimeObjectSystem && m_pRuntimeObjectSystem->GetObjectFactorySystem() )
	{
		m_pRuntimeObjectSystem->GetObjectFactorySystem()->RemoveListener(this);

		// delete object via correct interface
		IObject* pObj = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetObject( m_ObjectId );
		delete pObj;
	}

	delete m_pRuntimeObjectSystem;
	delete m_pCompilerLogger;
}


noob::app& noob::app::get()
{
	logger::log("application::get()");
	assert(app_pointer && "application not created!");
	return *app_pointer;
}


void noob::app::step()
{
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	uint64_t uNowNano = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	double delta = (uNowNano - time) * 0.000000001f;
	time = uNowNano;

	if (!paused)
	{
		app_state->update(delta);
	}

	app_state->draw(window_width, window_height, delta);
}


void noob::app::pause()
{
	paused = true;
}


void noob::app::resume()
{
	paused = false;
}

bool noob::app::init()
{
	logger::log("");
	// Initialise the RuntimeObjectSystem
	m_pRuntimeObjectSystem = new RuntimeObjectSystem;
	m_pCompilerLogger = new StdioLogSystem(); //ew StdioLogSystem();
	if( !m_pRuntimeObjectSystem->Initialise(m_pCompilerLogger, 0) )
	{
		m_pRuntimeObjectSystem = 0;
		return false;
	}
	m_pRuntimeObjectSystem->GetObjectFactorySystem()->AddListener(this);

	std::vector<std::string> loadables = { "DrawFunc", "InitFunc", "UpdateFunc"};


	for (std::string s : loadables)
	{
		// construct first object
		IObjectConstructor* pCtor = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetConstructor(s.c_str());
		if(pCtor)
		{
			IObject* pObj = pCtor->Construct();
			pObj->GetInterface(&app_state);
			if(0 == app_state)
			{
				std::string message;
				message += "Error - no updateable interface found for ";
				message += s;
				delete pObj;
				logger::log(message);
				return false;
			}
			m_ObjectId = pObj->GetObjectId();

		}
	}

	return app_state->init();
}

void noob::app::update(double delta)
{
	//check status of any compile
	if( m_pRuntimeObjectSystem->GetIsCompiledComplete() )
	{
		// load module when compile complete
		m_pRuntimeObjectSystem->LoadCompiledModule();
	}

	if( !m_pRuntimeObjectSystem->GetIsCompiling() )
	{
		logger::log("RuntimeCompiledCPP test. Woot.");
	}

	if (started == false)
	{

	}
	else started = true;


	app_state->update(delta);
}


/*
   void noob::app::draw(float width, float height, double delta)
   {
   app_state->draw(width, height, delta);
   }

*/
void noob::app::set_archive_dir(const std::string& filepath)
{

	{
		std::stringstream ss;
		ss << "setting archive dir(\"" << filepath << "\")";
		logger::log(ss.str());

	}	

	prefix = std::unique_ptr<std::string>(new std::string(filepath));

	{
		std::stringstream ss;
		ss << "archive dir = " << *prefix;
		logger::log(ss.str());
	}
}


void noob::app::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		{
			std::stringstream ss;
			ss << "Touch - pointerID " << pointerID << ", (" << x << ", " << y << "), action " << action;
			logger::log(ss.str());
		}

		if (pointerID < 5)
		{
			finger_positions[pointerID] = noob::vec2(x,y);
		}
	}
	else input_has_started = true;
}

void noob::app::window_resize(uint32_t w, uint32_t h)
{
	window_width = w;
	window_height = h;
	{
		std::stringstream ss;
		ss << "window_resize(" << window_width << ", " << window_height << ")";
		logger::log(ss.str());
	}

	if (window_height == 0) 
	{
		window_height = 1;
	}
}
