#include "EngineNix.hpp"

int main()
{
	std::unique_ptr<noob::engine_nix> engine = std::make_unique<noob::engine_nix>();
	bool result = engine->init();
	if (!result) return -1;
	while() {}

}
