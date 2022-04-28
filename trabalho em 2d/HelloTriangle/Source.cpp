#include <iostream>
#include <string>
#include <assert.h>

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

const float Pi = 3.14159;
int moveID = 0;
int colorID = -1;
float r, g, b = 0.0;
bool Pressed = false, moving = false;

// Prot�tipo da fun��o de callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);

// Prot�tipos das fun��es
int setupGeometry();
int createCircle(float radius, int nPoints);
int setupGeometry3D();
int setupCube();
int setupCubeColor();
void updateCameraPos(GLFWwindow* window);

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



// Fun��o MAIN
int main()
{
	// Inicializa��o da GLFW
	glfwInit();

	//Muita aten��o aqui: alguns ambientes n�o aceitam essas configura��es
	//Voc� deve adaptar para a vers�o do OpenGL suportada por sua placa
	//Sugest�o: comente essas linhas de c�digo para desobrir a vers�o e
	//depois atualize (por exemplo: 4.5 com 4 e 5)
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	//Essencial para computadores da Apple
//#ifdef __APPLE__
//	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif


	// Cria��o da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Piramide!", nullptr, nullptr);
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
	GLuint VAO = setupCube(); // createCircle(0.5, 100);//
	GLint colorLoc = glGetUniformLocation(shader.ID, "inputColor");
	assert(colorLoc > -1);

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
	int Sx = 0, Sy = 0, Sz = 0;
	float offset = 4;
	float posx = 0, posy = 0, posz = 0;

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
		glClearColor(0.87, 0.87, 0.87, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
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

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", glm::value_ptr(view));


		glm::mat4 model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(0.1, 0.1, 0.1));
		GLint modelLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);
		
		posx = -offset, posy = offset, posz = -offset*4;
		switch (moveID)
		{
		case 0:
			moving = false;
			break;
		case 1:
			if (!moving)
			{
				if (Sz - 1 > 0)
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
				if (Sx - 1 > 0)
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
				if (Sy - 1 > 0)
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

		for (int y = 0; y < 10; y++)
		{
			for (int z = 0; z < 10; z++)
			{
				for (int x = 0; x < 10; x++)
				{
					glm::mat4 model = glm::mat4(1);
					model = glm::translate(model, glm::vec3(posx, posy, posz));
					GLint modelLoc = glGetUniformLocation(shader.ID, "model");
					glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

					if (Sx == x && Sy == y && Sz == z)
					{
						//quadrado de sele��o
						glUniform4f(colorLoc, 0.0, 0.0, 0.0, 1.0f);
						glDrawArrays(GL_LINE_LOOP, 2, 4);
						if (Pressed)
						{
							Pressed = false;
							matriz[x][y][z] = colorID;
						}
					}
					int cor = matriz[x][y][z];

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

					if (matriz[x][y][z] != -1)
					{
						glUniform4f(colorLoc, r, g, b, 1.0f);
						glDrawArrays(GL_TRIANGLE_STRIP, 0, 24);
					}


					posx += 1.0;//contagem da linha
				}
				posx = -offset; //reset da linha
				posz += 1.0;
			}
			posz = -offset*4;
			posy += -1.0;//contagem da coluna
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
		moveID = 0;
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
		//x   y     z
		-0.5, -0.5, 0.0, //vertice 1
		 0.5, -0.5, 0.0, //vertice 2
		 0.0,  0.5, 0.0, //vertice 3
		////x   y     z
		//-0.5, 0.0, 0.0,
		// 0.0, 0.0, 0.0,
		// -0.25, 1.0, 0.0,

		// 0.0, 0.0, 0.0,
		// 0.5, 0.0, 0.0,
		// 0.25, 1.0,0.0,

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

	//totalSize � a quantidade de floats que usaremos para alocar nPoints + 1 v�rtices
	int totalSize = (nPoints + 2) * 3;
	vertices = new float[totalSize];

	//primeiro vertice - centro do circulo - origem
	vertices[0] = 0.0 + 400; //x
	vertices[1] = 0.0 + 300; //y
	vertices[2] = 0.0; //z

	float angle = 0.0;
	float slice = 2 * Pi / (float)nPoints;

	int i = 3;
	//Gerando as posi��es dos v�rtices do c�rculo em si
	while (i < totalSize)
	{
		float x = radius * cos(angle);
		float y = radius * sin(angle);
		float z = 0.0;

		vertices[i] = x * 100 + 400;
		vertices[i + 1] = y * 100 + 300;
		vertices[i + 2] = z;

		i += 3; // i = i+3
		angle += slice; //angle = angle + slice
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

int setupGeometry3D()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//Base da pir�mide: 2 tri�ngulos
		//x    y    z    r    g    b
		-0.5, -0.5, -0.5, 1.0, 1.0, 0.0,
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		 -0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		  0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		  0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		  //
		  -0.5, -0.5, -0.5, 1.0, 0.5, 0.5,
		   0.0,  0.5,  0.0, 1.0, 0.5, 0.5,
		   0.5, -0.5, -0.5, 1.0, 0.5, 0.5,
		  -0.5, -0.5, -0.5, 1.0, 0.0, 1.0,
		   0.0,  0.5,  0.0, 1.0, 0.0, 1.0,
		  -0.5, -0.5,  0.5, 1.0, 0.0, 1.0,
		  -0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		   0.0,  0.5, 0.0, 1.0, 1.0, 0.0,
		   0.5, -0.5, 0.5, 1.0, 1.0, 0.0,
		   0.5, -0.5, 0.5, 0.0, 1.0, 1.0,
		   0.0,  0.5,  0.0, 0.0, 1.0, 1.0,
		   0.5, -0.5, -0.5, 0.0, 1.0, 1.0,
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	return VAO;
}

int setupCubeColor()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//Base do cubo: 2 tri�ngulos
		//x    y    z    r    g    b
		-0.5,-0.5, 0.5, 0.62, 0.0, 0.77,
		-0.5, 0.5, 0.5, 0.62, 0.0, 0.77,
		 0.5,-0.5, 0.5, 0.62, 0.0, 0.77,
		 0.5, 0.5, 0.5, 0.62, 0.0, 0.77,
		 //parte frontal
		 -0.5,-0.5,-0.5, 0.62, 0.0, 0.77,
		 -0.5, 0.5,-0.5, 0.62, 0.0, 0.77,
		 -0.5,-0.5, 0.5, 0.62, 0.0, 0.77,
		 -0.5, 0.5, 0.5, 0.62, 0.0, 0.77,
		 //parte esquerda
		 -0.5,-0.5,-0.5, 0.62, 0.0, 0.77,
		 -0.5, 0.5,-0.5, 0.62, 0.0, 0.77,
		  0.5,-0.5,-0.5, 0.62, 0.0, 0.77,
		  0.5, 0.5,-0.5, 0.62, 0.0, 0.77,
		  //parte traseira
		  0.5,-0.5,-0.5, 0.62, 0.0, 0.77,
		  0.5, 0.5,-0.5, 0.62, 0.0, 0.77,
		  0.5,-0.5, 0.5, 0.62, 0.0, 0.77,
		  0.5, 0.5, 0.5, 0.62, 0.0, 0.77,
		  //parte esquerda
		  -0.5, 0.5, 0.5, 0.62, 0.0, 0.77,
		  -0.5, 0.5,-0.5, 0.62, 0.0, 0.77,
		   0.5, 0.5, 0.5, 0.62, 0.0, 0.77,
		   0.5, 0.5,-0.5, 0.62, 0.0, 0.77,
		   //parte superior
		   -0.5,-0.5, 0.5, 0.62, 0.0, 0.77,
		   -0.5,-0.5,-0.5, 0.62, 0.0, 0.77,
			0.5,-0.5, 0.5, 0.62, 0.0, 0.77,
			0.5,-0.5,-0.5, 0.62, 0.0, 0.77,
			//parte inferior		
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	//Atributo cor (r, g, b)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	// Observe que isso � permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de v�rtice 
	// atualmente vinculado - para que depois possamos desvincular com seguran�a
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// Desvincula o VAO (� uma boa pr�tica desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);
	return VAO;
}

int setupCube()
{
	// Aqui setamos as coordenadas x, y e z do tri�ngulo e as armazenamos de forma
	// sequencial, j� visando mandar para o VBO (Vertex Buffer Objects)
	// Cada atributo do v�rtice (coordenada, cores, coordenadas de textura, normal, etc)
	// Pode ser arazenado em um VBO �nico ou em VBOs separados
	GLfloat vertices[] = {
		//Base do cubo: 2 tri�ngulos
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