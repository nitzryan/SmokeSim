#include "SmokeGrid.h"

#include <iostream>
#include <iomanip>

const float SMOKE_FACTOR = 4.0f;
const float SIM_STEP_TIME = 0.01f;
const float DIFFUSION_FACTOR = 0.01f;
const float AMBIENT_TEMP = 280.0f;
const float FIRE_TEMP = 600.0f;
const float CONDUCTION = 0.2f;

SmokeGrid::SmokeGrid()
{
	w = 52;
	h = 52;
	gridSize = 0.1f;
	time = 0.f;
	smoke = std::vector<float>(w * h);
	
	
	temp = std::vector<float>(w * h, AMBIENT_TEMP);
	wind = Vec2F(0.3f, 0.2f);
	velX = std::vector<float>(w * h, wind.x);
	velY = std::vector<float>(w * h, wind.y);

	Material mat = Material(ColorRGBA(0.5f, 0.5f, 0.5f, 1.0f), 1.0f, 0.0f, 0.0f, 10.0f, -1);

	rects.reserve(w * h);
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			float lx = gridSize * i;
			float rx = lx + gridSize;
			float dy = gridSize * j;
			float uy = dy + gridSize;
			rects.emplace_back(Pos2F(lx, uy), Pos2F(rx, uy), Pos2F(lx, dy), Pos2F(rx, dy), mat);
		}
	}
}

void linearSolver(std::vector<float>& x, std::vector<float>& x0, float a, float c, int w, int h) {
	for (int k = 0; k < 10; k++) {
		for (int i = 1; i < w - 1; i++) {
			for (int j = 1; j < h - 1; j++) {
				int idx = i * h + j;
				int left = idx - h;
				int right = idx + h;
				int up = idx + 1;
				int down = idx - 1;
				x[idx] = (x0[idx] + a * (x[left] + x[right] + x[up] + x[down])) / c;
			}
		}
	}
}

void SmokeGrid::Update(float dt)
{
	time += dt;

	while (time > SIM_STEP_TIME) {
		// Sources
		for (int i = -2; i <= 2; i++) {
			smoke[((w / 2) + i) * h + 1] += SIM_STEP_TIME * SMOKE_FACTOR;
			velY[((w / 2) + i) * h + 1] = 5.0f;
		}
		
		for (int i = 0; i < h; i++) {
			velX[i] = wind.x;
			velX[w * h - i - 1] = wind.x;
			velY[i] = wind.y;
			velY[w * h - i - 1] = wind.y;
		}
		
		std::vector<float> nextSmoke = smoke;
		//std::vector<float> nextTemp = std::vector<float>(w * h);
		std::vector<float> nextVelX = velX;
		std::vector<float> nextVelY = velY;

		// Diffusion
		const float DIFFUSION_STEP_FACTOR = DIFFUSION_FACTOR * SIM_STEP_TIME / (gridSize * gridSize);
		//const float CONDUCTION_STEP_FACTOR = CONDUCTION * SIM_STEP_TIME;
		
		linearSolver(nextSmoke, smoke, DIFFUSION_STEP_FACTOR, 1 + 4 * DIFFUSION_STEP_FACTOR, w, h);
		linearSolver(nextVelX, velX, DIFFUSION_FACTOR, 1 + 4 * DIFFUSION_STEP_FACTOR, w, h);
		linearSolver(nextVelY, velY, DIFFUSION_FACTOR, 1 + 4 * DIFFUSION_STEP_FACTOR, w, h);

		//for (int k = 0; k < 10; k++) {
		//	for (int i = 1; i < w - 1; i++) {
		//		for (int j = 1; j < h - 1; j++) {
		//			// Calculate Indices
		//			
		//			
		//			// Density
		//			
		//			// Temperature
		//			//nextTemp[idx] = (temp[idx] + CONDUCTION_STEP_FACTOR * (nextTemp[left] + nextTemp[right] + nextTemp[up] + nextTemp[down])) / (1 + 4 * CONDUCTION_STEP_FACTOR);
		//			// Velocity
		//			nextVel[idx].x = (vel[idx].x + DIFFUSION_STEP_FACTOR * (nextVel[left].x + nextVel[right].x + nextVel[up].x + nextVel[down].x)) / (1 + 4 * DIFFUSION_STEP_FACTOR);
		//			nextVel[idx].y = (vel[idx].y + DIFFUSION_STEP_FACTOR * (nextVel[left].y + nextVel[right].y + nextVel[up].y + nextVel[down].y)) / (1 + 4 * DIFFUSION_STEP_FACTOR);
		//		}
		//	}

		//	//// Left/Right Edges
		//	//for (int j = 1; j < h - 1; j++) {
		//	//	// Left Edge
		//	//	int idx = j;
		//	//	int right = idx + h;
		//	//	int up = idx + 1;
		//	//	int down = idx - 1;
		//	//	next[idx] = (smoke[idx] + DIFFUSION_STEP_FACTOR * (next[right] + next[up] + next[down])) / (1 + 4 * DIFFUSION_STEP_FACTOR);
		//	//	// Right Edge
		//	//	idx = j + (w - 1) * h;
		//	//	int left = idx - h;
		//	//	up = idx + 1;
		//	//	down = idx - 1;
		//	//	next[idx] = (smoke[idx] + DIFFUSION_STEP_FACTOR * (next[left] + next[up] + next[down])) / (1 + 4 * DIFFUSION_STEP_FACTOR);
		//	//}

		//	//// Top/Bottom Edges
		//	//for (int i = 1; i < w - 1; i++) {
		//	//	// Top Edge
		//	//	int idx = i * h + (h - 1);
		//	//	int left = idx - h;
		//	//	int right = idx + h;
		//	//	int down = idx - 1;
		//	//	next[idx] = (smoke[idx] + DIFFUSION_STEP_FACTOR * (next[left] + next[right] + next[down])) / (1 + 4 * DIFFUSION_STEP_FACTOR);
		//	//	// Bottom edge, no diffusion out of bottom
		//	//	idx = i * h;
		//	//	left = idx - h;
		//	//	right = idx + h;
		//	//	int top = idx;
		//	//	next[idx] = (smoke[idx] + DIFFUSION_STEP_FACTOR * (next[left] + next[right] + next[top])) / (1 + 3 * DIFFUSION_STEP_FACTOR);
		//	//}

		//	//// Corners
		//	//next[0] = (smoke[0] + DIFFUSION_STEP_FACTOR * (next[1] + next[h])) / (1 + 3 * DIFFUSION_STEP_FACTOR);
		//	//next[h - 1] = (smoke[h - 1] + DIFFUSION_STEP_FACTOR * (next[h - 2] + next[2 * h - 1])) / (1 + 4 * DIFFUSION_STEP_FACTOR);
		//	//next[(w - 1) * h] = (smoke[(w - 1) * h] + DIFFUSION_STEP_FACTOR * (next[(w - 2) * h] + next[(w - 1) * h + 1])) / (1 + 3 * DIFFUSION_STEP_FACTOR);
		//	//int idx = w * h - 1;
		//	//next[idx] = (smoke[idx] + DIFFUSION_STEP_FACTOR * (next[idx - 1] + next[idx - h])) / (1 + 4 * DIFFUSION_STEP_FACTOR);
		//}

		// Advection
		for (int i = 1; i < w - 1; i++) {
			for (int j = 1; j < h - 1; j++) {
				// Find location of particle that will advect to center
				int idx = i * h + j;
				float x = i * gridSize - SIM_STEP_TIME * (nextVelX[idx]);
				float y = j * gridSize - SIM_STEP_TIME * (nextVelY[idx]);
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
		std::vector<float> div = std::vector<float>(w * h);
		std::vector<float> p = std::vector<float>(w * h);
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
		linearSolver(p, div, 1, 4, w, h);
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
		time -= SIM_STEP_TIME;
	}
}

void SmokeGrid::Render(Renderer& renderer)
{
	// Update transparency with amount of smoke
	for (size_t i = 0; i < rects.size(); i++) {
		rects[i].SetTransparency(std::min(smoke[i] * SMOKE_FACTOR, 1.0f));
	}

	for (auto& i : rects) {
		renderer.Render(i);
	}
}

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
