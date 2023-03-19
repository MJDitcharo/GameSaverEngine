#pragma once

#include "gs_device.h"
#include "gs_pipeline.h"
#include "gs_swap_chain.h"
#include "gs_window.h"
#include "gs_model.h"

// std
#include <memory>
#include <vector>

namespace md
{
	class GSApp
	{

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		GSApp();
		~GSApp();

		GSApp(const GSApp&) = delete;
		GSApp& operator=(const GSApp&) = delete;

		void run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);


		GSWindow gsWindow{ WIDTH, HEIGHT, "Game Saver" };
		GSDevice gsDevice{ gsWindow };
		std::unique_ptr<GSSwapChain> gsSwapChain;
		std::unique_ptr<GSPipeline> gsPipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<GSModel> gsModel;
	};
}