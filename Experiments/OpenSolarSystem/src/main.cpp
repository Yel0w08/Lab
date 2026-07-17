#include "raylib.h"
#include <iostream>
#include "Model/Planet.h" 
#pragma region imgui
#include "imgui.h"
#include "rlImGui.h"
#include "imguiThemes.h"
#pragma endregion

int main(void)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "OpenSolarSystem");

	Camera3D camera = { 0 };
	camera.position = { 0.0f, 80.0f, 80.0f };
	camera.target = { 0.0f, 0.0f, 0.0f };
	camera.up = { 0.0f, 1.0f, 0.0f };
	camera.fovy = 45.0f;
	camera.projection = CAMERA_PERSPECTIVE;

	float timeSpeedMultiplier = 5.0f;
	float currentDay = 0.0f;

#pragma region imgui
	rlImGuiSetup(true);
	imguiThemes::yellow();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontGlobalScale = 2;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.Colors[ImGuiCol_WindowBg].w = 0.5f;
	}
#pragma endregion

	while (!WindowShouldClose())
	{

		float dt = GetFrameTime();
		currentDay += dt * timeSpeedMultiplier;

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);

		float sunRadius = SUN_RADIUS_KM * SIZE_SCALE;
		DrawSphere({ 0.0f, 0.0f, 0.0f }, sunRadius, YELLOW);

		for (int i = 0; i < PLANET_COUNT; i++)
		{
			Planet& p = solarSystem[i];

			float orbitSpeed = 360.0f / p.orbitalPeriodDays;
			orbitAngles[i] += orbitSpeed * dt * timeSpeedMultiplier;

			float rotSpeed = 360.0f / (p.rotationPeriodHours / 24.0f);
			rotationAngles[i] += rotSpeed * dt * timeSpeedMultiplier;

			float distUnits = p.distanceFromSunKm * DISTANCE_SCALE;
			float radUnits = p.radiusKm * SIZE_SCALE;

			float rad = orbitAngles[i] * DEG2RAD;
			Vector3 pos = {
				cosf(rad) * distUnits,
				0.0f,
				sinf(rad) * distUnits
			};

			DrawSphereEx(pos, radUnits, 16, 16, p.color);
			DrawCircle3D({ 0,0,0 }, distUnits, { 1,0,0 }, 90.0f, Fade(GRAY, 0.3f));
		} // <-- accolade fermante manquante dans ta version

		DrawGrid(10, 1.0f);
		EndMode3D();

#pragma region imgui
		rlImGuiBegin();

		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);

		ImGui::BeginMainMenuBar();
		if (ImGui::MenuItem("Exit")) {
			CloseWindow();
		}
		if (ImGui::MenuItem("Info")) {
		}
		ImGui::EndMainMenuBar();

		ImGui::Begin("Info");

		if (ImGui::CollapsingHeader("Sun")) {
			ImGui::Text("Info about sun");
			ImGui::Text("blablabla.");
		}

		if (ImGui::CollapsingHeader("Earth")) {
			ImGui::Text("Info about earth");
			ImGui::Text("blablabla.");
		}

		ImGui::End();
#pragma endregion

#pragma region imgui
		rlImGuiEnd();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
#pragma endregion

		EndDrawing();
	}

#pragma region imgui
	rlImGuiShutdown();
#pragma endregion

	CloseWindow();
	return 0;
}