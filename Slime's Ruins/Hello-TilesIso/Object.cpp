#include "Object.h"

Object::Object()
{
	scale = glm::vec3(1.0, 1.0, 1.0);
	position = glm::vec3(0.0, 0.0, 0.0);
}

Object::~Object()
{
}

void Object::initialize(GLuint text, glm::vec3 scale)
{
	textID = text;

	float vertices[] = {
		// posicoes          // cores           // coordenadas de textura
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,		1.0f, 1.0f, // topo da direita
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,		1.0f, 0.0f, // base da direita
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,		0.0f, 0.0f, // topo da esquerda
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,		0.0f, 1.0f  // base da esquerda 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void Object::update()
{
	shader->Use();
	setTranslation(position, true);
	setScale(scale, false);
	// Get their uniform location
	GLint modelLoc = glGetUniformLocation(shader->ID, "model");
	// Pass them to the shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
}

void Object::draw()
{
	//seta a textura
	glBindTexture(GL_TEXTURE_2D, textID);
	glUniform1i(glGetUniformLocation(shader->ID, "ourTexture1"), 0);

	//considerando que possui EBO
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Object::setTexture(GLuint textID)
{
	this->textID = textID;
}

void Object::setShader(Shader* shader)
{
	this->shader = shader;
	shader->Use();
}

void Object::setPosition(glm::vec3 position)
{
	this->position = position;
}

void Object::setScale(glm::vec3 scale)
{
	this->scale = scale;
}

void Object::setAngle(float angle)
{
	this->angle = angle;
}

void Object::setRotation(float angle, glm::vec3 pos, bool reset)
{
	if (reset) 
		model = glm::mat4(1);
	model = glm::rotate(model, angle, pos);
}

void Object::setTranslation(glm::vec3 pos, bool reset)
{
	if (reset) 
		model = glm::mat4(1);
	model = glm::translate(model, pos);
	position = pos;
}

void Object::setScale(glm::vec3 scalepos, bool reset)
{
	if (reset) 
		model = glm::mat4(1);
	model = glm::scale(model, scalepos);
	scale = scalepos;
}
