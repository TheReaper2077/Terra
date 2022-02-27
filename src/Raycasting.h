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
	
	glm::vec3 camera_pos, direction_normalized;

	ChunkManager *chunk_manager;

public:
	void Init(ChunkManager *chunk_manager) {
		this->chunk_manager = chunk_manager;
	}

	float Intbound(int s, float ds) {
		if (ds < 0) return Intbound(-s, -ds);
		s = mod(s, 1);
		return (float)(1 - s)/(float)ds;
	}

	float mod(int value, int modulus) {
		return (value % modulus + modulus) % modulus;
	}

	// // function intbounds(s,ds) { return (ds > 0? Math.ceil(s)-s: s-Math.floor(s)) / Math.abs(ds); }
	// int ceil(float value) {
	// 	// if ((int)value < value) return (int)value + 1;
	// 	if (value < 0) {
	// 		return (int)value;
	// 	}
	// 	return (int)value + 1;
	// }

	// int floor(float value) {
	// 	// if ((int)value < value) return (int)value - 1;
	// 	if (value < 0) {
	// 		return (int)value - 1;
	// 	}
	// 	return (int)value;
	// }

	// float Intbound(float s, float ds) {
	// 	return ((ds > 0) ? ceil(s)-s: s-floor(s)) / abs(ds);
	// }

	void Update(glm::vec3 &camera_pos, glm::vec3 &dir, float radius, uint8_t &face) {
		float dx = dir.x;
		float dy = dir.y;
		float dz = dir.z;

		stepX = dx/abs(dx);
		stepY = dy/abs(dy);
		stepZ = dz/abs(dz);

		X = (int)camera_pos.x;
		Y = (int)camera_pos.y;
		Z = (int)camera_pos.z;

		tMaxX = Intbound(stepX, dx);
		tMaxY = Intbound(stepY, dy);
		tMaxZ = Intbound(stepZ, dz);

		tDeltaX = stepX/dx;
		tDeltaY = stepY/dy;
		tDeltaZ = stepZ/dz;

		radius /= sqrt(dx*dx + dy*dy + dz*dz);

		while (true) {
			if (dx == 0 && dy == 0 && dz == 0) break;

			if (chunk_manager->BlockSolid(glm::ivec3(X, Y, Z))) {
				break;
			}

			face = 0;

			if(tMaxX < tMaxY) {
				if(tMaxX < tMaxZ) {
					if (tMaxX > radius) break;

					X += stepX;
					tMaxX += tDeltaX;

					if (stepX) {
						face |= 0b010000;
					} else {
						face |= 0b100000;
					}
				} else {
					if (tMaxZ > radius) break;

					Z += stepZ;
					tMaxZ += tDeltaZ;

					if (stepZ) {
						face |= 0b000001;
					} else {
						face |= 0b000010;
					}
				}
			} else {
				if(tMaxY < tMaxZ) {
					if (tMaxY > radius) break;

					Y += stepY;
					tMaxY += tDeltaY;

					if (stepY) {
						face |= 0b000100;
					} else {
						face |= 0b001000;
					}
				} else {
					if (tMaxZ > radius) break;

					Z += stepZ;
					tMaxZ += tDeltaZ;

					if (stepZ) {
						face |= 0b000001;
					} else {
						face |= 0b000010;
					}
				}
			}
		}
	}
};