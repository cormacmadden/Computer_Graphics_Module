#pragma once
#ifndef CAMERA_H
#define CAMERA_H
#define _USE_MATH_DEFINES

#pragma region INCLUDES 
// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.

// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>

// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations

// Project includes
#include "maths_funcs.h"
#include "Utils.h"
#include <vector>
#pragma endregion INCLUDES 

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 20.0f;
const float SENSITIVITY = 0.4f; //mouse is not very sensitive 

class Camera
{
public:
    vec3 Pos;
    vec3 Front;
    vec3 Up;
    vec3 Right;
    vec3 WorldUp;
    float Yaw;
    float Pitch = -70.0f;
    float MovementSpeed;
    float MouseSensitivity;

    //function to create cam at pos 0,0,10
    Camera(vec3 position = vec3(0.0f, 15.0f, 30.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY)
    {
        Pos = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCamVectors();
    }
    //function to return cameras viewpoint as mat4
    mat4 GetViewMatrix()
    {
        return look_at(Pos, Pos + Front, Up);
    }
    //function to handle change of cam position due to keyboard input 
    void ProcessKeyboard(MovementDir direction, float deltaTime)
    {
        float speed = MovementSpeed * deltaTime;
        //std::cout << deltaTime;
        if (direction == FORWARD)Pos += Front * speed;
        if (direction == BACKWARD)Pos -= Front * speed;
        if (direction == LEFT)Pos -= Right * speed;
        if (direction == RIGHT) Pos += Right * speed;
        if (direction == UP) Pos += WorldUp * speed;
        if (direction == DOWN) Pos -= WorldUp * speed;

    }

    //function to allow camera to move with mouse pos
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;//multiply offset by how sensitive mouse is
        yoffset *= MouseSensitivity;
        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch) //not allow camera to go behind us 
        {
            if (Pitch > 89.0f) Pitch = 89.0f;
            if (Pitch < -89.0f) Pitch = -89.0f;
        }
        updateCamVectors();
    }


    //function to convert degrees to radians
    float convert(float degree) {
        float pi = M_PI;
        return (degree * (pi / 180));
    }
private:

    //function to update cam's vectors 
    void updateCamVectors()
    {
        vec3 front;
        front.v[0] = cos(convert(Yaw)) * cos(convert(Pitch));
        front.v[1] = sin(convert(Pitch));
        front.v[2] = sin(convert(Yaw)) * cos(convert(Pitch));

        Front = normalise(front);
        Right = normalise(cross(Front, WorldUp));
        Up = normalise(cross(Right, Front));
    }


};
#endif