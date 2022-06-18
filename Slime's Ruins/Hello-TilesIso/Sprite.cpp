#include "Sprite.h"

Sprite::Sprite()
{
	iAnim = 0;
	iFrame = 0;
	scale = glm::vec3(1.0, 1.0, 1.0);
	position = glm::vec3(0.0, 0.0, 0.0);
	nAnims = 0;
	nFrames = 0;
}

void Sprite::inicializar(GLuint text, glm::vec3 scale, int nAnims, int nFrames)
{
	textID = text;
	
	this->nAnims = nAnims;
	this->nFrames = nFrames;

	offset.x = 1.0 / nFrames;
	offset.y = 1.0 / nAnims;

	float vertices[] = {
		// posicoes          // cores           // coordenadas de textura
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   offset.x, offset.y, // topo da direita
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   offset.x, 0.0, // base da direita
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0, // topo da esquerda
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, offset.y  // base da esquerda 
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

	this->scale.x = scale.x / nFrames;
	this->scale.y = scale.y / nAnims;
}

void Sprite::setAnim(int anim)
{
	iAnim = anim;
}

void Sprite::setFrame(int frame)
{
	iFrame = frame;
}

void Sprite::draw()
{
	//shader->Use();
	GLint offsetLoc = glGetUniformLocation(shader->ID, "offset");

	iFrame = (iFrame + 1) % nFrames;

	glm::vec2 _offset = glm::vec2(offset.x * iFrame, offset.y * iAnim);
	glUniform2f(offsetLoc, _offset.x, _offset.y);

	//seta a textura
	glBindTexture(GL_TEXTURE_2D, textID);
	glUniform1i(glGetUniformLocation(shader->ID, "ourTexture1"), 0);

	//considerando que possui EBO
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
