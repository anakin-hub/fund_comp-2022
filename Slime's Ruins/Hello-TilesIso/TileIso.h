#pragma once

#include "Shader.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class TileIso
{
public:
	TileIso();
	~TileIso();
	void inicializar(float th, float tw);
	void setCor(float r, float g, float b);
	void setCor(glm::vec4 cor);
	void draw(glm::mat4 model);
	void setShader(Shader *shader);

	float getWidth() { return tw;  }
	float getHeight() { return th; }
protected:
	GLuint VAO; //ids dos buffers (na real só precisaria guardar o VAO)

	Shader *shader;
	glm::vec4 cor;
	float tw, th;
};

