/* Hello Tilemap Isométrico
 *
 * Adaptado por Rossana Baptista Queiroz
 * para as disciplinas de Computação Gráfica - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 11/06/2022
 */

using namespace std;

#include "SceneManager.h"


// The MAIN function, from here we start the application and run the game loop
int main()
{
	SceneManager* scene = new SceneManager;
	scene->initialize(800, 600);

	scene->run();

	scene->finish();

	return 0;
}


