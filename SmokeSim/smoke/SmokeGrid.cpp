#include "SmokeGrid.h"

#include <iostream>
#include <fstream>
#include <iomanip>

#include "../rendering/BufferWriter.h"

const double DIFFUSION_FACTOR = 0.001;
const double CONDUCTION = 0.00001;

SmokeGrid::SmokeGrid(int approxTiles, const SmokeSetup& set, float st)
{
	setup = set;
	float aspectRatio = (float)(setup.width) / setup.height;
	w = sqrt((float)approxTiles * aspectRatio);
	h = w / aspectRatio;

	gridSize = setup.width / (w - 1);
	time = 0.f; stepTime = st;
	smoke = std::vector<double>(w * h);
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
						s.heat,
						s.fuel
						});
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
	for (auto& i : obstacles) {
		tileIsObstacle[i.tile] = true;
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

void SmokeGrid::UpdateStep(float stepTime)
{
	// Sources
	for (auto& i : sources) {
		double added = stepTime * i.intensity * gridSize * gridSize;
		smoke[i.tile] += added;
		velX[i.tile] = i.vel.x;
		velY[i.tile] = i.vel.y;
	}

	for (int i = 0; i < h; i++) {
		velX[i] = wind.x;
		velX[w * h - i - 1] = wind.x;
		velY[i] = wind.y;
		velY[w * h - i - 1] = wind.y;
	}

	for (int i = 0; i < w; i++) {
		velY[i * h + h - 1] = velY[i * h + h - 2];
	}

	std::vector<double> nextSmoke = smoke;
	std::vector<double> nextVelX = velX;
	std::vector<double> nextVelY = velY;

	// Diffusion
	const float DIFFUSION_STEP_FACTOR = DIFFUSION_FACTOR * stepTime / ((double)gridSize * gridSize);
	const float CONDUCTION_STEP_FACTOR = CONDUCTION * stepTime / ((double)gridSize * gridSize);

	linearSolver(nextSmoke, smoke, DIFFUSION_STEP_FACTOR, 1 + 4 * DIFFUSION_STEP_FACTOR, w, h, SolverType::DENSITY);
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
			else {
				float modSmoke = smoke[n] / ((double)gridSize * gridSize);
				float alpha = modSmoke / (1 + modSmoke);
				alpha *= alpha;
				//alpha = modTemp;
				BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), ColorRGBA(0.6f, 0.6f, 0.6f, alpha), normal, -1, -1);
				//BufferWriter::AddPoint(vbo, vboLoc, Pos2F(i * gridSize, j * gridSize), ColorRGBA(1.0f, 0.0f, 0.0f, alpha), normal, -1, -1);
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

void SmokeGrid::CalculateToFile(int approxTiles, const SmokeSetup& setup, float time, int stepsPerRender, const char* filename)
{
	float stepTime = 0.01f / stepsPerRender;
	
	std::cout << "Starting Tiles: " << approxTiles << " Time Step: " << stepTime << std::endl;
	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cout << "File " << filename << " failed to create\n";
		return;
	}

	SmokeGrid grid(approxTiles, setup, stepTime);
	file << grid.w << "," << grid.h << "\n";
	file << grid.gridSize << "\n";
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

	for (int i = 0; i < 1000; i++) {
		for (int n = 0; n < stepsPerRender; n++) {
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
