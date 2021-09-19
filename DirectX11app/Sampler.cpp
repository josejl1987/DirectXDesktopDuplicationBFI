#include "Sampler.h"
#include "D3D11RenderManager.h"




Sampler::Sampler() 
{
	memset(&this->sampDesc, 0, sizeof(D3D11_SAMPLER_DESC));
}



