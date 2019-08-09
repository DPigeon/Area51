//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#pragma once


#include "ParsingHelper.h"
#include <vector>
#include "BSpline.h"

#include "Billboard.h"

class Camera;
class Model;
class Animation;
class AnimationKey;
class BSpline;
class BSplineCamera;
class ParticleSystem;
class ParticleDescriptor;

class World
{
public:
	World();
	~World();
	
    static World* GetInstance();

	void Update(float dt);
	void Draw();

	void LoadScene(const char * scene_path);
    Animation* FindAnimation(ci_string animName);
    AnimationKey* FindAnimationKey(ci_string keyName);

    const Camera* GetCurrentCamera() const;

    void AddBillboard(Billboard* b);
    void RemoveBillboard(Billboard* b);
    void AddParticleSystem(ParticleSystem* particleSystem);
    void RemoveParticleSystem(ParticleSystem* particleSystem);
    void AddParticleDescriptor(ParticleDescriptor* particleDescriptor);
    ParticleDescriptor* FindParticleDescriptor(ci_string name);

	void SetCurrentCamera(int cameraNumber);
	bool GetLoadingState();
	void SetLoadingState(bool state);
    int NumberOfPlanetsToGenerate();
    int NumberOfPlanetsGenerated() { return mNumberOfPlanetsGenerated; }
    
private:
    static World* instance;
    
	std::vector<Model*> mModel;
    std::vector<Animation*> mAnimation;
    std::vector<AnimationKey*> mAnimationKey;
	std::vector<Camera*> mCamera;
    std::vector<BSpline*> mSpline;
	std::vector<BSplineCamera*> mSplineCamera;
    int mNumberOfPlanetsGenerated;
    std::vector<Model*> generatePlanets();
    /**
     * Checks if the randomly generated planet could possibly overlap with any of
     * the generated planets
     *
     * @param planetRandomPoint The point to compare with all the others
     * @param planetPositions The list of points used to compare
     *
     * @return bool If the point would overlap or not
     */
    bool planetHasSpace(glm::vec3 planetRandomPoint, std::vector<glm::vec3> planetPositions);
	unsigned int mCurrentCamera;
    std::vector<ParticleSystem*> mParticleSystemList;
    std::vector<ParticleDescriptor*> mParticleDescriptorList;
    BillboardList* mpBillboardList;
	bool isLoading;
};

float randomFloat(float min, float max);
glm::vec3 randomSphericalCoordinatesToCartesian(float radius, glm::vec3 initialCenter);
