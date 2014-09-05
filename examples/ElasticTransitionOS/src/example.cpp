#include "oxygine-framework.h"
#include <ox-binder.h>

using namespace oxygine;

void example_preinit()
{
	ObjectScript::Oxygine::init();
}

void example_postinit()
{
	ObjectScript::Oxygine::postInit();
}

void example_init()
{
	ObjectScript::Oxygine::run();
}

void example_update()
{
	sleep(10);
}

void example_destroy()
{
	ObjectScript::Oxygine::shutdown();
}