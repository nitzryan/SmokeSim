#include "SmokeGrid.h"

#include <iostream>
#include <fstream>
#include <iomanip>

#include <cstdlib> // Rand

#include "../rendering/BufferWriter.h"

const double DIFFUSION_FACTOR = 0.01;
const float AMBIENT_TEMP = 280.0f;
const double CONDUCTION = 0.002;
const float MIN_TEMP_FIRE = 600.0f;
const float O2_TO_TEMP = 100.0f;
//const double STEF_BOLT_CONST = 0.0000000567;
const double STEF_BOLT_CONST = 0.00000000567;
const float LIFT_FACTOR = 0.15f;
const float TEMP_EXPAND_FACTOR = 0.04f;
const int MAX_SWIRLS = 80;
const float SPAWN_SWIRL_CHANCE = 0.75f;

SmokeGrid::SmokeGrid(int approxTiles, const SmokeSetup& set, float st)
{
	srand(5611);

	setup = set;
	float aspectRatio = (float)(setup.width) / setup.height;
	w = sqrt((float)approxTiles * aspectRatio);
	h = w / aspectRatio;

	gridSize = setup.width / (w - 1);
	time = 0.f; 
	stepTime = st;
	totalSimTime = 0.f;
	smoke = std::vector<double>(w * h);
	
	temp = std::vector<double>(w * h, AMBIENT_TEMP);
	wind = setup.wind;
	velX = std::vector<double>(w * h, wind.x);
	velY = std::vector<double>(w * h, wind.y);

	//rects.reserve(w * h);
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			float lx = gridSize * i;
			float rx = lx + gridSize;
			float dy = gridSize * j;
			float uy = dy + gridSize;
		}
	}

	// Initialize sources
	for (auto& s : setup.sources) {
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				float x = i * gridSize;
				float y = j * gridSize;
				if (x > s.x0 && x < s.x1 && y > s.y0 && y < s.y1) {
					sources.emplace_back(sourceBlock{
						i * h + j,
						s.vel,
						s.intensity,
						s.heat
						});
					velX[i * h + j] = s.vel.x;
					velY[i * h + j] = s.vel.y;
				}
			}
		}
	}

	// Initialize Obstactles
	for (auto& c : setup.circles) {
		for (int i = 0; i < w; i++) {
			for (int j = 0; j < h; j++) {
				float x = i * gridSize;
				float y = j * gridSize;
				float dx = c.x0 - x;
				float dy = c.y0 - y;
				float r = sqrtf(dx * dx + dy * dy);
				if (r < c.r) {
					// Check 4 edges
					dx = dx + gridSize;
					r = sqrtf(dx * dx + dy * dy);
					bool rightClosed = r < c.r;
					dx -= 2 * gridSize;
					r = sqrtf(dx * dx + dy * dy);
					bool leftClosed = r < c.r;
					dx += gridSize;
					dy += gridSize;
					r = sqrtf(dx * dx + dy * dy);
					bool upClosed = r < c.r;
					dy -= 2 * gridSize;
					r = sqrtf(dx * dx + dy * dy);
					bool downClosed = r < c.r;

					// Set Direction
					ObstacleDirection dir;
					if (!upClosed && rightClosed && leftClosed && downClosed) {
						dir = ObstacleDirection::U;
					}
					else if (!upClosed && !rightClosed && leftClosed && downClosed) {
						dir = ObstacleDirection::UR;
					}
					else if (!upClosed && rightClosed && !leftClosed && downClosed) {
						dir = ObstacleDirection::UL;
					}
					else if (upClosed && rightClosed && !leftClosed && downClosed) {
						dir = ObstacleDirection::L;
					}
					else if (upClosed && rightClosed && !leftClosed && !downClosed) {
						dir = ObstacleDirection::DL;
					}
					else if (upClosed && rightClosed && leftClosed && !downClosed) {
						dir = ObstacleDirection::D;
					}
					else if (upClosed && !rightClosed && leftClosed && !downClosed) {
						dir = ObstacleDirection::DR;
					}
					else if (upClosed && !rightClosed && leftClosed && downClosed) {
						dir = ObstacleDirection::R;
					}
					else if (upClosed && rightClosed && leftClosed && downClosed) {
						dir = ObstacleDirection::INT;
					}
					else {
						std::cout << "Error: obstacle size too small, ignored\n";
						continue;
					}
					obstacles.emplace_back(Obstacle{
						i * h + j,
						dir
						});
				}
			}
		}
	}

	tileIsObstacle = std::vector<bool>(w * h, false);

	for (auto& r : setup.rects) {
		for (int i = 1; i < w - 1; i++) {
			for (int j = 1; j < h - 1; j++) {
				float x = i * gridSize;
				float y = j * gridSize;
				if (x >= r.x0 && x <= r.x1 && y >= r.y0 && y <= r.y1) {
					tileIsObstacle[i * h + j] = true;
				}
			}
		}
	}

	for (int i = 1; i < w - 1; i++) {
		for (int j = 1; j < h - 1; j++) {
			int idx = i * h + j;
			if (tileIsObstacle[idx]) {
				bool upClosed = tileIsObstacle[idx + 1];
				bool downClosed = tileIsObstacle[idx - 1];
				bool rightClosed = tileIsObstacle[idx + h];
				bool leftClosed = tileIsObstacle[idx - h];

				ObstacleDirection dir;
				if (!upClosed && rightClosed && leftClosed && downClosed) {
					dir = ObstacleDirection::U;
				}
				else if (!upClosed && !rightClosed && leftClosed && downClosed) {
					dir = ObstacleDirection::UR;
				}
				else if (!upClosed && rightClosed && !leftClosed && downClosed) {
					dir = ObstacleDirection::UL;
				}
				else if (upClosed && rightClosed && !leftClosed && downClosed) {
					dir = ObstacleDirection::L;
				}
				else if (upClosed && rightClosed && !leftClosed && !downClosed) {
					dir = ObstacleDirection::DL;
				}
				else if (upClosed && rightClosed && leftClosed && !downClosed) {
					dir = ObstacleDirection::D;
				}
				else if (upClosed && !rightClosed && leftClosed && !downClosed) {
					dir = ObstacleDirection::DR;
				}
				else if (upClosed && !rightClosed && leftClosed && downClosed) {
					dir = ObstacleDirection::R;
				}
				else if (upClosed && rightClosed && leftClosed && downClosed) {
					dir = ObstacleDirection::INT;
				}
				else {
					std::cout << "Error: obstacle size too small, ignored\n";
					continue;
				}
				obstacles.emplace_back(Obstacle{
					i * h + j,
					dir
					});
			}
		}
	}

	tileIsObstacle = std::vector<bool>(w * h, false);
	for (auto& i : obstacles) {
		tileIsObstacle[i.tile] = true;
	}

	tileIsSource = std::vector<bool>(w * h, false);
	for (auto& i : sources) {
		tileIsSource[i.tile] = true;
	}

	swirls.reserve(MAX_SWIRLS);
	for (int i = 0; i < MAX_SWIRLS; i++) {
		swirls.emplace_back(Swirl{ 0, 0, 0, 0, 0, 0 });
	}
}

void SmokeGrid::linearSolver(std::vector<double>& x, std::vector<double>& x0, float a, float c, int w, int h, SolverType st) {
	for (int k = 0; k < 2; k++) {
		for (int i = 1; i < w - 1; i++) {
			for (int j = 1; j < h - 1; j++) {
				int idx = i * h + j;
				if (tileIsObstacle[idx]) {
					continue;
				}
				int left = idx - h;
				int right = idx + h;
				int up = idx + 1;
				int down = idx - 1;
				x[idx] = (x0[idx] + a * (x[left] + x[right] + x[up] + x[down])) / c;
			}
		}
	}

	// Set Obstacle values
	switch (st) {
	case SolverType::DENSITY:
		for (auto& i : obstacles) {
			switch (i.dir) {
			case ObstacleDirection::UL:
				x[i.tile] = (x[i.tile - h] + x[i.tile + 1]) / 2;
				break;
			case ObstacleDirection::L:
				x[i.tile] = x[i.tile - h];
				break;
			case ObstacleDirection::DL:
				x[i.tile] = (x[i.tile - h] + x[i.tile - 1]) / 2;
				break;
			case ObstacleDirection::U:
				x[i.tile] = x[i.tile + 1];
				break;
			case ObstacleDirection::UR:
				x[i.tile] = (x[i.tile + 1] + x[i.tile + h]) / 2;
				break;
			case ObstacleDirection::R:
				x[i.tile] = x[i.tile + h];
				break;
			case ObstacleDirection::DR:
				x[i.tile] = (x[i.tile + h] + x[i.tile - 1]) / 2;
				break;
			case ObstacleDirection::D:
				x[i.tile] = x[i.tile - 1];
				break;
			case ObstacleDirection::INT:
				break;
			}
		}
		break;
	case SolverType::VEL_X:
		for (auto& i : obstacles) {
			switch (i.dir) {
			case ObstacleDirection::DL:
			case ObstacleDirection::L:
			case ObstacleDirection::UL:
				x[i.tile] = -x[i.tile - h];
				break;
			case ObstacleDirection::DR:
			case ObstacleDirection::R:
			case ObstacleDirection::UR:
				x[i.tile] = -x[i.tile + h];
				break;
			default:
				break;
			}
			//x[i.tile] = 0;
		}
		break;
	case SolverType::VEL_Y:
		for (auto& i : obstacles) {
			switch (i.dir) {
			case ObstacleDirection::DL:
			case ObstacleDirection::D:
			case ObstacleDirection::DR:
				x[i.tile] = -x[i.tile - 1];
				break;
			case ObstacleDirection::UR:
			case ObstacleDirection::U:
			case ObstacleDirection::UL:
				x[i.tile] = -x[i.tile + 1];
				break;
			default:
				break;
			}
			//x[i.tile] = 0;
		}
		break;
	}
}

float SpawnRandomness(float x, float y, float time) {
	float result = 1.0f;
	result += 0.1f * sinf(time + 0.5f * x + 0.2f * y);
	result += 0.05f * sinf(2 * time + 2.0f * x + 1.0f * y);
	result += 0.1f * sinf(0.25 * time);
	return result;
}

void MaybeSpawnSwirl(Swirl& s, float stepTime) {
	float p = SPAWN_SWIRL_CHANCE * stepTime;
	int r = rand() % 10000;
	if (p * 1000 > r) {
		s.r = 0.04f + 0.04f * (float)(rand()) / RAND_MAX;
		s.rmax = s.r * 1.25f;
		s.w = -10.0f + 20.0f * (float)(rand()) / RAND_MAX;
		s.tleft = 0.3f + 0.6f * (float)(rand()) / RAND_MAX;
		s.x = 1.6f + 0.8f * (float)(rand()) / RAND_MAX;
		s.y = 0.4f + 0.6f * (float)(rand()) / RAND_MAX;
	}
}

void SmokeGrid::UpdateStep(float stepTime)
{
	// Radiate away temperature
	for (auto& i : temp) {
		if (i < AMBIENT_TEMP) { // Allows for simplification below, otherwise system can blow up if temp falls below ambient
								// Due to heat radiating away from calculation
			i = AMBIENT_TEMP;
			continue;
		}
		double tempDelta = i - AMBIENT_TEMP;
		i -= STEF_BOLT_CONST * tempDelta * tempDelta * tempDelta * tempDelta * stepTime;
	}

	// Sources
	for (auto& i : sources) {
		float x = i.tile / h * gridSize;
		float y = i.tile % h * gridSize;
		float randFactor = SpawnRandomness(x, y, totalSimTime);
		double added = stepTime * i.intensity * gridSize * gridSize * randFactor;
		smoke[i.tile] += added;
		//velX[i.tile] = 0.9f * velX[i.tile] + 0.1f * i.vel.x;
		//velY[i.tile] = 0.9f * velY[i.tile] + 0.1f * i.vel.y;
		added = stepTime * i.heat;
		temp[i.tile] = i.heat * randFactor;
	}

	// Enforce left, right boundary conditions
	for (int i = 0; i < h; i++) {
		velX[i] = wind.x;
		velX[w * h - i - 1] = wind.x;
		velY[i] = wind.y;
		velY[w * h - i - 1] = wind.y;
	}
	// Enforce top boundary conditions
	for (int i = 0; i < w; i++) {
		//velX[i * h + h - 1] = velX[i * h + h - 2];
		velY[i * h + h - 1] = velY[i * h + h - 2];
	}

	// Apply Swirls
	for (auto& i : swirls) {
		if (i.tleft > 0) {
			i.tleft -= stepTime;

			for (int j = 1; j < w - 1; j++) {
				float x = j * gridSize;
				float dx = x - i.x;
				if (abs(dx) > i.rmax) {
					continue;
				}
				float dx2 = dx * dx;

				for (int k = 1; k < h - 1; k++) { // Don't check above certain point
					float y = k * gridSize;
					float dy = y - i.y;
					if (abs(dy) > i.rmax) {
						continue;
					}
					float d = sqrt(dx2 + dy * dy);
					if (d > i.rmax) {
						continue;
					}

					float mag = 1.0f;
					if (d > i.r) {
						mag = (d - i.r) / (i.rmax - i.r);
					}

					// Determine velocity of swirl, need to swirl relative to

					float tileX = i.x / gridSize;
					float tileY = i.y / gridSize;
					int left = (int)tileX;
					int right = left + 1;
					int down = (int)tileY;
					int up = down + 1;
					float magX = tileX - left;
					float magY = tileY - down;

					float uSwirl = velX[h * left + down] * (1 - magX) * (1 - magY) +
						velX[h * left + up] * (1 - magX) * magY +
						velX[h * right + down] * magX * (1 - magY) +
						velX[h * right + up] * magX * magY;
					float vSwirl = velY[h * left + down] * (1 - magX) * (1 - magY) +
						velY[h * left + up] * (1 - magX) * magY +
						velY[h * right + down] * magX * (1 - magY) +
						velY[h * right + up] * magX * magY;

					int idx = j * h + k;
					velX[idx] += (uSwirl - dy * i.w - velX[idx]) * mag;
					velY[idx] += (vSwirl + dx * i.w - velY[idx]) * mag;
				}
			}
		}
		else {
			MaybeSpawnSwirl(i, stepTime);
		}
	}

	std::vector<double> nextSmoke = smoke;
	std::vector<double> nextTemp = temp;
	std::vector<double> nextVelX = velX;
	std::vector<double> nextVelY = velY;

	// Modify velocity based on density (inversely proportional to temperature)
	for (int i = 1; i < w - 1; i++) {
		for (int j = 1; j < h - 1; j++) {
			int idx = i * h + j;
			
			float denBelow = temp[idx - 1];
			float den = temp[idx];
			float denAbove = temp[idx + 1];
			float denRight = temp[idx + h];
			float denLeft = temp[idx - h];

			velY[idx] += LIFT_FACTOR * stepTime * (denBelow - den);
			velY[idx] += LIFT_FACTOR * stepTime * (den - denAbove);
			
			velX[idx] += TEMP_EXPAND_FACTOR * stepTime * (denLeft - den);
			velX[idx] += TEMP_EXPAND_FACTOR * stepTime * (den - denRight);
		}
	}

	// Diffusion
	const float DIFFUSION_STEP_FACTOR = DIFFUSION_FACTOR * stepTime / ((double)gridSize * gridSize);
	const float CONDUCTION_STEP_FACTOR = CONDUCTION * stepTime / ((double)gridSize * gridSize);

	linearSolver(nextSmoke, smoke, DIFFUSION_STEP_FACTOR, 1 + 4 * DIFFUSION_STEP_FACTOR, w, h, SolverType::DENSITY);
	linearSolver(nextTemp, temp, CONDUCTION_STEP_FACTOR, 1 + 4 * CONDUCTION_STEP_FACTOR, w, h, SolverType::DENSITY);
	linearSolver(nextVelX, velX, DIFFUSION_STEP_FACTOR, 1 + 4 * DIFFUSION_STEP_FACTOR, w, h, SolverType::VEL_X);
	linearSolver(nextVelY, velY, DIFFUSION_STEP_FACTOR, 1 + 4 * DIFFUSION_STEP_FACTOR, w, h, SolverType::VEL_Y);

	// Advection
	for (int i = 1; i < w - 1; i++) {
		for (int j = 1; j < h - 1; j++) {
			// Find location of particle that will advect to center
			int idx = i * h + j;
			float x = i * gridSize - stepTime * (nextVelX[idx]);
			float y = j * gridSize - stepTime * (nextVelY[idx]);
			x /= gridSize;
			y /= gridSize;
			if (x < 0.5) x = 0.5;
			else if (x > w - 1.5) x = w - 1.5f;
			if (y < 0.5) y = 0.5;
			else if (y > h - 1.5) y = h - 1.5f;
			// Find squares and fractions of squares
			int lowX = (int)x;
			int highX = lowX + 1;
			int lowY = (int)y;
			int highY = lowY + 1;
			float fracX = x - lowX;
			float fracY = y - lowY;
			float fracll = (1 - fracX) * (1 - fracY);
			float fraclr = fracX * (1 - fracY);
			float fracul = (1 - fracX) * fracY;
			float fracur = fracX * fracY;
			int blIdx = lowX * h + lowY;
			int brIdx = blIdx + h;
			int ulIdx = blIdx + 1;
			int urIdx = brIdx + 1;
			smoke[idx] = fracll * nextSmoke[blIdx] + fraclr * nextSmoke[brIdx] + fracul * nextSmoke[ulIdx] + fracur * nextSmoke[urIdx];
			temp[idx] = fracll * nextTemp[blIdx] + fraclr * nextTemp[brIdx] + fracul * nextTemp[ulIdx] + fracur * nextTemp[urIdx];
			velX[idx] = fracll * nextVelX[blIdx] + fraclr * nextVelX[brIdx] + fracul * nextVelX[ulIdx] + fracur * nextVelX[urIdx];
			velY[idx] = fracll * nextVelY[blIdx] + fraclr * nextVelY[brIdx] + fracul * nextVelY[ulIdx] + fracur * nextVelY[urIdx];
		}
	}

	// Velocity Projection
	std::vector<double> div = std::vector<double>(w * h);
	std::vector<double> p = std::vector<double>(w * h);
	for (int i = 1; i < w - 1; i++) {
		for (int j = 1; j < h - 1; j++) {
			int idx = i * h + j;
			int left = idx - h;
			int right = idx + h;
			int up = idx + 1;
			int down = idx - 1;
			div[idx] = -0.5 * gridSize * (velX[right] - velX[left] + velY[up] - velY[down]);
		}
	}
	linearSolver(p, div, 1, 4, w, h, SolverType::DENSITY);
	for (int i = 1; i < w - 1; i++) {
		for (int j = 1; j < h - 1; j++) {
			int idx = i * h + j;
			int left = idx - h;
			int right = idx + h;
			int up = idx + 1;
			int down = idx - 1;
			velX[idx] -= 0.5 * (p[right] - p[left]) / gridSize;
			velY[idx] -= 0.5 * (p[up] - p[down]) / gridSize;
		}
	}

	// Move to next timestep
	time -= stepTime;
	totalSimTime += stepTime;
}

void SmokeGrid::Update(float dt)
{
	time += dt;

	while (time > stepTime) {
		UpdateStep(stepTime);
	}
}

/*
void SmokeGrid::Render(Renderer& renderer)
{
	// Update transparency with amount of smoke
	for (size_t i = 0; i < rects.size(); i++) {
		rects[i].SetTransparency(smoke[i] / (1 + smoke[i]));
	}

	for (auto& i : rects) {
		renderer.Render(i);
	}
}
*/

void ScreenCoordsToSimCoords(float& x, float& y) {
	// Convert from 0..1 to -1..-1
	x *= 2;
	x -= 1;
	y *= -2;
	y += 1;

	// Convert to size of scene
	x *= 1.25;
	y *= 1.25;
}

void SmokeGrid::MouseDown(float x, float y)
{
	ScreenCoordsToSimCoords(x, y);
	wind = Vec2F(2.0f * x, 2.0f * y);
	std::cout << std::fixed << std::setprecision(2) << "New Wind Direction: " << x << " , " << y << std::endl;
}

void SmokeGrid::MouseUp()
{
}

void SmokeGrid::MouseMove(float x, float y)
{
}

int SmokeGrid::NumIndices() const
{
	return 6 * (w - 1) * (h - 1);
}

int SmokeGrid::NumPoints() const
{
	return w * h;
}

void SmokeGrid::Render(std::vector<float>& vbo, unsigned int vboLoc, unsigned int pointCount, std::vector<unsigned int>& ebo, unsigned int eboLoc)
{
	Vec3F normal = Vec3F(0, 0, 1);

	int n = 0;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			if (tileIsObstacle[n]) {
				BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), ColorRGBA(0.9f, 0.2f, 0.2f, 1.0f), normal, -1, -1);
			}
			else if (tileIsSource[n]) {
				BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), ColorRGBA(0.4f, 0.2f, 0.1f, 1.0f), normal, -1, -1);
			}
			else {
				float modSmoke = smoke[n] / ((double)gridSize * gridSize);
				float alpha = modSmoke / (3 + modSmoke);
				ColorRGBA color(0.0f, 0.0f, 0.0f, alpha);

				// Note temperatures are in kelvin
				float tm = temp[n];
				if (tm > 1300) { // yellow -> white flame
					float c = std::min(0.8f + (tm - 1300) * 0.01f, 1.0f);
					color.r = c;
					color.g = c;
					color.b = 0.1f + std::min((tm - 1300) * 0.001f, 0.9f);
				}
				else if (tm > 1000) { // orange -> yellow flame
					color.r = 0.8f;
					color.g = 0.4f + 0.4f * (tm - 1000.0f) / 300.0f;
					color.b = 0.1f;
				}
				else if (tm > 800) { // smoke -> orange
					color.r = 0.1f + 0.7f * (tm - 800.0f) / 200.0f;
					color.g = 0.1f + 0.3f * (tm - 800.0f) / 200.0f;
					color.b = 0.1f;
				}
				else {
					float c = 0.1f * std::max((tm - 400.0f) / 400.0f, 0.0f);
					color.r = c;
					color.g = c;
					color.b = c;
				}
				//alpha = modTemp;
				BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), color, normal, -1, -1);
				//BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), ColorRGBA(1.0f, 1.0f, 1.0f, alpha), normal, -1, -1);
			}
			
			n++;
		}
	}

	for (int i = 0; i < w - 1; i++) {
		for (int j = 0; j < h - 1; j++) {
			// Triangle Right, Right Up
			ebo[eboLoc] = pointCount + i * h + j;
			eboLoc++;
			ebo[eboLoc] = pointCount + (i + 1) * h + j;
			eboLoc++;
			ebo[eboLoc] = pointCount + (i + 1) * h + j + 1;
			eboLoc++;
			// Triangle Right Up, Up
			ebo[eboLoc] = pointCount + i * h + j;
			eboLoc++;
			ebo[eboLoc] = pointCount + (i + 1) * h + j + 1;
			eboLoc++;
			ebo[eboLoc] = pointCount + i * h + j + 1;
			eboLoc++;
		}
	}
}

Material SmokeGrid::GetMaterial() const
{
	Material mat = Material(ColorRGBA(0, 0, 0, 1), 1.0f, 0.0f, 0.0f, 10.0f, -1);
	return mat;
}

void SmokeGrid::CalculateToFile(int approxTiles, const SmokeSetup& setup, float time, float stepTime, const char* filename)
{
	std::cout << "Starting Tiles: " << approxTiles << " Time Step: " << stepTime << std::endl;
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cout << "File " << filename << " failed to create\n";
		return;
	}
	
	SmokeGrid grid(approxTiles, setup, stepTime);
	file << grid.w << "," << grid.h << "\n";
	file << grid.gridSize << "\n";
	file << stepTime << "\n";
	for (auto i : grid.tileIsObstacle) {
		if (i) {
			file << "1";
		}
		else {
			file << "0";
		}
		
		file << ",";
	}
	file << "\n";

	grid.time = time;
	int iter = 1;
	if (stepTime == 0.001f)
		iter = 10;

	for (int i = 0; i < 1000; i++) {
		for (int n = 0; n < iter; n++) {
			grid.UpdateStep(stepTime);
		}
		
		for (int w = 0; w < grid.w; w++) {
			for (int h = 0; h < grid.h; h++) {
				file << grid.smoke[w * grid.h + h];
				file << ",";
			}
		}
		file << "\n";
	}
	file.close();
}
