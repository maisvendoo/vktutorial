#include	<main.h>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

    VulkanEngine vkengine;

    vkengine.init();

    vkengine.run();

    vkengine.cleanup();
	
	return 0;
}
