#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <cmath> //biblioteca matematica do C++

int moveID = 0;
int colorID = -1;
float r, g, b = 0.0;
bool Pressed = false, moving = false;
fstream txtFile;

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Prot�tipos das fun��es
int setupCube();
void updateCameraPos(GLFWwindow* window);

//fun��es para carregar & salvar arquivo
void saveDraw();
void loadDraw();

// Dimens�es da janela (pode ser alterado em tempo de execu��o)
const GLuint WIDTH = 600, HEIGHT = 600;
int viewID = 1;

//Vari�veis de controle da c�mera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool firstMouse = true;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0; //para calcular o quanto que o mouse deslocou
float yaw = -90.0, pitch = 0.0; //rota��o em x e y

//Cria��o da matriz que controlar� a posi��o dos voxels
GLfloat matriz[10][10][10];

// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "VOXEL EDITOR 10X10X10 - ANAKIN, BRENDA & EDUARDO", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Fazendo o registro da fun��o de callback para a janela GLFW
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


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


	// Compilando e buildando o programa de shader
	Shader shader("../shaders/helloC.vs", "../shaders/helloC.fs");

	// Gerando um buffer simples, com a geometria de um tri�ngulo
	GLuint VAO = setupCube();
	GLint colorLoc = glGetUniformLocation(shader.ID, "inputColor");
	//assert(colorLoc > -1);

	glUseProgram(shader.ID);

	//Matriz de view
	glm::mat4 view = glm::mat4(1);
	view = glm::lookAt(cameraPos, glm::vec3(0.0, 0.0, 0.0) + cameraFront, cameraUp);
	shader.setMat4("view", glm::value_ptr(view));

	//Matriz de proje��o
	glm::mat4 projection = glm::mat4(1); //matriz identidade
	projection = glm::perspective(45.0f, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	shader.setMat4("projection", glm::value_ptr(projection));

	//Habilita teste de profundidade
	glEnable(GL_DEPTH_TEST);

	//pr� cria��o do modelo do cubo
	glm::mat4 model = glm::mat4(1);
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
	
	//Configura a matriz para que nenhum voxel tenha cor, fazendo eles n�o serem desenhados
	for (int x = 0; x < 10; x++)
	{
		for (int y = 0; y < 10; y++)
		{
			for (int z = 0; z < 10; z++)
			{
				matriz[x][y][z] = -1;//matriz responsavel por indicar quais voxels tem cor e devem ser desenhados e quais n�o tem
			}
		}
	}
	//definindo variaveis para controle do selecionador de voxel
	int Sx = 0, Sy = 0, Sz = 0;
	//definindo variaveis para o controle do passo dos voxels e seus posicionamentos na area
	float offset = 4; //indica o espa�amento entre cada etapa de voxels
	float posx = 0, posy = 0, posz = 0; //indica a posi��o dos voxels na area quando combinados com o x, y e z da matriz

	// Loop da aplica��o - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Checa se houveram eventos de input (key pressed, mouse moved etc.) e chama as fun��es de callback correspondentes
		glfwPollEvents();
		//Antigo "processInput" -- a ideia � que vire m�todo da classe Camera futuramente
		updateCameraPos(window);

		// Definindo as dimens�es da viewport com as mesmas dimens�es da janela da aplica��o
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// Limpa o buffer de cor e de profundidade
		glClearColor(0.87, 0.87, 0.87, 1.0f); //set de cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//set de tamanho da linha e ponto
		glLineWidth(5);
		glPointSize(20);

		//Para acionar as m�ltiplas views
		switch (viewID)
		{
		case 1:
			cameraPos = glm::vec3(0.5, -0.5, 3.0);
			cameraFront = glm::vec3(0.0, 0.0, -1.0);
			cameraUp = glm::vec3(0.0, 1.0, 0.0);
			viewID = -1;
			break;
		case 2:
			cameraPos = glm::vec3(0.5, -0.5, -26.0);
			cameraFront = glm::vec3(0.0, 0.0, 1.0);
			cameraUp = glm::vec3(0.0, 1.0, 0.0);
			viewID = -1;
			break;
		case 3:
			cameraPos = glm::vec3(-14.0, -0.5, -11.5);
			cameraFront = glm::vec3(1.0, 0.0, 0.0);
			cameraUp = glm::vec3(0.0, 1.0, 0.0);
			viewID = -1;
			break;
		case 4:
			cameraPos = glm::vec3(15.0, -0.5, -11.5);
			cameraFront = glm::vec3(-1.0, 0.0, 0.0);
			cameraUp = glm::vec3(0.0, 1.0, 0.0);
			viewID = -1;
			break;
		case 5:
			cameraPos = glm::vec3(0.5, 14.0, -11.5);
			cameraFront = glm::vec3(0.0, -1.0, 0.0);
			cameraUp = glm::vec3(0.0, 0.0, -1.0);
			viewID = -1;
			break;
		}

		//setup da camera indicando para onde ela deve "olhar"
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", glm::value_ptr(view));		
		
		glBindVertexArray(VAO);
		
		//ajustes para o contorno do cubo da area permitida desenhar voxels
		model = glm::translate(model, glm::vec3(-4.5, 4.5, 7.0));
		modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));
		//sele��o da cor do contorno da area
		glUniform4f(colorLoc, 0.0, 0.0, 0.0, 1.0f);
		//desenho da area permitida desenhar voxels
		glDrawArrays(GL_LINE_LOOP, 40, 4);
		glDrawArrays(GL_LINE_LOOP, 44, 4);
		glDrawArrays(GL_LINES, 48, 8);

		//configurando a posi��o inicial da matriz
		posx = -offset, posy = offset, posz = -offset*4;
		//movimenta��o do selecionador de voxels
		switch (moveID)
		{
		case 0:
			moving = false;
			break;
		case 1:
			if (!moving)
			{
				if (Sz - 1 > -1)
					Sz += -1;
				moving = true;
			}
			break;
		case 2:
			if (!moving)
			{
				if(Sz+1 < 10)
					Sz += 1;
				moving = true;
			}
			break;
		case 3:
			if (!moving)
			{
				if (Sx - 1 > -1)
					Sx += -1;
				moving = true;
			}
			break;
		case 4:
			if (!moving)
			{
				if (Sx + 1 < 10)
					Sx += 1;
				moving = true;
			}
			break;
		case 5:
			if (!moving)
			{
				if (Sy - 1 > -1)
					Sy += -1;
				moving = true;
			}
			break;
		case 6:
			if (!moving)
			{
				if (Sy + 1 < 10)
					Sy += 1;
				moving = true;
			}
			break;
		}

		//defini��o dos for para "andar" dentro da matriz
		for (int y = 0; y < 10; y++)
		{
			for (int z = 0; z < 10; z++)
			{
				for (int x = 0; x < 10; x++)
				{
					//controle da posi��o dos voxels
					model = glm::mat4(1);
					model = glm::translate(model, glm::vec3(posx, posy, posz));
					modelLoc = glGetUniformLocation(shader.ID, "model");
					glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

					//controle da posi��o do selecionador e desenho do mesmo
					if (Sx == x && Sy == y && Sz == z)
					{
						//desenho do cubo de sele��o
						glUniform4f(colorLoc, 0.0, 0.0, 0.0, 1.0f);
						glDrawArrays(GL_LINE_LOOP, 24, 4);
						glDrawArrays(GL_LINE_LOOP, 28, 4);
						glDrawArrays(GL_LINES, 32, 8);
						//controle de pressionamento de tecla para pintar ou apagar um voxel
						if (Pressed)
						{
							Pressed = false;//controle do pressionar para n�o causar over click e assim pintar mais de 1x o mesmo voxel
							matriz[x][y][z] = colorID;//envia a cor definida para a posi��o desejada na matriz de voxels
						}
					}
					//cria��o da variavel que controla a cor dos voxels e envio da cor da posi��o atual do voxel
					int cor = matriz[x][y][z];
					//controle da cor que ser� pintada no voxel
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

					//verifica se o voxel esta com alguma cor setada e se estiver, pinta ele, caso n�o esteja, ser� ignorado
					if (matriz[x][y][z] != -1)
					{
						glUniform4f(colorLoc, r, g, b, 1.0f);
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);
					}			

					posx += 1.0;//contagem do x
				}
				posx = -offset; //reset do x
				posz += 1.0;//contagem do z
			}
			posz = -offset*4;//reset do z
			posy += -1.0;//contagem da y
		}

		glBindVertexArray(0);
		// Troca os buffers da tela
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

	if (key == GLFW_KEY_H && action == GLFW_PRESS)
	{
		viewID = 1; //FRENTE
	}

	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		viewID = 2; //TR�S
	}

	if (key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		viewID = 3; //ESQUERDA
	}

	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		viewID = 4; //DIREITA
	}

	if (key == GLFW_KEY_U && action == GLFW_PRESS)
	{
		viewID = 5; //TOPO
	}

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
	else if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		moveID = 5; //CIMA
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		moveID = 6; //BAIXO
	}
	else
	{
		moveID = 0; //Mant�m a mesma posi��o
	}

	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		colorID = 1; //VERMELHO
		Pressed = true;
	}
	else if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		colorID = 2; //LARANJA
		Pressed = true;
	}
	else if (key == GLFW_KEY_3 && action == GLFW_PRESS)
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
	
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		//SALVAR ARQUIVO
		saveDraw();
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		//CARREGAR ARQUIVO
		loadDraw();
	}
}

// Esta fun��o est� bastante harcoded - objetivo � criar os buffers que armazenam a 
// geometria de um tri�ngulo
// Apenas atributo coordenada nos v�rtices
// 1 VBO com as coordenadas, VAO com apenas 1 ponteiro para atributo
// A fun��o retorna o identificador do VAO

int setupCube()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//VOXEL
		//x    y    z 
		-0.5,-0.5, 0.5,
		-0.5, 0.5, 0.5,
		 0.5,-0.5, 0.5,
		 0.5, 0.5, 0.5,
		//parte frontal
		-0.5,-0.5,-0.5,
		-0.5, 0.5,-0.5,
		-0.5,-0.5, 0.5,
		-0.5, 0.5, 0.5,
		//parte esquerda
		-0.5,-0.5,-0.5,
		-0.5, 0.5,-0.5,
		 0.5,-0.5,-0.5,
		 0.5, 0.5,-0.5,
		//parte traseira
		 0.5,-0.5,-0.5,
		 0.5, 0.5,-0.5,
		 0.5,-0.5, 0.5,
		 0.5, 0.5, 0.5,
		//parte esquerda
		-0.5, 0.5, 0.5,
		-0.5, 0.5,-0.5,
		 0.5, 0.5, 0.5,
		 0.5, 0.5,-0.5,
		//parte superior
		-0.5,-0.5, 0.5,
		-0.5,-0.5,-0.5,
		 0.5,-0.5, 0.5,
		 0.5,-0.5,-0.5,
		//parte inferior
		
		//CUBO DE SELE��O
		-0.5,-0.5, 0.5,
		 0.5,-0.5, 0.5,
		 0.5, 0.5, 0.5,
		-0.5, 0.5, 0.5,
		//parte frontal
		-0.5, 0.5,-0.5,
		 0.5, 0.5,-0.5,
		 0.5,-0.5,-0.5,
		-0.5,-0.5,-0.5,		 
		//parte traseira
		-0.5,-0.5,-0.5,
		-0.5,-0.5, 0.5,
		//linha esq inf
		-0.5, 0.5,-0.5,
		-0.5, 0.5, 0.5,
		//linha esq sup
		 0.5, 0.5,-0.5,
		 0.5, 0.5, 0.5,
		//linha dir sup
		 0.5,-0.5,-0.5,
		 0.5,-0.5, 0.5,
		//linha dir inf

		//CONTORNO DA AREA (CUBO)
		-5.0, 5.0,-16.5,
		 5.0, 5.0,-16.5,
		 5.0,-5.0,-16.5,
		-5.0,-5.0,-16.5,
		//parte traseira
		-5.0, 5.0,-6.5,
		 5.0, 5.0,-6.5,
		 5.0,-5.0,-6.5,
		-5.0,-5.0,-6.5,
		//parte frontal
		-5.0, 5.0,-16.5,
		-5.0, 5.0,-6.5,
		//linha esq sup
		 5.0, 5.0,-16.5,
		 5.0, 5.0,-6.5,
		//linha dir sup
		 5.0,-5.0,-16.5,
		 5.0,-5.0,-6.5,
		//linha dir inf
		-5.0,-5.0,-16.5,
		-5.0,-5.0,-6.5,
		//linha esq inf
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

	//Atributo posi��o (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	return VAO;
}
//controle da c�mera pelas setas do teclado
void updateCameraPos(GLFWwindow* window)
{
	float cameraSpeed = 0.05f; // adjust accordingly

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void saveDraw()
{
	txtFile.open("matriz.txt");
	if (txtFile.is_open())
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				for (int k = 0; k < 10; k++)
				{
					txtFile << matriz[i][j][k] << " ";
				}
				txtFile << endl;
			}
		}
		txtFile.close();
	}
}

void loadDraw()
{
	txtFile.open("matriz.txt");
	if (txtFile.is_open())
	{
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 10; j++)
			{
				for (int k = 0; k < 10; k++)
				{
					txtFile >> matriz[i][j][k];
				}
			}
		}
		txtFile.close();
	}
}

//Fun��o para controle da posi��o do mouse e contrle da c�mera pelo mesmo
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);

	//Precisamos tamb�m atualizar o cameraUp!! Pra isso, usamos o Up do mundo (y),
	//Recalculamos Right e depois o Up
	glm::vec3 right = glm::normalize(glm::cross(cameraFront, glm::vec3(0.0, 1.0, 0.0)));
	cameraUp = glm::normalize(glm::cross(right, cameraFront));

}