#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>

class GoodCamera {
	private:
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraRight = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::mat4 view;

	public:
		GoodCamera(float x, float y, float z) {
			cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		}

		void setPos(float x, float y, float z) {
			cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
		}

		void changePos(float x, float y, float z) {
			cameraPos = glm::vec3(cameraPos.x + x, cameraPos.y + y, cameraPos.z + z);
		}

		void setTarget(glm::vec3 target) {
			cameraTarget = glm::vec3(target);
		}

		void update() {
			cameraDirection = glm::normalize(cameraPos - cameraTarget);
			cameraRight = glm::normalize(glm::cross(up, cameraDirection));
			cameraUp = glm::cross(cameraDirection, cameraRight);

			view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
		}

		glm::mat4 getViewMatrix() {
			return view;
		}
};