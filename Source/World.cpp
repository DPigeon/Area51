//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 14/1/15
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include <stdlib.h>
#include <math.h>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler
// GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/common.hpp>
#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
#include <random>


#include "World.h"
#include "Renderer.h"
#include "ParsingHelper.h"

#include "StaticCamera.h"
#include "FirstPersonCamera.h"


#include "CubeModel.h"
#include "SphereModel.h"
#include "Animation.h"
#include <GLFW/glfw3.h>
#include "EventManager.h"

#include "BSpline.h"
#include "BSplineCamera.h"

using namespace std;
using namespace glm;

World* World::instance;

// Light Coefficients
const vec3 lightColor(1.0f, 1.0f, 1.0f);
const float lightKc = 0.05f;
const float lightKl = 0.02f;
const float lightKq = 0.002f;
const vec4 lightPosition(3.0f, 0.0f, 20.0f, 1.0f);


World::World()
{
    instance = this;
    
    // Setup Camera
    mCamera.push_back(new FirstPersonCamera(vec3(3.0f, 5.0f, 20.0f)));
    mCamera.push_back(new StaticCamera(vec3(3.0f, 30.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
    mCamera.push_back(new StaticCamera(vec3(0.5f,  0.5f, 5.0f), vec3(0.0f, 0.5f, 0.0f), vec3(0.0f, 1.0f, 0.0f)));
    mCurrentCamera = 0;
}

World::~World()
{
    // Models
    for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
    {
        delete *it;
    }
    
    mModel.clear();
    
    for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        delete *it;
    }
    
    mAnimation.clear();
    
    for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        delete *it;
    }
    
    mAnimationKey.clear();
    
    // Camera
    for (vector<Camera*>::iterator it = mCamera.begin(); it < mCamera.end(); ++it)
    {
        delete *it;
    }
    mCamera.clear();
}

World* World::GetInstance()
{
    return instance;
}

void World::Update(float dt)
{
    // User Inputs
    // 0 1 2 to change the Camera
    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_1 ) == GLFW_PRESS)
    {
        mCurrentCamera = 0;
    }
    else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_2 ) == GLFW_PRESS)
    {
        if (mCamera.size() > 1)
        {
            mCurrentCamera = 1;
        }
    }
    else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_3 ) == GLFW_PRESS)
    {
        if (mCamera.size() > 2)
        {
            mCurrentCamera = 2;
        }
		Renderer::SetShader(SHADER_PHONG);
    }
    
    // Spacebar to change the shader
    if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_0 ) == GLFW_PRESS)
    {
        Renderer::SetShader(SHADER_PHONG);
    }
    else if (glfwGetKey(EventManager::GetWindow(), GLFW_KEY_9 ) == GLFW_PRESS)
    {
        Renderer::SetShader(SHADER_SOLID_COLOR);
    }
    
    // Update animation and keys
    for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
    for (vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        (*it)->Update(dt);
    }

    mCamera[mCurrentCamera]->Update(dt);
    
    // Update models
    for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
    {
        (*it)->Update(dt);
    }
    
}

void World::Draw()
{
    Renderer::BeginFrame();
    
    glUseProgram(Renderer::GetShaderProgramID());
    
    // Everything we need to send to the GPU
    
    GLuint WorldMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");
    GLuint ViewMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewTransform");
    GLuint ProjMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ProjectionTransform");
    GLuint ViewProjMatrixID = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
    
    // Get a handle for Light Attributes uniform
    GLuint LightPositionID = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldLightPosition");
    GLuint LightColorID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightColor");
    GLuint LightAttenuationID = glGetUniformLocation(Renderer::GetShaderProgramID(), "lightAttenuation");
    
    GLuint MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
    
    // Draw the Vertex Buffer
    // Note this draws a unit Sphere
    // The Model View Projection transforms are computed in the Vertex Shader
    
    // Set shader constants
    glUniform4f(LightPositionID, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
    glUniform3f(LightColorID, lightColor.r, lightColor.g, lightColor.b);
    glUniform3f(LightAttenuationID, lightKc, lightKl, lightKq);
    
    // Send the view projection constants to the shader
    mat4 View = mCamera[mCurrentCamera]->GetViewMatrix();
    glUniformMatrix4fv(ViewMatrixID,  1, GL_FALSE,  &View[0][0]);
    
    mat4 Projection = mCamera[mCurrentCamera]->GetProjectionMatrix();
    glUniformMatrix4fv(ProjMatrixID,  1, GL_FALSE, &Projection[0][0]);
    
    mat4 ViewProjection = mCamera[mCurrentCamera]->GetViewProjectionMatrix();
    glUniformMatrix4fv(ViewProjMatrixID,  1, GL_FALSE, &ViewProjection[0][0]);
    
    // Draw models
    for (vector<Model*>::iterator it = mModel.begin(); it < mModel.end(); ++it)
    {
        if ((*it)->GetMaterialCoefficients().length() > 0){
            MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");
            glUniformMatrix4fv(WorldMatrixID, 1, GL_FALSE, &((*it)->GetWorldMatrix())[0][0]);
            float ka = 0.9f;
            float kd = 0.5f;
            float ks = 1.0f;
            float n = 50.0f;
            
            glUniform4f(MaterialID, ka, kd, ks, n);
        }
        (*it)->Draw();
    }
    
    unsigned int prevShader = Renderer::GetCurrentShader();
	Renderer::SetShader(SHADER_PHONG);
    glUseProgram(Renderer::GetShaderProgramID());
    
    //Draw the BSpline between all the planets here
	for (vector<BSpline*>::iterator it = mSpline.begin(); it < mSpline.end(); ++it) {
		MaterialID = glGetUniformLocation(Renderer::GetShaderProgramID(), "materialCoefficients");

		glUniformMatrix4fv(WorldMatrixID, 1, GL_FALSE, &((*it)->GetWorldMatrix())[0][0]);
		float ka = 0.9f;
		float kd = 0.5f;
		float ks = 1.0f;
		float n = 50.0f;

		glUniform4f(MaterialID, ka, kd, ks, n);
		(*it)->ConstructTracks(mSplineCamera.front()->GetExtrapolatedPoints());
	}

    Renderer::CheckForErrors();
    
    // Restore previous shader
    Renderer::SetShader((ShaderType) prevShader);
    
    Renderer::EndFrame();
}

void World::LoadScene(const char * scene_path)
{
    // Using case-insensitive strings and streams for easier parsing
    ci_ifstream input;
    input.open(scene_path, ios::in);
    
    // Invalid file
    if(input.fail() )
    {
        fprintf(stderr, "Error loading file: %s\n", scene_path);
        getchar();
        exit(-1);
    }
    
    ci_string item;
    while( std::getline( input, item, '[' ) )
    {
        ci_istringstream iss( item );
        
        ci_string result;
        if( std::getline( iss, result, ']') )
        {
            if( result == "cube" )
            {
                // Box attributes
                CubeModel* cube = new CubeModel();
                cube->Load(iss);
                mModel.push_back(cube);
            }
            else if( result == "sphere" )
            {
                PlanetModel* sphere = new PlanetModel();
                sphere->Load(iss);
                mModel.push_back(sphere);
            }
            else if ( result == "animationkey" )
            {
                AnimationKey* key = new AnimationKey();
                key->Load(iss);
                mAnimationKey.push_back(key);
            }
            else if (result == "animation")
            {
                Animation* anim = new Animation();
                anim->Load(iss);
                mAnimation.push_back(anim);
            }
			else if (result == "spline")
			{
				BSpline* planetTour = new BSpline();
				std::vector<Model*> planets = generatePlanets();
				mModel.insert(mModel.begin(), planets.begin(), planets.end());

				for (std::vector<Model*>::iterator it = planets.begin(); it < planets.end(); ++it) {
					planetTour->AddControlPoint(glm::vec3((*it)->GetPosition()));
				}

				planetTour->CreateVertexBuffer();

				// FIXME: This is hardcoded: replace last camera with spline camera
				mSpline.push_back(planetTour);
				mCamera.pop_back();

				// Add camera to traverse the spline
				BSplineCamera* SplineCamera = new BSplineCamera(planetTour, 1.0f); // 10.0 before
				mCamera.push_back(SplineCamera);
				mSplineCamera.push_back(SplineCamera);
			}
            else if ( result.empty() == false && result[0] == '#')
            {
                // this is a comment line
            }
        }
    }
    input.close();
    
    // Set Animation vertex buffers
    for (vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        // Draw model
        (*it)->CreateVertexBuffer();
    }
}

std::vector<Model*> World::generatePlanets(){
    std::vector<Model*> planetList;
    //Temporary number here
    for (int i = 0; i < 8; i++) {
        PlanetModel* randomSphere = new PlanetModel();
        randomSphere->SetPosition(vec3(randomFloat(0, 100.0f),randomFloat(10.0f, 100.0f),randomFloat(0.0f, 100.0f)));
        float planetScalingConstant = randomFloat(0.5f, 4.0f);
        randomSphere->SetScaling(vec3(planetScalingConstant,planetScalingConstant,planetScalingConstant));
        planetList.push_back(randomSphere);
    }
    return planetList;
}

//Creates many random points within a cubic area
GLuint randVAO(int& vertexCount, std::vector<float>& angles)
{
    float areaWidth = 50.0f;
    
    std::vector<glm::vec3> vertPos;
    std::vector<float> vertRot;
    
    std::default_random_engine rando;
    for (int i = 0; i != vertexCount; ++i)
    {
        //Create a random position
        glm::vec3 randomPosition((float)rando(), (float)rando(), (float)rando());
        randomPosition /= (rando.max() / areaWidth);
        randomPosition -= glm::vec3(areaWidth / 2.0f, areaWidth / 2.0f, 0.0f);
        randomPosition.z *= -1.0f;
        
        //Create a random angle
        float randomAngle = rando() / (rando.max() / 360.0f);
        angles.push_back(randomAngle);
        randomAngle = glm::radians(randomAngle);
        
        
        vertPos.push_back(randomPosition);
        vertRot.push_back(randomAngle);
    }
    
    vertexCount = vertPos.size();
    
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    
    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertPos.size() * sizeof(glm::vec3), &vertPos.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    
    //Angle VBO setup
    GLuint angles_VBO;
    glGenBuffers(1, &angles_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, angles_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertRot.size() * sizeof(float), &vertRot.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    
    return VAO;
}

void  World::generateStars()
{
    //     glEnable(GL_BLEND); //Enable transparency blending
    //    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Define transprency blending
    
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
    glEnable(GL_COLOR_MATERIAL);
    glColor3ub(255, 0, 0  );
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable(GL_TEXTURE_2D);
    //Draw Stars
    int vCount = 300;
    std::vector<float> angles(100);
    GLuint VAO = randVAO(vCount, angles);
    
    glEnable( GL_POINT_SMOOTH );
    glColor3b(1.0, 0, 0);
    glPointSize( 5 ); // must be added before glDrawArrays is called
    glBindVertexArray(VAO);
    for (int i = 0; i < 1000; i++) {
        glDrawArrays( GL_POINTS, i, 1 ); // draw the vertixes
    }
    glBindVertexArray(0);
    glDisable( GL_POINT_SMOOTH ); // stop the smoothing to make the points circular
    
    glDisable( GL_COLOR_MATERIAL );
    
    glEnd();
    glFinish();
}


float randomFloat(float min, float max)
{
    // this  function assumes max > min, you may want
    // more robust error checking for a non-debug build
    assert(max > min);
    float random = ((float) rand()) / (float) RAND_MAX;
    
    // generate (in your case) a float between 0 and (4.5-.78)
    // then add .78, giving you a float between .78 and 4.5
    float range = max - min;
    return (random*range) + min;
}

Animation* World::FindAnimation(ci_string animName)
{
    for(std::vector<Animation*>::iterator it = mAnimation.begin(); it < mAnimation.end(); ++it)
    {
        if((*it)->GetName() == animName)
        {
            return *it;
        }
    }
    return nullptr;
}

AnimationKey* World::FindAnimationKey(ci_string keyName)
{
    for(std::vector<AnimationKey*>::iterator it = mAnimationKey.begin(); it < mAnimationKey.end(); ++it)
    {
        if((*it)->GetName() == keyName)
        {
            return *it;
        }
    }
    return nullptr;
}

const Camera* World::GetCurrentCamera() const
{
    return mCamera[mCurrentCamera];
}
