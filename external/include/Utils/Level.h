#ifndef LEVEL_H
#define LEVEL_H

const glm::vec4 level1[] = {
	glm::vec4(1,  -5,  1, 4),
};

class Level
{
public:
	Level(string wtf) {};

	//glm::vec4* getLevels() {
	//	glm::vec4 level[] = {
	//		glm::vec4(1,  -5,  1, 4),
	//	};
	//	return level;
	//}

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