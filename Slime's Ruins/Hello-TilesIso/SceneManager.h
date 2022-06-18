#pragma once

//Shader
#include "Shader.h"

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Leitor
#include "stb_image.h"

//Classes
#include "TileIso.h"
#include "Object.h"
#include "Sprite.h"
#include "Timer.h"

//bibliotecas adicionais
#include <vector>
#include <fstream>
#include <ctime>

#define LINS 10
#define COLS 10

enum direcoes{PARADO, NORTE, SUL, LESTE, OESTE};
enum state{RESET, PLAYING};

class SceneManager
{
public:
	SceneManager();
	~SceneManager();
	void initialize(GLuint width, GLuint height);
	void initializeGraphics();

	void addShader(string vFilename, string fFilename);
	int loadTexture(string path);

	//GLFW callbacks
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void resize(GLFWwindow* window, int width, int height);

	void update();
	void render();
	void run();
	void finish();

	// scene
	void setupScene();
	void setupCamera2D();
	int setupTexture(string filename, int& largura, int& altura, int& nroCanais); //apenas mostra como criar uma textura

	void loadingLevel(int playerlvl);
	void loadLevel(string file);
private:
	
	//GFLW window
	GLFWwindow *window;

	//Array de programas de shader
	vector <Shader*> shaders;

	//scene attributes
	GLuint VAO;

	//Transformations - Model Matrix
	glm::mat4 model;

	//2D Camera - Projection matrix
	glm::mat4 projection;

	//Conjunto de tiles
	vector <TileIso> tileset;

	int map[LINS][COLS];
	int mapwalk[LINS][COLS];

	//Player
	Sprite player;
	int startx, starty;//POSIÇÃO INICIAL DA FASE ATUAL
	int poslinha; //POS Y DO PERSONAGEM NO MAPA
	int poscoluna; //POS X DO PERSONAGEM NO MAPA
	int Plvl;

	//Objetos
	Sprite potion, boxdoor, trapdoor;

	//Timer
	Timer timer;

	//files
	fstream txtFile;
};

