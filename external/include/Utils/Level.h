#ifndef LEVEL_H
#define LEVEL_H

class Level
{

public:

	const glm::vec4 level1[6] = {
		glm::vec4(1,  -5,  1, 4),
	};

	Level(string wtf) {};

	bool collision(Camera camera) {
		glm::vec4 obstacle = level1[0];

		if (camera.Position.z > (obstacle.y - obstacle.z) && camera.Position.z < (obstacle.y + obstacle.z)) {
			std::cout << camera.Position.z << std::endl;
			return true;
		}
	}

private:

};
#endif