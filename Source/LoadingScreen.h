//
// COMP 371 Project
//
// Created by David-Etienne Pigeon on 8/6/2019.
//


#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GL/glew.h>

class LoadingScreen
{
public:
	LoadingScreen();
	static void Draw();
	static void ProgressBar(float percentage);
	~LoadingScreen();

private:
	//static ImVec4 clear_color;
};