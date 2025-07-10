#include <gtest/gtest.h>
#include <memory>
#include "Graphics/Renderer.h"
#include "Graphics/Camera.h"
#include "Graphics/Shader.h"

using namespace Tempest;

class TestRenderer : public ::testing::Test {
protected:
    void SetUp() override {
        // Create renderer with Mock graphics API for testing
        GraphicsConfig config;
        config.api = "Mock";
        config.enableDebug = true;
        
        m_renderer = std::make_unique<Renderer>();
        m_camera = std::make_unique<Camera>();
    }
    
    void TearDown() override {
        if (m_renderer) {
            m_renderer->Shutdown();
        }
    }
    
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Camera> m_camera;
};

TEST_F(TestRenderer, Initialization) {
    GraphicsConfig config;
    config.api = "Mock";
    config.enableDebug = true;
    
    EXPECT_TRUE(m_renderer->Initialize(config));
    EXPECT_TRUE(m_renderer->IsInitialized());
}

TEST_F(TestRenderer, WindowCreation) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    WindowConfig windowConfig;
    windowConfig.title = "Test Window";
    windowConfig.width = 800;
    windowConfig.height = 600;
    windowConfig.fullscreen = false;
    
    m_renderer->CreateWindow(windowConfig);
    
    // Window should not close immediately
    EXPECT_FALSE(m_renderer->ShouldClose());
}

TEST_F(TestRenderer, ViewportSettings) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    // Test viewport setting
    m_renderer->SetViewport(0, 0, 800, 600);
    
    const auto& renderState = m_renderer->GetRenderState();
    EXPECT_EQ(renderState.viewportWidth, 800);
    EXPECT_EQ(renderState.viewportHeight, 600);
}

TEST_F(TestRenderer, ClearColor) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    Color testColor = Color::Red();
    m_renderer->SetClearColor(testColor);
    
    const auto& renderState = m_renderer->GetRenderState();
    EXPECT_EQ(renderState.clearColor.r, testColor.r);
    EXPECT_EQ(renderState.clearColor.g, testColor.g);
    EXPECT_EQ(renderState.clearColor.b, testColor.b);
    EXPECT_EQ(renderState.clearColor.a, testColor.a);
}

TEST_F(TestRenderer, BlendMode) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    m_renderer->SetBlendMode(BlendMode::Additive);
    
    const auto& renderState = m_renderer->GetRenderState();
    EXPECT_EQ(renderState.blendMode, BlendMode::Additive);
}

TEST_F(TestRenderer, DepthTest) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    m_renderer->EnableDepthTest(false);
    
    const auto& renderState = m_renderer->GetRenderState();
    EXPECT_FALSE(renderState.depthTestEnabled);
}

TEST_F(TestRenderer, MatrixTransforms) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0,0,3), glm::vec3(0,0,0), glm::vec3(0,1,0));
    glm::mat4 model = glm::mat4(1.0f);
    
    m_renderer->SetProjectionMatrix(projection);
    m_renderer->SetViewMatrix(view);
    m_renderer->SetModelMatrix(model);
    
    const auto& renderState = m_renderer->GetRenderState();
    EXPECT_EQ(renderState.projectionMatrix, projection);
    EXPECT_EQ(renderState.viewMatrix, view);
    EXPECT_EQ(renderState.modelMatrix, model);
}

TEST_F(TestRenderer, RenderingPipeline) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    WindowConfig windowConfig;
    windowConfig.width = 800;
    windowConfig.height = 600;
    m_renderer->CreateWindow(windowConfig);
    
    // Test basic rendering pipeline
    m_renderer->BeginFrame();
    m_renderer->Clear(Color::Black());
    m_renderer->EndFrame();
    
    // Should not have errors
    EXPECT_FALSE(m_renderer->CheckError());
}

TEST_F(TestRenderer, CameraIntegration) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    // Test camera integration
    m_camera->SetGameCamera();
    
    glm::mat4 viewMatrix = m_camera->GetViewMatrix();
    glm::mat4 projectionMatrix = m_camera->GetProjectionMatrix();
    
    m_renderer->SetViewMatrix(viewMatrix);
    m_renderer->SetProjectionMatrix(projectionMatrix);
    
    const auto& renderState = m_renderer->GetRenderState();
    EXPECT_EQ(renderState.viewMatrix, viewMatrix);
    EXPECT_EQ(renderState.projectionMatrix, projectionMatrix);
}

TEST_F(TestRenderer, ImmediateMode) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    // Test immediate mode rendering
    m_renderer->BeginImmediate();
    m_renderer->ImmediateVertex(glm::vec3(0, 0, 0), Color::White());
    m_renderer->ImmediateVertex(glm::vec3(1, 0, 0), Color::Red());
    m_renderer->ImmediateVertex(glm::vec3(0, 1, 0), Color::Green());
    m_renderer->EndImmediate();
    
    // Should not have errors
    EXPECT_FALSE(m_renderer->CheckError());
}

TEST_F(TestRenderer, ErrorHandling) {
    // Test error handling when not initialized
    EXPECT_FALSE(m_renderer->IsInitialized());
    
    // These should not crash
    m_renderer->Clear(Color::Black());
    m_renderer->SetViewport(0, 0, 800, 600);
    m_renderer->SetBlendMode(BlendMode::Alpha);
    
    // Error string should be empty when not initialized
    EXPECT_TRUE(m_renderer->GetErrorString().empty());
}

TEST_F(TestRenderer, ResourceManagement) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    // Test resource creation (these are stubs for now)
    uint32_t shader = m_renderer->CreateShader("", "");
    uint32_t texture = m_renderer->CreateTexture(256, 256);
    uint32_t mesh = m_renderer->CreateMesh({}, {});
    
    // Should return valid IDs (even if 0 for stubs)
    EXPECT_GE(shader, 0);
    EXPECT_GE(texture, 0);
    EXPECT_GE(mesh, 0);
    
    // Test resource deletion (should not crash)
    m_renderer->DeleteShader(shader);
    m_renderer->DeleteTexture(texture);
    m_renderer->DeleteMesh(mesh);
}

TEST_F(TestRenderer, GraphicsAPI) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    // Test graphics API access
    IGraphicsAPI* api = m_renderer->GetGraphicsAPI();
    EXPECT_NE(api, nullptr);
    
    // Test API type
    EXPECT_EQ(config.api, "Mock");
}

TEST_F(TestRenderer, CommandQueue) {
    GraphicsConfig config;
    config.api = "Mock";
    
    ASSERT_TRUE(m_renderer->Initialize(config));
    
    WindowConfig windowConfig;
    windowConfig.width = 800;
    windowConfig.height = 600;
    m_renderer->CreateWindow(windowConfig);
    
    // Test command queue functionality
    m_renderer->BeginFrame();
    
    // Queue some commands
    m_renderer->Clear(Color::Blue());
    m_renderer->SetViewport(100, 100, 600, 400);
    m_renderer->SetBlendMode(BlendMode::Alpha);
    m_renderer->EnableDepthTest(true);
    
    m_renderer->EndFrame();
    
    // Should not have errors
    EXPECT_FALSE(m_renderer->CheckError());
}

TEST_F(TestRenderer, ColorUtilities) {
    // Test color utilities
    Color white = Color::White();
    Color black = Color::Black();
    Color red = Color::Red();
    Color green = Color::Green();
    Color blue = Color::Blue();
    
    EXPECT_EQ(white.r, 1.0f);
    EXPECT_EQ(white.g, 1.0f);
    EXPECT_EQ(white.b, 1.0f);
    EXPECT_EQ(white.a, 1.0f);
    
    EXPECT_EQ(black.r, 0.0f);
    EXPECT_EQ(black.g, 0.0f);
    EXPECT_EQ(black.b, 0.0f);
    EXPECT_EQ(black.a, 1.0f);
    
    EXPECT_EQ(red.r, 1.0f);
    EXPECT_EQ(red.g, 0.0f);
    EXPECT_EQ(red.b, 0.0f);
    EXPECT_EQ(red.a, 1.0f);
    
    EXPECT_EQ(green.r, 0.0f);
    EXPECT_EQ(green.g, 1.0f);
    EXPECT_EQ(green.b, 0.0f);
    EXPECT_EQ(green.a, 1.0f);
    
    EXPECT_EQ(blue.r, 0.0f);
    EXPECT_EQ(blue.g, 0.0f);
    EXPECT_EQ(blue.b, 1.0f);
    EXPECT_EQ(blue.a, 1.0f);
} 