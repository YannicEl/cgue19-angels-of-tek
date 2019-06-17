#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <string>
#include <iostream>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	RESET
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera
{
public:

	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	int line = 3;
	float Yaw;
	float Pitch;


	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCamera();
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		//std::cout << Position.x << std::endl;
		//std::cout << Position.y << std::endl;
		//std::cout << Position.z << std::endl;
		//float velocity = MovementSpeed * deltaTime;
		float velocity = 2;
		if (direction == FORWARD) {
			Position += Front * MovementSpeed * deltaTime;
		}
		//if (direction == BACKWARD)
		//	Position -= Front * velocity;
		if (direction == LEFT) {
			if (line > 1){
				//std::cout << line;
				line -= 1;
				Position = glm::vec3(Position.x -1, Position.y, Position.z);
			}
		}
		if (direction == RIGHT) {
			if (line < 5) {
				//std::cout << line;
				line += 1;
				Position = glm::vec3(Position.x + 1, Position.y, Position.z);
			}
		}
		if (direction == RESET) {
			Position = glm::vec3(Position.x, Position.y, 3);
		}
	}

	void ProcessMouseScroll(float yoffset)
	{
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= 45.0f)
			Zoom = 45.0f;
		if (Zoom >= 1.0f && Zoom <= 45.0f)
			Zoom -= yoffset;
	}

private:
	void updateCamera()
	{
		glm::vec3 front;
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		Right = glm::normalize(glm::cross(Front, WorldUp)); 
		Up = glm::normalize(glm::cross(Right, Front));
	}
};
#endif