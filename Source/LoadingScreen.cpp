//
// COMP 371 Project
//
// Created by David-Etienne Pigeon on 8/6/2019.
//

#include "LoadingScreen.h"
#include "TextureLoader.h"

#include <iostream>

using namespace std;
using namespace ImGui;

LoadingScreen::LoadingScreen() {
}

void LoadingScreen::Draw() {
	{
        int sizeLogo;
		#if defined(PLATFORM_OSX)
			int logo = TextureLoader::LoadTexture("Textures/Area51_Logo.png", sizeLogo);
		#else
			int logo = TextureLoader::LoadTexture("../Assets/Textures/Area51_Logo.png", sizeLogo); // Let's find a better logo
		#endif

			ImVec2 size = ImVec2(1024.0f, 768.0f);
			ImVec2 position = ImVec2(0.0f, 0.0f);
			ImVec2 logoSize = ImVec2(600.0f, 300.0f);
			ImVec2 progressBarPosition = ImVec2(185.0f, 625.0f);

			SetNextWindowSize(size);
			SetNextWindowPos(position);
			ImGui::Begin("Guide Tour of Area 51"); 

			ImVec2 logoCentered = ImVec2((GetWindowSize().x - logoSize.x) * 0.4f, (GetWindowSize().y - logoSize.y) * 0.4f);
			SetCursorPos(logoCentered);
			ImGui::Image(ImTextureID(logo), logoSize, ImVec2(0.0f, 0.0f), ImVec2(1, -1));

			SetCursorPos(progressBarPosition);
			ProgressBar();

			ImGui::End();
	}
}

void LoadingScreen::ProgressBar() {
	// 1. Start timer at 0 (for 5-15 seconds, increment the bar)
	// 2. Now at 60-70 percent, wait until comparedPoints returns true
	// 3. Put bar to 100%
	// 4. Open next window (navigation window will be for later)

	double maxRandom = 13.0f; // Set a constant random number for now (cannot determine how many points total on the spline at beginning cause we live extrapolate)
	double time = ImGui::GetTime();
	double percent = time / maxRandom;

	ImGui::ProgressBar(time / maxRandom, ImVec2(0.0f, 0.0f));
}

LoadingScreen::~LoadingScreen() {
}
