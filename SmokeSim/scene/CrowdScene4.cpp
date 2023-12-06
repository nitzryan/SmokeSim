#include "CrowdScene4.h"

const int MAX_PEOPLE = 30;
const float AVERAGE_SPAWN_TIME = 2.0f;
const Material personMat = Material(ColorRGBA(0.8f, 0.5f, 0.2f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1);
const Material destMat = Material(ColorRGBA(0.8f, 0.8f, 0.8f, 0.5f), 1.0f, 0.0f, 0.0f, 10.0f, -1);

CrowdScene4::CrowdScene4() :
	mt(5611),
	map(Material(ColorRGBA(0.8f, 0.8f, 0.8f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1), mt, {
		CrowdDest(-5.0f, -2.0f, 6.25f, 5.0f, destMat, Pos2F(-3.5f, 5.2f), Pos2F(-3.5f, 4.8f)),
		CrowdDest(-2.0f, 2.0f, 6.25f, 5.0f, destMat, Pos2F(0.0f, 5.2f), Pos2F(0.0f, 4.8f)),
		CrowdDest(2.0f, 5.0f, 6.25f, 5.0f, destMat, Pos2F(3.5f, 5.2f), Pos2F(3.5f, 4.8f)),
		CrowdDest(-5.0f, -2.0f, -5.0f, -6.25f, destMat, Pos2F(-3.5f, -5.2f), Pos2F(-3.5f, -4.8f)),
		CrowdDest(-2.0f, 2.0f, -5.0f, -6.25f, destMat, Pos2F(0.0f, -5.2f), Pos2F(0.0f, -4.8f)),
		CrowdDest(2.0f, 5.0f, -5.0f, -6.25f, destMat, Pos2F(3.5f, -5.2f), Pos2F(3.5f, -4.8f)),
		CrowdDest(-6.25f, -5.0f, -2.0f, -5.0f, destMat, Pos2F(-5.2f, -3.5f), Pos2F(-4.8f, -3.5f)),
		CrowdDest(-6.25f, -5.0f, 2.0f, -2.0f, destMat, Pos2F(-5.2f, 0.0f), Pos2F(-4.8f, 0.0f)),
		CrowdDest(-6.25f, -5.0f, 5.0f, 2.0f, destMat, Pos2F(-5.2f, 3.5f), Pos2F(-4.8f, 3.5f)),
		CrowdDest(5.0f, 6.25f, -2.0f, -5.0f, destMat, Pos2F(5.2f, -3.5f), Pos2F(4.8f, -3.5f)),
		CrowdDest(5.0f, 6.25f, 2.0f, -2.0f, destMat, Pos2F(5.2f, 0.0f), Pos2F(4.8f, 0.0f)),
		CrowdDest(5.0f, 6.25f, 5.0f, 2.0f, destMat, Pos2F(5.2f, 3.5f), Pos2F(4.8f, 3.5f)),
		})
{
	people.reserve(MAX_PEOPLE);
	auto dest = map.GetRandomDest(mt);
	for (int i = 0; i < MAX_PEOPLE; i++) {
		people.emplace_back(dest->GetSpawnPos(mt), personMat, nullptr, dest, &map);
	}
}

void CrowdScene4::Render(Renderer& renderer)
{
	for (auto& i : people) {
		if (!i.ReadyToRecycle()) {
			renderer.Render(i);
		}
	}
	map.Render(renderer);
}

void CrowdScene4::Update(float dt)
{
	std::uniform_real_distribution<float> randPerson(0.0f, 1.0f);
	float cutoff = 1.0f - (dt / AVERAGE_SPAWN_TIME);

	// Calculate Movement
	for (auto& i : people) {
		i.CalculateDirection(dt);
	}

	// Adjust movement to avoid other people
	for (size_t i = 0; i < people.size(); i++) {
		if (people[i].ReadyToRecycle()) {
			continue;
		}

		people[i].ComputeTTC(people, i + 1, dt);
	}

	// Adjust movement to avoid walls
	auto walls = map.GetWalls();
	for (auto& i : people) {
		for (auto j : walls) {
			i.AvoidWall(*j, dt);
		}
	}

	// Move of create people
	for (size_t i = 0; i < people.size(); i++) {
		if (people[i].ReadyToRecycle()) { // Chance to spawn
			if (randPerson(mt) > cutoff) {
				Pos2F pos(0, 0);
				const CrowdNode* node;
				const CrowdDest* dest = map.GetSpawnConditions(mt, pos, node);
				people[i] = CrowdPerson(pos, personMat, node, dest, &map);
			}
		}
		else { // Move towards goal
			people[i].Move(dt);
		}
	}
}
