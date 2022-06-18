#pragma once
#include "Object.h"
class Sprite :
    public Object
{
public:
    Sprite();
    void inicializar(GLuint text, glm::vec3 scale, int nAnims, int nFrames);
    void setAnim(int anim);
    void setFrame(int frame);

    void draw();
protected:

	int iAnim, iFrame, nAnims, nFrames;
	glm::vec2 offset;
};

