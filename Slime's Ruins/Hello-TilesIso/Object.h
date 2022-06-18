#pragma once
#include <iostream>
#include <string>
#include <assert.h>
#include <cmath>

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//stb_image
#include "stb_image.h"

//Shaders
#include "Shader.h"

using namespace std;

class Object
{
public:
	Object();
	~Object();

	void initialize(GLuint text, glm::vec3 scale);
	//void initialize(string filename);
	void update();
	void draw();

	void setTexture(GLuint textID);
	void setShader(Shader* shader);
	void setPosition(glm::vec3 position);
	void setScale(glm::vec3 scale);
	void setAngle(float angle);
protected:
	void setRotation(float angle, glm::vec3 pos, bool reset);
	void setTranslation(glm::vec3 pos, bool reset);
	void setScale(glm::vec3 scalepos, bool reset);

	GLuint VAO, textID;
	glm::mat4 model; //matriz transform
	Shader* shader;
	glm::vec3 position, scale;
	float angle;
};

