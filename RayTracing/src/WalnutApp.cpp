#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

#include <glm/gtc/type_ptr.hpp>

#include <execution>

using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer()
		: m_Camera(45.0f, 0.1f, 100.0f)
	{
		{
			Sphere sphere;
			sphere.Position = { 0.0f, 0.0f, 0.0f };
			sphere.Radius = 0.5f;
			sphere.MaterialIndex = 0;
			m_RenderScene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 1.0f, 0.5f, -1.0f };
			sphere.Radius = 1.0f;
			sphere.MaterialIndex = 1;
			m_RenderScene.Spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.Position = { 0.0f, -100.5f, 0.0f };
			sphere.Radius = 100.0f;
			sphere.MaterialIndex = 2;
			m_RenderScene.Spheres.push_back(sphere);
		}

		{
			Material& material = m_RenderScene.Materials.emplace_back();
			material.Albedo = { 0.47f, 0.79f, 0.72f };
			material.Roughness = 0.2f;
			material.Metallic = 0.0f;
		}

		{
			Material& material = m_RenderScene.Materials.emplace_back();
			material.Albedo = { 0.66f, 0.39f, 0.67f };
			material.Roughness = 0.2f;
			material.Metallic = 0.0f;
		}

		{
			Material& material = m_RenderScene.Materials.emplace_back();
			material.Albedo = { 0.13f, 0.12f, 0.18f };
			material.Roughness = 0.6f;
			material.Metallic = 0.0f;
		}
	}

	virtual void OnUpdate(float ts) override
	{
		if (m_Camera.OnUpdate(ts))
			m_Renderer.ResetFrameIndex();
	}
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("hardware_concurrency() = %d", std::thread::hardware_concurrency());
		ImGui::Text("Last render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render"))
			Render();

		ImGui::Checkbox("Accumulate", &m_Renderer.GetSettings().Accumulate);

		if (ImGui::Button("Reset"))
			m_Renderer.ResetFrameIndex();

		ImGui::End();

		ImGui::Begin("Scene");
		bool sceneChanged = false;

		for (size_t i = 0; i < m_RenderScene.Spheres.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Sphere #%d", i);

			Sphere& sphere = m_RenderScene.Spheres[i];
			sceneChanged |= ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
			sceneChanged |= ImGui::DragFloat("Radius", &sphere.Radius, 0.05f);
			sceneChanged |= ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, (int)m_RenderScene.Materials.size() - 1);

			ImGui::Separator();
			ImGui::PopID();
		}

		for (size_t i = 0; i < m_RenderScene.Materials.size(); i++)
		{
			ImGui::PushID(i);
			ImGui::Text("Material #%d", i);

			Material& material = m_RenderScene.Materials[i];
			sceneChanged |= ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
			sceneChanged |= ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
			sceneChanged |= ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);

			ImGui::Separator();
			ImGui::PopID();
		}

		if (sceneChanged)
			m_Renderer.ResetFrameIndex();
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image)
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() },
				ImVec2(0, 1), ImVec2(1, 0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render()
	{
		Timer timer;

		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_RenderScene, m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer m_Renderer;
	Camera m_Camera;
	RenderScene m_RenderScene;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

	float m_LastRenderTime = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracing";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				app->Close();

			ImGui::EndMenu();
		}
	});
	return app;
}