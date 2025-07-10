#include "GraphicsAPI.h"
#include "OpenGLAPI.h"
#include "MockGraphicsAPI.h"
#include <spdlog/spdlog.h>

namespace Tempest {

std::unique_ptr<IGraphicsAPI> CreateGraphicsAPI(const std::string& api) {
    if (api == "OpenGL" || api == "opengl") {
        spdlog::info("Creating OpenGL graphics API");
        return std::make_unique<OpenGLAPI>();
    }
    else if (api == "Mock" || api == "mock" || api == "Test" || api == "test") {
        spdlog::info("Creating Mock graphics API for testing");
        return std::make_unique<MockGraphicsAPI>();
    }
    // TODO: Add Vulkan support in future
    // else if (api == "Vulkan" || api == "vulkan") {
    //     return std::make_unique<VulkanAPI>();
    // }
    
    spdlog::warn("Unknown graphics API: {}, falling back to OpenGL", api);
    return std::make_unique<OpenGLAPI>();
}

} // namespace Tempest 