#ifndef LEVEL_H
#define LEVEL_H

#include <vector>

class Level
{
public:
	int coutner = 0;

	vector <glm::vec4> level1;
	Level(string wtf) {
		for (int i = 0; i < 1000; i++)
		{
			level1.push_back(glm::vec4(rand() % 5 - 2, -1 * i * 2 - 25, 1, 4));
		}
		//level1.push_back(glm::vec4(-2, -6, 1, 4));
		//level1.push_back(glm::vec4(-1, -9, 1, 4));
		//level1.push_back(glm::vec4(0, -10, 1, 4));
		//level1.push_back(glm::vec4(1, -11, 1, 4));
		//level1.push_back(glm::vec4(2, -12, 1, 4));
		//level1.push_back(glm::vec4(-2, -13, 1, 4));
		//level1.push_back(glm::vec4(-1, -15, 1, 4));
		//level1.push_back(glm::vec4(0, -17, 1, 4));
		//level1.push_back(glm::vec4(1, -18, 1, 4));
		//level1.push_back(glm::vec4(2, -19, 1, 4));
	};

	bool collision(Camera camera) {
		glm::vec4 obstacle = level1.at(coutner);

		if (camera.Position.z < obstacle.y && coutner < level1.size() - 1)
			coutner++;

		if (camera.line - 3 == obstacle.x){
			if (camera.Position.z - 1 > (obstacle.y - 0.5f) && camera.Position.z - 1 < (obstacle.y + 0.5f)) {
				return true;
			}
		}
		return false;
	}

	bool win() {
		if (coutner >= level1.size() - 1)
			return true;
		return false;
	}

private:

};
#endif