#pragma once

#include "define.h"
#include "ChunkManager.h"

class Ray {
private:
	int max_limit = 10;

public:
	int X = 0, Y = 0, Z = 0;
	int stepX = 0, stepY = 0, stepZ = 0;
	float tMaxX = 0, tMaxY = 0, tMaxZ = 0;
	float tDeltaX = 0, tDeltaY = 0, tDeltaZ = 0;
	
	glm::vec3 pos, direction_normalized;

	ChunkManager *chunk_manager;

public:
	void Init(ChunkManager *chunk_manager) {
		this->chunk_manager = chunk_manager;
	}

	float Intbound(float s, float ds) {
		return ((ds > 0) ? ceil(s)-s: s-floor(s)) / abs(ds);
	}

	void Update(glm::vec3 &pos, glm::vec3 &dir, float radius, uint8_t &face) {
		float dx = dir.x;
		float dy = dir.y;
		float dz = dir.z;

		stepX = (dx > 0) ? 1: (dx < 0) ? -1: 0;
		stepY = (dy > 0) ? 1: (dy < 0) ? -1: 0;
		stepZ = (dz > 0) ? 1: (dz < 0) ? -1: 0;

		// std::cout << stepX << ' ' << stepY << ' ' << stepZ << '\n';

		X = (stepX < 0) ? ceil(pos.x): floor(pos.x) - 1;
		Y = (stepY < 0) ? ceil(pos.y): floor(pos.y) - 1;
		Z = (stepZ < 0) ? ceil(pos.z): floor(pos.z) - 1;

		tMaxX = Intbound(stepX, dx);
		tMaxY = Intbound(stepY, dy);
		tMaxZ = Intbound(stepZ, dz);

		tDeltaX = (float)stepX/dx;
		tDeltaY = (float)stepY/dy;
		tDeltaZ = (float)stepZ/dz;

		radius /= sqrt(dx*dx + dy*dy + dz*dz);

		while (true) {
			if (dx == 0 && dy == 0 && dz == 0) break;

			if (chunk_manager->BlockSolid(glm::ivec3(X, Y, Z))) {
				// if (stepX > 0) X -= 1;
				// if (stepY > 0) Y -= 1;
				// if (stepZ > 0) Z -= 1;
				break;
			}

			if(tMaxX < tMaxY) {
				if(tMaxX < tMaxZ) {
					if (tMaxX > radius) break;

					X += stepX;
					tMaxX += tDeltaX;

					if (stepX) {
						face = 0b010000;
					} else {
						face = 0b100000;
					}
				} else {
					if (tMaxZ > radius) break;

					Z += stepZ;
					tMaxZ += tDeltaZ;

					if (stepZ) {
						face = 0b000010;
					} else {
						face = 0b000001;
					}
				}
			} else {
				if(tMaxY < tMaxZ) {
					if (tMaxY > radius) break;

					Y += stepY;
					tMaxY += tDeltaY;

					if (stepY) {
						face = 0b000100;
					} else {
						face = 0b001000;
					}
				} else {
					if (tMaxZ > radius) break;

					Z += stepZ;
					tMaxZ += tDeltaZ;

					if (stepZ) {
						face = 0b000010;
					} else {
						face = 0b000001;
					}
				}
			}
		}
	}
};