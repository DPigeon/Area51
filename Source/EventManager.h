//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once

struct GLFWwindow;

class EventManager
{
public:
	EventManager();
	~EventManager();
	static void Initialize();
	static void Shutdown();
	static void SetLoadingState(bool state);
	static bool GetLoadingState();

	static void Update();

	static float GetFrameTime();
	static bool ExitRequested();

	static GLFWwindow* GetWindow();
	static EventManager* GetInstance();

	static float GetMouseMotionX();
	static float GetMouseMotionY();

	static void EnableMouseCursor();
	static void DisableMouseCursor();
    static float GetRandomFloat(float min, float max);

private:
	static EventManager* instance;

	// Time
	static double sLastFrameTime;
	static float sFrameTime;

	// Mouse
	static double sLastMousePositionX;
	static float  sMouseDeltaX;
	static double sLastMousePositionY;
	static float  sMouseDeltaY;

	// Window
	static GLFWwindow* spWindow;

    bool isLoading;
};
