#include "SceneManager.h"

//static controllers for mouse and keyboard
static bool keys[1024];
static bool resized;
static GLuint width, height;
static int dir;
static int status;

SceneManager::SceneManager()
{
	srand(time(0));
}

SceneManager::~SceneManager()
{
}

void SceneManager::initialize(GLuint w, GLuint h)
{
	width = w;
	height = h;
	
	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	initializeGraphics();

}

void SceneManager::initializeGraphics()
{
	// Init GLFW
	glfwInit();

	// Create a GLFWwindow object that we can use for GLFW's functions
	window = glfwCreateWindow(width, height, "Hello Transform", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	//Setando a callback de redimensionamento da janela
	glfwSetWindowSizeCallback(window, resize);
	
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Build and compile our shader program
	addShader("../shaders/transformations.vs", "../shaders/transformations.frag"); //0
	addShader("../shaders/sprite.vs", "../shaders/sprite.frag"); //1

	//setup the scene -- LEMBRANDO QUE A DESCRIÇÃO DE UMA CENA PODE VIR DE ARQUIVO(S) DE 
	// CONFIGURAÇÃO
	setupScene();

	resized = true; //para entrar no setup da câmera na 1a vez
	
	setupCamera2D();

}

void SceneManager::addShader(string vFilename, string fFilename)
{
	Shader *shader = new Shader (vFilename.c_str(), fFilename.c_str());
	shaders.push_back(shader);
}

int SceneManager::loadTexture(string path)
{
	GLuint texID;

	// Gera o identificador da textura na memória 
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	//Ajusta os parâmetros de wrapping e filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Carregamento da imagem
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}


void SceneManager::key_callback(GLFWwindow * window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_W)
		{
			dir = NORTE;
		}
		if (key == GLFW_KEY_S)
		{
			dir = SUL;
		}
		if (key == GLFW_KEY_E)
		{
			dir = OESTE;
		}
		if (key == GLFW_KEY_D)
		{
			dir = LESTE;
		}
		if (key == GLFW_KEY_SPACE)
		{
			status = RESET;
		}
	}
	else
	{
		dir = PARADO;
	}
}

void SceneManager::resize(GLFWwindow * window, int w, int h)
{
	width = w;
	height = h;
	resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, width, height);
}


void SceneManager::update()
{
	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);

	

	int px, py;

	px = poslinha;
	py = poscoluna;

	if (status == RESET)
	{
		loadingLevel(Plvl);
		poslinha = startx;
		poscoluna = starty;
		status = PLAYING;
	}
	else
	{
		if (dir != PARADO)
		{
			if (dir == NORTE)
			{
				player.setAnim(8);
				poscoluna--;
			}

			if (dir == SUL)
			{
				player.setAnim(6);
				poslinha++;
			}

			if (dir == LESTE)
			{
				player.setAnim(7);
				poscoluna++;
			}

			if (dir == OESTE)
			{
				player.setAnim(5);
				poslinha--;
			}

			//Validações pro personagem não sair do mapa
			if (poslinha < 0)
			{
				poslinha = 0;
			}
			if (poscoluna < 0)
			{
				poscoluna = 0;
			}
			if (poslinha > 9)
			{
				poslinha = 9;
			}
			if (poscoluna > 9)
			{
				poscoluna = 9;
			}

			if (mapwalk[poslinha][poscoluna] == 0)
			{
				poslinha = px;
				poscoluna = py;
			}
			else if (mapwalk[poslinha][poscoluna] == 2)
			{
				mapwalk[poslinha][poscoluna]--;
				map[poslinha][poscoluna] = 4;
				for (int i = 0; i < LINS; i++)
				{
					for (int j = 0; j < COLS; j++)
					{
						if (map[i][j] == 1)
						{
							map[i][j]++;
							mapwalk[i][j]--;
						}
					}
				}

			}


			if (map[px][py] >= 3 && map[px][py] < 5)
			{
				map[px][py]++;
				if (map[px][py] > 4)
				{
					mapwalk[px][py]--;
				}
			}

			if (mapwalk[poslinha][poscoluna] == 3)
			{
				cout << "\nPARABENS VOCE PASSOU!\n";
				Plvl++;
				if (Plvl < 4)
				{
					startx = poslinha;
					starty = poscoluna;
					loadingLevel(Plvl);
				}
				else
				{
					cout << "\nPARABENS VOCE TERMINOU O JOGO!\n";
					glfwSetWindowShouldClose(window, GL_TRUE);
				}
			}
		}
	}
}

void SceneManager::render()
{
	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	
	if (resized) //se houve redimensionamento na janela, redefine a projection matrix
	{
		setupCamera2D();
		resized = false;
	}

	shaders[0]->Use();

	float xi = 368; //metade da largura da tela - metade da largura do tile
	float yi = 100;

	for (int i = 0; i < LINS; i++) //linhas
	{
		for (int j = 0; j < COLS; j++) //colunas
		{
			float x = xi + (j - i) * tileset[0].getWidth() / 2.0;
			float y = yi + (j + i) * tileset[0].getHeight() / 2.0;
			model = glm::mat4();
			model = glm::translate(model, glm::vec3(x, y, 0.0));
			tileset[map[i][j]].draw(model);
		}	
	}


	float x = xi + (poscoluna - poslinha) * tileset[0].getWidth() / 2.0;
	float y = yi + (poscoluna + poslinha) * tileset[0].getHeight() / 2.0;
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(x, y, 0.0));

	tileset[7].draw(model);
	
	for (int i = 0; i < LINS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			float x = xi + (j - i) * tileset[0].getWidth() / 2.0;
			float y = yi + (j + i) * tileset[0].getHeight() / 2.0;
			if (mapwalk[i][j] == 2)
			{
				potion.setPosition(glm::vec3(x + (tileset[0].getWidth() / 2.0), y + (tileset[0].getHeight() / 4.0), 1.0));
				potion.update();
				potion.draw();
			}
			/*if (mapwalk[i][j] == 3)
			{
				trapdoor.setPosition(glm::vec3(x + (tileset[0].getWidth() / 2.0), y + (tileset[0].getHeight() / 4.0), 1.0));
				trapdoor.update();
				trapdoor.draw();
			}*/
			if (map[i][j] == 1)
			{
				boxdoor.setPosition(glm::vec3(x + (tileset[0].getWidth() / 2.0), y + (tileset[0].getHeight() / 4.0), 1.0));
				boxdoor.update();
				boxdoor.draw();
			}
		}
	}

	player.setPosition(glm::vec3(x + (tileset[0].getWidth()/2.0), y + (tileset[0].getHeight()/4.0), 1.0));

	player.update();
	player.draw();

	timer.finish();
	double waitingTime = timer.calcWaitingTime(12, timer.getElapsedTimeMs());
	if (waitingTime) {
		std::this_thread::sleep_for(std::chrono::milliseconds((int)waitingTime));
	}
}

void SceneManager::run()
{
	//GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		//timer starts
		timer.start();

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();

		//Update method(s)
		update();

		//Render scene
		render();
		
		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
}

void SceneManager::finish()
{
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
}


glm::vec4 normalizaRGB(glm::vec4 byteColor)
{
	glm::vec4 normColor(byteColor.r / 255.0, byteColor.g / 255.0, byteColor.b / 255.0, byteColor.a / 255.0);
	return normColor;
}


void SceneManager::setupScene()
{
	TileIso tile;
	glm::vec4 corDoTile;

	tile.setShader(shaders[0]);

	//Tile 0
	corDoTile.r = 26; corDoTile.g = 255; corDoTile.b = 0; corDoTile.a = 255;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Tile 1
	corDoTile.r = 112; corDoTile.g = 0; corDoTile.b = 160; corDoTile.a = 255;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Tile 2
	corDoTile.r = 179; corDoTile.g = 0; corDoTile.b = 255; corDoTile.a = 255;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Tile 3
	corDoTile.r = 76; corDoTile.g = 88; corDoTile.b = 107; corDoTile.a = 255;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Tile 4
	corDoTile.r = 141; corDoTile.g = 153; corDoTile.b = 174; corDoTile.a = 255;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Tile 5
	corDoTile.r = 217; corDoTile.g = 4; corDoTile.b = 41; corDoTile.a = 255;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Tile 6
	corDoTile.r = 40; corDoTile.g = 42; corDoTile.b = 62; corDoTile.a = 255;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Tile 7
	corDoTile.r = 0; corDoTile.g = 0; corDoTile.b = 0; corDoTile.a = 155;//Aqui coloca a cor RGBA 
	tile.setCor(normalizaRGB(corDoTile));
	tile.inicializar(32, 64);
	tileset.push_back(tile);

	//Inicializa o personagem
	GLuint playerID = loadTexture("../textures/slime4.png");
	player.setShader(shaders[1]);
	glm::vec3 Pscale(160.0, 160.0, 1.0);
	player.inicializar(playerID, Pscale, 4, 6);

	//Inicializa a poção (KEY)
	GLuint potionID = loadTexture("../textures/potion.png");
	potion.setShader(shaders[1]);
	glm::vec3 pscale(32.0, 32.0, 1.0);
	potion.inicializar(potionID, pscale, 1, 1);

	//Inicializa a boxdoor
	GLuint boxID = loadTexture("../textures/box.png");
	boxdoor.setShader(shaders[1]);
	glm::vec3 bscale(32.0, 32.0, 1.0);
	boxdoor.inicializar(boxID, bscale, 1, 1);

	////Inicializa a trapdoor
	//GLuint trapID = loadTexture("../textures/trapdoor.png");
	//trapdoor.setShader(shaders[1]);
	//glm::vec3 tscale(64.0, 32.0, 1.0);
	//trapdoor.inicializar(trapID, tscale, 1, 1);
	
	//Inicializando pos do personagem
	Plvl = 1;
	startx = poslinha = 2;
	starty = poscoluna = 7;
	dir = PARADO;
	status = PLAYING;

	loadingLevel(Plvl);
}

void SceneManager::setupCamera2D()
{
	glViewport(0, 0, width, height);

	//Setando o tamanho da janela do mundo
	float xMin = 0.0, xMax = 800, yMin = 600, yMax = 0.0, zNear = -1.0, zFar = 1.0;
	projection = glm::ortho(xMin, xMax, yMin, yMax, zNear, zFar);

	// Passando para os shaders que a usam
	shaders[0]->Use();
	GLint projLoc = glGetUniformLocation(shaders[0]->ID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	shaders[1]->Use();
	projLoc = glGetUniformLocation(shaders[1]->ID, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//Habilitando a profundidade
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_ALWAYS);

	//Habilitando a transparência
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int SceneManager::setupTexture(string filename, int &largura, int &altura, int &nroCanais)
{
	//Texture index
	unsigned int texture;

	// load and create a texture 
	// -------------------------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
	
	cout << "Nro de canais: " << nrChannels << endl;
	cout << "largura x altura: " << width << " x " << height << endl;

	if (data)
	{
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		altura = height;
		largura = width;
		nroCanais = nrChannels;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

	glActiveTexture(GL_TEXTURE0);

	//Talvez eu desloque isso depois
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return texture;
}

void SceneManager::loadingLevel(int playerlvl)
{
	string lvl;
	switch (playerlvl)
	{
	case 1:
		lvl = "lvl1.txt";
		break;
	case 2:
		lvl = "lvl2.txt";
		break;
	case 3:
		lvl = "lvl3.txt";
		break;
	default:

		break;
	}

	//Inicialização mapa
	loadLevel(lvl);

	//Copiando o mapa predefinido
	for (int i = 0; i < LINS; i++)
	{
		for (int j = 0; j < COLS; j++)
		{
			if (map[i][j] >= 5 || map[i][j] == 1)
			{
				mapwalk[i][j] = 0;
			}
			else if (map[i][j] == 2)
			{
				mapwalk[i][j] = 2;
			}
			else if (map[i][j] == 0)
			{
				mapwalk[i][j] = 3;
			}
			else
			{
				mapwalk[i][j] = 1;
			}
		}
	}
}

void SceneManager::loadLevel(string file)
{
	txtFile.open(file);
	if (txtFile.is_open())
	{
		for (int i = 0; i < LINS; i++)
		{
			for (int j = 0; j < COLS; j++)
			{
				txtFile >> map[i][j];
			}
		}
		txtFile.close();
	}
}
