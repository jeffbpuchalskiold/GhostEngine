#include "Render.h"
#include "GhostEngine.h"




Render::Render()
{
}


Render::~Render()
{
}

void Render::createGraphicsPipelines()
{
	auto fragShaderCode = readFile("frag.spv");
	auto vertShaderCode = readFile("vert.spv");
}


