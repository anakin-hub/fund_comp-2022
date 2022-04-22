/* Hello Triangle - c�digo adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gr�fico - Jogos Digitais - Unisinos
 * Vers�o inicial: 7/4/2017
 * �ltima atualiza��o em 05/03/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>
#include <Windows.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader Class
#include "Shader.h"

// Biblioteca matematica C#
#include <cmath>

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//Constantes
const float Pi = 3.14159;
int moveID = 0;

// Prot�tipos das fun��es
int setupGeometry();
int createCircle(float radius, int nPoints);
int setupMatrix();
void drawRotating(Shader s);
void randomColor(GLint cl);

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 800, HEIGHT = 800;

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	//srand(GetTickCount64());//semente random numbers

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! - Anakin", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d fun��es da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informa��es de vers�o
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height); //viewport original
	
	// Compilando e buildando o programa de shader
	Shader shader("../shaders/ortho.vs", "../shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupGeometry();
	

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a vari�veis do tipo uniform em GLSL para armazenar esse tipo de info
	// que n�o est� nos buffers
	GLint colorLoc = glGetUniformLocation(shader.ID, "inputColor");
	assert(colorLoc > -1);
	
	glUseProgram(shader.ID);

	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::ortho(-6.0, 6.0, -6.0, 6.0, -1.0, 1.0);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	//glm::mat4 model = glm::mat4(1); //matriz id necessaria
	//TRANSFORMA��ES - ORDEM: TRANSLATE, ROTATE, SCALE(LAST ONE)
	//model = glm::translate(model, glm::vec3(1.5, 0.0, 0.0));
	//model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 0, 1));
	//model = glm::scale(model, glm::vec3(0.5, 0.5, 1.0));
	//END
	//GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	//glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
	float posx = 0.0, posy = 0.0;

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		 //posi��o inicial //pergunta pq o ponto inicial da tela � 1 a menos q o valor da tela?

		switch (moveID)
		{
		case 0:
			
			break;
		case 1:
			posy += 0.2;
			break;
		case 2:
			posy += -0.2;
			break;
		case 3:
			posx += -0.2;
			break;
		case 4:
			posx += 0.2;
			break;
		}

		// Limpa o buffer de cor
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(5);
		glPointSize(15);

		glUniform4f(colorLoc, 1.0f, 0.0f, 0.5f, 1.0f);
		glBindVertexArray(VAO);

		glm::mat4 model = glm::mat4(1);
		model = glm::translate(model, glm::vec3(posx, posy, 0.0));
		model = glm::scale(model, glm::vec3(0.5, 0.5, 1.0));
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		glDrawArrays(GL_LINE_LOOP, 0, 4);		

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execu��o da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Fun��o de callback de teclado - s� pode ter uma inst�ncia (deve ser est�tica se
// estiver dentro de uma classe) - � chamada sempre que uma tecla for pressionada
// ou solta via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_W && action == GLFW_PRESS)
	{
		moveID = 1; //FRENTE
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		moveID = 2; //TR�S
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		moveID = 3; //ESQUERDA
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS)
	{
		moveID = 4; //DIREITA
	}
	else
	{
		moveID = 0;
	}
}

// Esta fun��o est� bastante harcoded - objetivo � criar os buffers que armazenam a 
// geometria de um tri�ngulo
// Apenas atributo coordenada nos v�rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun��o retorna o identificador do VAO
int setupGeometry()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		 1.0, -1.0, 0.0,
		 1.0,  1.0, 0.0,
		-1.0,  1.0, 0.0,
		-1.0, -1.0, 0.0,
	};

	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0); 

	return VAO;
}

int createCircle(float radius, int nPoints)
{
	float* vertices;
	int totalSize = (nPoints +2) * 3;//quantidade de floats usadas para alocar nPoints+1 vertices
	vertices = new float[totalSize];

	//primeiro vertice - centro do circulo - Origem
	vertices[0] = 0.0;//x
	vertices[1] = 0.0;//y
	vertices[2] = 0.0;//z

	float angle = Pi/6;
	float slice = 2 * Pi / (float)nPoints;//para otimizar a fatia da pizza alterar o npoints, mudo para 100

	int i = 3;

	//Gerando as posi��es dos v�rtices do circulo em si
	while (i < totalSize)
	{
		float x, y, z;

		x = radius * cos(angle);
		y = radius * sin(angle);
		z = 0.0;

		vertices[i] = x;
		vertices[i + 1] = y;
		vertices[i + 2] = z;

		i+=3; //i = i+3
		angle += slice;
	}

	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, totalSize * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

int setupMatrix()
{
	GLfloat vertices[10][10][10];
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			for (int z = 0; z < 10; z++)
			{
				vertices[x][y][z] = -1;
			}
		}
	}

	GLuint VBO, VAO;

	//Gera��o do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conex�o (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Gera��o do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de v�rtices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localiza��o no shader * (a localiza��o dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se est� normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void drawRotating(Shader s)
{
	glm::mat4 model = glm::mat4(1);
	model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0, 0, 1));
	GLint modelLoc = glGetUniformLocation(s.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
}

void randomColor(GLint cl)
{
	float r, g, b;
	r = (float)rand() / (float)RAND_MAX;
	g = (float)rand() / (float)RAND_MAX;
	b = (float)rand() / (float)RAND_MAX;

	glUniform4f(cl, r, g, b, 1.0f);
}