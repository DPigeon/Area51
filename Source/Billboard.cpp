//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 15/7/15.
//
// Copyright (c) 2014-2019 Concordia University. All rights reserved.
//

#include "Billboard.h"
#include "Renderer.h"
#include "World.h"
#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>


using namespace std;
using namespace glm;


// This is used for sorting the list of Billboards
bool CompareBillboardAlongZ::operator()(const Billboard* a, const Billboard* b)
{
    mat4 viewMatrix = World::GetInstance()->GetCurrentCamera()->GetViewMatrix();
    return ((viewMatrix*vec4(a->position, 1.0f)).z < (viewMatrix*vec4(b->position, 1.0f)).z);
}


BillboardList::BillboardList(unsigned int maxNumBillboards, int textureID)
: mTextureID(textureID), mMaxNumBillboards(maxNumBillboards)
{
    // Pre-allocate Vertex Buffer - 6 vertices by billboard (2 triangles)
    mVertexBuffer.resize(maxNumBillboards * 6);
    
    // Initialize vertices to white, with UVs taking all the texture
    for (int i=0; i<(int)mVertexBuffer.size(); ++i)
    {
        // White vertex
        mVertexBuffer[i].color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        
        // Normal on Z axis
        mVertexBuffer[i].normal = vec3(0.0f, 0.0f, 1.0f);
        
        // Texture coordinates
        switch(i%6)
        {
            case 0: mVertexBuffer[i].textureCoordinate = vec2(0.0f, 1.0f); break;
            case 1: mVertexBuffer[i].textureCoordinate = vec2(0.0f, 0.0f); break;
            case 2: mVertexBuffer[i].textureCoordinate = vec2(1.0f, 1.0f); break;
            case 3: mVertexBuffer[i].textureCoordinate = vec2(1.0f, 1.0f); break;
            case 4: mVertexBuffer[i].textureCoordinate = vec2(0.0f, 0.0f); break;
            case 5: mVertexBuffer[i].textureCoordinate = vec2(1.0f, 0.0f); break;
        }
    }
    
    // Create a vertex array
    glGenVertexArrays(1, &mVAO);
    glBindVertexArray(mVAO);
    
    // Upload Vertex Buffer to the GPU, keep a reference to it (mVertexBufferID)
    // Note the vertex buffer will change over time, we use GL_DYNAMIC_DRAW
    glGenBuffers(1, &mVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mVertexBuffer.size() * sizeof(BillboardVertex), &mVertexBuffer[0], GL_DYNAMIC_DRAW);
    Renderer::CheckForErrors();
    
    // 1st attribute buffer : vertex Positions
    glVertexAttribPointer(0,                // attribute. No particular reason for 0, but must match the layout in the shader.
                          3,                // size
                          GL_FLOAT,        // type
                          GL_FALSE,        // normalized?
                          sizeof(BillboardVertex), // stride
                          (void*)0        // array buffer offset
                          );
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : vertex normal
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(BillboardVertex),
                          (void*)sizeof(vec3)    // Normal is Offseted by vec3 (see class Vertex)
                          );
    glEnableVertexAttribArray(1);

    
    // 3rd attribute buffer : vertex color
    glVertexAttribPointer(2,
                          4,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(BillboardVertex),
                          (void*) (2* sizeof(vec3)) // Color is Offseted by 2 vec3 (see class Vertex)
                          );
    glEnableVertexAttribArray(2);

    // 3rd attribute buffer : texture coordinates
    glVertexAttribPointer(3,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(BillboardVertex),
                          (void*) (2* sizeof(vec3) + sizeof(vec4)) // texture coordinates are Offseted by 2 vec3 (see class Vertex) and a vec4
                          );
    glEnableVertexAttribArray(3);


}

BillboardList::~BillboardList()
{
	// Free the GPU from the Vertex Buffer
	glDeleteBuffers(1, &mVBO);
	glDeleteVertexArrays(1, &mVAO);

	mVertexBuffer.resize(0);
	mBillboardList.resize(0);
}

void BillboardList::AddBillboard(Billboard* b)
{
    mBillboardList.push_back(b);
    
    assert(mBillboardList.size() <= mVertexBuffer.size() / 6.0f);
}

void BillboardList::RemoveBillboard(Billboard* b)
{
    list<Billboard*>::iterator it = find(mBillboardList.begin(), mBillboardList.end(), b);
    assert((*it != nullptr));
    mBillboardList.remove(*it);
}

void BillboardList::Update(float dt)
{
    // Sort billboards according to their depth, the functor CompareBillboardAlongZ does the magic!
    CompareBillboardAlongZ comp;
    mBillboardList.sort(comp);
    
    // Set Data in Vertex Buffer
    unsigned long firstVertexIndex = 0;

    // Maybe the view matrix will be useful to align the billboards
    const Camera* cam = World::GetInstance()->GetCurrentCamera();
    mat4 viewMatrix = cam->GetViewMatrix();
    mat4 transposeViewMatrix = glm::transpose(viewMatrix);
    
    vec4 rright = transposeViewMatrix[0];
    vec4 rup = transposeViewMatrix[1];
    vec4 mLookAt = (transposeViewMatrix[2]);
    

    for (list<Billboard*>::iterator it = mBillboardList.begin(); it != mBillboardList.end(); ++it)
    {
        const Billboard* b = *it;

        // Colors
        mVertexBuffer[firstVertexIndex].color = mVertexBuffer[firstVertexIndex + 1].color = mVertexBuffer[firstVertexIndex +2].color = mVertexBuffer[firstVertexIndex + 3].color = mVertexBuffer[firstVertexIndex + 4].color = mVertexBuffer[firstVertexIndex + 5].color = b->color;

        
        // @TODO 5 - Align billboards with Camera plane
        //
        // For each billboard, update each vertex position and normals
        // Currently, positions are aligned with the X-Y plane, billboards must face the camera
        //
        // You must update the positions and normals for the 6 vertices below
        
//        A billboard includes a position (center), a color, a size, an angle and a color.
//        o A billboard renders with two triangles, 6 vertices in total, always facing the camera.
//        o You can extract the camera basis vectors from the view matrix, rotate these vectors by
//        the angle of the billboard along 𝑙&&𝑜&&&𝑜&&&𝑘&&&𝐴&&&𝑡⃑, offset vertices from center by half the size of
//        the billboard along the rotated right and up vectors.
//        o Billboards also include a normal that will later be used for lighting. It must be facing
//            towards the camera.
        
        
        // Normals
        mVertexBuffer[firstVertexIndex].normal = mVertexBuffer[firstVertexIndex + 1].normal = mVertexBuffer[firstVertexIndex +2].normal = mVertexBuffer[firstVertexIndex + 3].normal = mVertexBuffer[firstVertexIndex + 4].normal = mVertexBuffer[firstVertexIndex + 5].normal = glm::normalize(vec3(mLookAt[0], mLookAt[1], mLookAt[2])); // wrong...
        
        // First triangle
        // Top left
        mVertexBuffer[firstVertexIndex].position.x = b->position.x + 0.5f*b->size.x * (-rright[0] + rup[0])/2;
        mVertexBuffer[firstVertexIndex].position.y = b->position.y + 0.5f*b->size.y * (-rright[1] + rup[1])/2;
        mVertexBuffer[firstVertexIndex].position.z = b->position.z + 0.5f*b->size.x * (-rright[2] + rup[2])/2;
        
        // Bottom Left
        mVertexBuffer[firstVertexIndex + 1].position.x = b->position.x + 0.5f*b->size.x * (-rright[0] - rup[0])/2;
        mVertexBuffer[firstVertexIndex + 1].position.y = b->position.y + 0.5f*b->size.y * (-rright[1] - rup[1])/2;
        mVertexBuffer[firstVertexIndex + 1].position.z = b->position.z + 0.5f*b->size.x * (-rright[2] - rup[2])/2;
        
        // Top Right
        mVertexBuffer[firstVertexIndex + 2].position.x = b->position.x + 0.5f*b->size.x * (rright[0] + rup[0])/2;
        mVertexBuffer[firstVertexIndex + 2].position.y = b->position.y + 0.5f*b->size.y * (rright[1] + rup[1])/2;
        mVertexBuffer[firstVertexIndex + 2].position.z = b->position.z + 0.5f*b->size.x * (rright[2] + rup[2])/2;
        
        // Second Triangle
        // Top Right
        mVertexBuffer[firstVertexIndex + 3].position.x = b->position.x + 0.5f*b->size.x * (rright[0] + rup[0])/2;
        mVertexBuffer[firstVertexIndex + 3].position.y = b->position.y + 0.5f*b->size.y * (rright[1] + rup[1])/2;
        mVertexBuffer[firstVertexIndex + 3].position.z = b->position.z + 0.5f*b->size.y * (rright[2] + rup[2])/2;
        
        // Bottom Left
        mVertexBuffer[firstVertexIndex + 4].position.x = b->position.x + 0.5f*b->size.x * (-rright[0] - rup[0])/2;
        mVertexBuffer[firstVertexIndex + 4].position.y = b->position.y + 0.5f*b->size.y * (-rright[1] - rup[1])/2;
        mVertexBuffer[firstVertexIndex + 4].position.z = b->position.z + 0.5f*b->size.y * (-rright[2] - rup[2])/2;
        
        // Bottom Right
        mVertexBuffer[firstVertexIndex + 5].position.x = b->position.x + 0.5f*b->size.x * (rright[0] - rup[0])/2;
        mVertexBuffer[firstVertexIndex + 5].position.y = b->position.y + 0.5f*b->size.y * (rright[1] - rup[1])/2;
        mVertexBuffer[firstVertexIndex + 5].position.z = b->position.z + 0.5f*b->size.y * (rright[2] - rup[2])/2;

        
        // do not touch this...
        firstVertexIndex += 6;
    }
    
    Renderer::CheckForErrors();
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 6*sizeof(BillboardVertex)*mBillboardList.size(), (void*)&mVertexBuffer[0]);
}

void BillboardList::Draw()
{
    Renderer::CheckForErrors();

    
    // Set current shader to be the Textured Shader
    ShaderType oldShader = (ShaderType)Renderer::GetCurrentShader();
    
    Renderer::SetShader(SHADER_TEXTURED);
    glUseProgram(Renderer::GetShaderProgramID());

    Renderer::CheckForErrors();

    
    GLuint textureLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "mySamplerTexture");
    glActiveTexture(GL_TEXTURE0);

    Renderer::CheckForErrors();

    
    glBindTexture(GL_TEXTURE_2D, mTextureID);
    glUniform1i(textureLocation, 0);				// Set our Texture sampler to user Texture Unit 0

    
    Renderer::CheckForErrors();

    // This looks for the MVP Uniform variable in the Vertex Program
    GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "ViewProjectionTransform");
    
    // Send the view projection constants to the shader
    const Camera* currentCamera = World::GetInstance()->GetCurrentCamera();
    mat4 VP = currentCamera->GetViewProjectionMatrix();
    glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

    // Draw the Vertex Buffer
    // Note this draws a unit Cube
    // The Model View Projection transforms are computed in the Vertex Shader
    glBindVertexArray(mVAO);
    
    GLuint WorldMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(), "WorldTransform");

    // Billboard position are all relative to the origin
    mat4 worldMatrix(1.0f);
    glUniformMatrix4fv(WorldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) mBillboardList.size()*6); // 6 vertices by billboard
    
    Renderer::CheckForErrors();
    
    Renderer::SetShader(oldShader);
}
