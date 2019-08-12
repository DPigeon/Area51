//
// COMP 371 Assignment Framework
//
// Created by Nicolas Bergeron on 8/7/14.
// Updated by Gary Chang on 28/1/15
//
// Copyright (c) 2014-2015 Concordia University. All rights reserved.
//

#pragma once

#include "Camera.h"
#include "BSpline.h"

class BSplineCamera : public Camera
{
public:
    BSplineCamera(BSpline* spline, float speed);
    virtual ~BSplineCamera();

    virtual void Update(float dt);
    virtual glm::mat4 GetViewMatrix() const;

	void ExtrapolatePoints(glm::vec3 mPosition);
	std::vector<glm::vec3> GetExtrapolatedPoints();
	bool ComparePoints(std::vector<glm::vec3> points);
	bool GetSmallestDistance(glm::vec3 point, glm::vec3 nextPoint);

private:
    glm::vec3 mPosition;
    glm::vec3 mUp;

    BSpline* mpSpline;
    float mSplineParameterT;
    float mSpeed;

	std::vector<glm::vec3> mExtrapolatedPoints;
};
