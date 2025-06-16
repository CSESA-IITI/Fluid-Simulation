#include <Triton/Triton.hpp>
#include <core/core.hpp>
#include <iostream>

int main() {
	Triton::Luna ren;

	Core::FluidSim sim(ren.getContext());

	ren.addObject(sim.quadObject);

	std::vector<VkDescriptorImageInfo> imageInfos(sim.m_computeTextures.size());

	for (size_t i = 0; i < imageInfos.size(); i++)
	{
		sim.m_computeTextures[i].getImageInfo(imageInfos[i]);
	}

	ren.InitializePipeline(imageInfos);

	ren.startLoop([&sim](int currentFrame) { return sim.drawFrame(currentFrame); });
}