/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle 
 *
 * Adaptado por Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 05/03/2022
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

// Protótipo da função de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

//Constantes
const float Pi = 3.14159;
int moveID = 0;
int colorID = -1;
float r, g, b = 0.0;
bool Pressed = false, moving = false;
//GLfloat matriz[10][10][10];

// Protótipos das funções
int setupGeometry();
int createCircle(float radius, int nPoints);
void setupMatrix();
void drawRotating(Shader s);
void randomColor(GLint cl);

// Dimensões da janela (pode ser alterado em tempo de execução)
const GLuint WIDTH = 800, HEIGHT = 800;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	//Muita atenção aqui: alguns ambientes não aceitam essas configurações
	//Você deve adaptar para a versão do OpenGL suportada por sua placa
	//Sugestão: comente essas linhas de código para desobrir a versão e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

	srand(GetTickCount64());//semente random numbers

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Triangulo! - Anakin", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da função de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height); //viewport original
	
	// Compilando e buildando o programa de shader
	Shader shader("../shaders/ortho.vs", "../shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();
	

	// Enviando a cor desejada (vec4) para o fragment shader
	// Utilizamos a variáveis do tipo uniform em GLSL para armazenar esse tipo de info
	// que não está nos buffers
	GLint colorLoc = glGetUniformLocation(shader.ID, "inputColor");
	assert(colorLoc > -1);
	
	glUseProgram(shader.ID);

	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::ortho(-6.0, 6.0, -6.0, 6.0, -1.0, 1.0);

	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	//glm::mat4 model = glm::mat4(1); //matriz id necessaria
	//TRANSFORMAÇÕES - ORDEM: TRANSLATE, ROTATE, SCALE(LAST ONE)
	//model = glm::translate(model, glm::vec3(1.5, 0.0, 0.0));
	//model = glm::rotate(model, glm::radians(90.f), glm::vec3(0, 0, 1));
	//model = glm::scale(model, glm::vec3(0.2, 0.2, 1.0));
	//END
	//GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	//glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	float offset = 4.5;
	float posx = -offset, posy = 1.0+offset; //posição inicial //pergunta pq o ponto inicial da tela é 1 a menos q o valor da tela?

	//Limpa o buffer de cor
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f); //cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

	glLineWidth(5);
	glPointSize(15);

	//glUniform4f(colorLoc, 1.0f, 1.0f, 0.0f, 1.0f);
	//glBindVertexArray(VAO);

	//for (int y = 0; y < 1; y++)
	//{
	//	for (int x = 0; x < 10; x++)
	//	{
	//		/*if(x == 1)
	//		randomColor(colorLoc);
	//		else*/
	//		if (x < 9)
	//		{
	//			colorID = x+1;
	//		}
	//		else
	//		{
	//			colorID = 0;
	//		}


	//		switch (colorID)
	//		{
	//		case 1:
	//			r = 1.0;
	//			g = b = 0.0;
	//			break;
	//		case 2:
	//			r = 1.0;
	//			g = 0.65;
	//			b = 0.0;
	//			break;
	//		case 3:
	//			r = g = 1.0;
	//			b = 0.0;
	//			break;
	//		case 4:
	//			r = b = 0.0;
	//			g = 1.0;
	//			break;
	//		case 5:
	//			b = g = 1.0;
	//			r = 0.0;
	//			break;
	//		case 6:
	//			r = g = 0.0;
	//			b = 1.0;
	//			break;
	//		case 7:
	//			r = 0.6;
	//			g = 0.2;
	//			b = 1.0;
	//			break;
	//		case 8:
	//			r = b = 1.0;
	//			g = 0.0;
	//			break;
	//		case 9:
	//			r = g = b = 0.0;
	//			break;
	//		case 0:
	//			r = g = b = 1.0;
	//			break;
	//		}
	//		glUniform4f(colorLoc,r, g, b, 1.0f);
	//		
	//		glm::mat4 model = glm::mat4(1);
	//		model = glm::translate(model, glm::vec3(posx, posy, 0.0));
	//		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	//		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
	//		

	//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//		
	//		posx += 1.0;//contagem da linha
	//	}
	//	posy += -1.0;//contagem da coluna
	//	posx = -offset; //reset da linha
	//}

	GLfloat matriz[10][10];
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{			
			matriz[x][y] = -1;
		}
	}

	//posy = posx = 0.0;
	int Sx = 0, Sy = 0;
	
	glfwSwapBuffers(window);
	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		posx = -offset, posy = -1 + offset;

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT);

		glLineWidth(5);
		glPointSize(15);

		glBindVertexArray(VAO);

		switch (moveID)
		{
		case 0:
			moving = false;
			break;
		case 1:
			if (!moving)
			{
				Sy += -1;
				moving = true;
			}
			break;
		case 2:
			if (!moving)
			{
				Sy += 1;
				moving = true;
			}
			break;
		case 3:
			if (!moving)
			{
				Sx += -1;
				moving = true;
			}
			break;
		case 4:
			if (!moving)
			{
				Sx += 1;
				moving = true;
			}
			break;
		}

		for (int y = 0; y < 10; y++)
		{
			for (int x = 0; x < 10; x++)
			{
				glm::mat4 model = glm::mat4(1);
				model = glm::translate(model, glm::vec3(posx, posy, 0.0));
				GLint modelLoc = glGetUniformLocation(shader.ID, "model");
				glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

				if (Sx == x && Sy == y)
				{
					//quadrado de seleção
					glUniform4f(colorLoc, 0.0, 0.0, 0.0, 1.0f);
					glDrawArrays(GL_LINE_LOOP, 2, 4);
					if (Pressed)
					{
						Pressed = false;
						matriz[x][y] = colorID;
					}
				}
				int cor = matriz[x][y];

				switch (cor)
				{
				case 0:
					r = g = b = 1.0;
					break;
				case 1:
					r = 1.0;
					g = b = 0.0;
					break;
				case 2:
					r = 1.0;
					g = 0.65;
					b = 0.0;
					break;
				case 3:
					r = g = 1.0;
					b = 0.0;
					break;
				case 4:
					r = b = 0.0;
					g = 1.0;
					break;
				case 5:
					b = g = 1.0;
					r = 0.0;
					break;
				case 6:
					r = g = 0.0;
					b = 1.0;
					break;
				case 7:
					r = 0.6;
					g = 0.2;
					b = 1.0;
					break;
				case 8:
					r = b = 1.0;
					g = 0.0;
					break;
				case 9:
					r = g = b = 0.0;
					break;
				}

				if (matriz[x][y] != -1)
				{
					glUniform4f(colorLoc, r, g, b, 1.0f);
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}
								

				posx += 1.0;//contagem da linha
			}
			posy += -1.0;//contagem da coluna
			posx = -offset; //reset da linha
		}

		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers
	glDeleteVertexArrays(1, &VAO);
	// Finaliza a execução da GLFW, limpando os recursos alocados por ela
	glfwTerminate();
	return 0;
}

// Função de callback de teclado - só pode ter uma instância (deve ser estática se
// estiver dentro de uma classe) - É chamada sempre que uma tecla for pressionada
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
		moveID = 2; //TRÁS
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

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		colorID = 1; //VERMELHO
		Pressed = true;
	}
	else if(key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		colorID = 2; //LARANJA
		Pressed = true;
	}
	else if(key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		colorID = 3; //AMARELO
		Pressed = true;
	}
	else if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		colorID = 4; //VERDE
		Pressed = true;
	}
	else if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		colorID = 5; //AZUL
		Pressed = true;
	}
	else if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		colorID = 6; //AZUL ESCURO
		Pressed = true;
	}
	else if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		colorID = 7; //VIOLETA (ROXO)
		Pressed = true;
	}
	else if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		colorID = 8; //ROSA
		Pressed = true;
	}
	else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
	{
		colorID = 9; //PRETO
		Pressed = true;
	}
	else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
	{
		colorID = 0; //BRANCO
		Pressed = true;
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		colorID = -1; //APAGAR
		Pressed = true;
	}
}

// Esta função está bastante harcoded - objetivo é criar os buffers que armazenam a 
// geometria de um triângulo
// Apenas atributo coordenada nos vértices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A função retorna o identificador do VAO
int setupGeometry()
{
	GLfloat vertices[] = {
		-0.5, -0.5, 0.0,
		-0.5,  0.5, 0.0,
		 0.5, -0.5, 0.0,
		 0.5,  0.5, 0.0,
		-0.5,  0.5, 0.0,
		-0.5, -0.5, 0.0,
	};

	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
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

	//Gerando as posições dos vértices do circulo em si
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

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);
	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, totalSize * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);
	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);
	//Para cada atributo do vertice, criamos um "AttribPointer" (ponteiro para o atributo), indicando: 
	// Localização no shader * (a localização dos atributos devem ser correspondentes no layout especificado no vertex shader)
	// Numero de valores que o atributo tem (por ex, 3 coordenadas xyz) 
	// Tipo do dado
	// Se está normalizado (entre zero e um)
	// Tamanho em bytes 
	// Deslocamento a partir do byte zero 
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}

void setupMatrix()
{
	GLfloat matriz[10][10][10];
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			for (int z = 0; z < 10; z++)
			{
				matriz[x][y][z] = -1;
			}
		}
	}
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