#include "gs_app.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <stdexcept>
#include <array>

namespace md
{

	struct SimplePushConstantData
	{
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};


	GSApp::GSApp()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	GSApp::~GSApp() { vkDestroyPipelineLayout(gsDevice.device(), pipelineLayout, nullptr); }


	void GSApp::run()
	{
		while (!gsWindow.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(gsDevice.device());
	}

	void GSApp::loadModels()
	{
		std::vector<GSModel::Vertex> vertices
		{
			{{0.0f, -0.5f}, {1.0f, 0.0f,0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		gsModel = std::make_unique<GSModel>(gsDevice, vertices);
	}

	void GSApp::createPipelineLayout()
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);


		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
		if (vkCreatePipelineLayout(gsDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("failed to create pipeline layout");
	}

	void GSApp::createPipeline()
	{
		assert(gsSwapChain != nullptr && "Cannot create swapchain before pipeline");

		

		PipelineConfigInfo pipelineConfig{};
		GSPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = gsSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		gsPipeline = std::make_unique<GSPipeline>(gsDevice, "D:/Visual Studio 2022 Projects/GameSaver/GameSaver/shaders/VertexShader.spv", "D:/Visual Studio 2022 Projects/GameSaver/GameSaver/shaders/PixelShader.spv", pipelineConfig);
	}

	void GSApp::recreateSwapChain()
	{
		auto extent = gsWindow.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = gsWindow.getExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(gsDevice.device());

		if (gsSwapChain == nullptr)
			gsSwapChain = std::make_unique<GSSwapChain>(gsDevice, extent);
		else
		{
			gsSwapChain = std::make_unique<GSSwapChain>(gsDevice, extent, std::move(gsSwapChain));
			if (gsSwapChain->imageCount() != commandBuffers.size())
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}


		// if renderpass compatible do nothing else
		createPipeline();
	}

	void GSApp::createCommandBuffers()
	{
		commandBuffers.resize(gsSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = gsDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(gsDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate command buffers");

	}

	void GSApp::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			gsDevice.device(),
			gsDevice.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void GSApp::recordCommandBuffer(int imageIndex)
	{

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin recording command buffer");


		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = gsSwapChain->getRenderPass();
		renderPassInfo.framebuffer = gsSwapChain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = gsSwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// VK_SUBPASS_CONTENTS_INLINE is used when not using secondary cmd buffers
		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(gsSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(gsSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0,0}, gsSwapChain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);


		gsPipeline->bind(commandBuffers[imageIndex]);
		gsModel->bind(commandBuffers[imageIndex]);

		for (int i = 0; i < 4; i++)
		{
			SimplePushConstantData push{};
			push.offset = { 0.0f, -0.4f + i * 0.25f };
			push.color = { 0.0f, 0.0f, 0.2f + 0.2f * i };

			vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
			gsModel->draw(commandBuffers[imageIndex]);
		}


		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
			throw std::runtime_error("failed to record command buffer");

	}

	void GSApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = gsSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("failed to acquire next swap chain image");


		recordCommandBuffer(imageIndex);
		result = gsSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || gsWindow.wasWindowResized())
		{
			gsWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS)
			throw std::runtime_error("failed to present swap chain image");
	}

}