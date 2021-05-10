// antity.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>

#include "core/ECS/Registry.hpp"

using namespace ant;

int main()
{
	Registry reg;

	reg.RegisterComponent<int>();
	reg.RegisterComponent<float>();
	reg.RegisterComponent<char>();

	Entity entity = reg.CreateEntity();
	Entity other = reg.CreateEntity();

	reg.AddComponent<int>(entity, 1);
	reg.AddComponent<int>(other, 10);
	reg.AddComponent<float>(entity, .4f);
	reg.AddComponent<char>(other, 'c');

	int counter = 0;

	for (auto [e, i, f] : reg.GetComponents<int, float>())
	{

		counter++;
		f = .5f;
	}

	auto [j] = reg.GetEntityComponents<float>(entity);

}
